// ======================================================================
// \title  BBSMessageStorage.cpp
// \author Marius Baden
// \brief  cpp file for BBSMessageStorage component implementation class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================
#include <string>
#include <vector>
#include <functional>
#include <regex>

#include <Os/File.hpp>
#include <Os/Directory.hpp>
#include <Os/FileSystem.hpp>

#include <Components/MessageStorage/MessageStorage.hpp>
#include "Types/FppConstantsAc.hpp"
// #include "config/MessageStorageCfg.hpp"
#include <Fw/Types/BasicTypes.hpp>
#include "Components/MessageStorage/MessageStorage.hpp"

namespace MessageModule 
{
	// CONFIG _ NEEDS TO BE MOVED TO CONFIG FILE
	enum { 
		MESSAGESTORAGE_INITIAL_INDEX = 0,
		MESSAGESTORAGE_MSGFILE_DELIMITER = 0xD9, 
		MESSAGESTORAGE_STORED_INDEX_HISTORY_SIZE = 10, // CHANGE 
		MESSAGESTORAGE_MSGFILE_NAME_MAXLENGTH = 15 // CHANGE
	}; 
	static const std::string MESSAGESTORAGE_MSGFILE_FILE_EXTENSION(".bbsmsg");
	static const std::regex MESSAGESTORAGE_MSGFILE_FILE_NAME_REGEX("[0-9]{1,10}\\.bbsmsg");
	static const std::string MESSAGESTORAGE_MSGFILE_DIRECTORY("/home/g83r/Desktop/uga/sat-lab/MEMESat-FSW/obc/ms1/TcpClientDeployment/bbsstorage/"); // CHANGE

	// ----------------------------------------------------------------------
	// Construction, initialization, and destruction
	// ----------------------------------------------------------------------

	MessageStorage ::
    MessageStorage(
		const char* const compName
    ) : MessageStorageComponentBase(compName),
		nextIndexCounter(0),
		lastSuccessfullyStoredIndices()
	{}


	void MessageStorage::init(const NATIVE_INT_TYPE instance)
	{
		MessageStorageComponentBase::init(instance);
		this->restoreIndexFromHighestStoredIndexFoundInDirectory();
	} // init()

	MessageStorage::~MessageStorage() {}

	// ----------------------------------------------------------------------
	// Handler implementations for user-defined typed input ports
	// ----------------------------------------------------------------------

	/**
	 *  Stores a message to the file system and returns whether the operation was successful of 
	 * type BBSModule::BBSMessageStorageStatus.
	 *
	 * @param portNum The port number of the port that received the message.
	 * @param data The message to store.
	 */
	MessageModule::MessageStorageStatus MessageStorage::storeMessage_handler(
			const NATIVE_INT_TYPE portNum,
			const MessageModule::Message &data)
	{
		// Get next index and increment nextIndexCounter		
		const U32 index = this->nextIndex();
		// telem store count is incremented in storeMessage()
		this->tlmWrite_STORE_COUNT(++this->numStoreAttempts);
		// TODO Optional: Handle making space / deleting old messages by the BBS component
		// Or at least scheduele them for deletion by the FileRecycler?
		const bool success = this->storeMessage(index, data); // 
		// receive status from store_message op cast to BBSModule::BBSMessageStorageStatus
		const MessageModule::MessageStorageStatus status = success
														? MessageModule::MessageStorageStatus::OK
														: MessageModule::MessageStorageStatus::ERROR;
		return status;
	} // storeMessage_handler()

	/** 
	 * loadMessageFromIndex_handler loads a message from the file system and returns whether 
	 * the operation was successful of type BBSModule::BBSMessageValid.
	 * 
	 * @param portNum The port number of the port that received the message.
	 * @param index The index of the message to load.
	 * @param data The message to load into.
	 */
	MessageModule::MessageValid MessageStorage::loadMessageFromIndex_handler(
			const NATIVE_INT_TYPE portNum,
			const U32 index,
			MessageModule::Message &data
	) {
		// telem load count is incremented in loadMessage() an returns whether the operation was successful
		const bool success = this->loadMessage(index, data);
		// telem load count is incremented in loadMessage()
		this->tlmWrite_LOAD_COUNT(++this->numLoadAttempts);
		// receive status from load_messages op cast to BBSModule::BBSMessageValid
		const MessageModule::MessageValid status = static_cast<MessageModule::MessageValid::t>(success);
		return status;
	} // loadMessageFromIndex_handler()

	/** 
	 * loadMessageLastN_handler loads the last N messages from the file system and returns the number of
	 * messages loaded.
	 * 
	 * @param portNum The port number of the port that received the message.
	 * @param num_messages The number of messages to load.
	 * @param lastMessages The messages to load into.
	 */
	U8 MessageStorage::loadMessageLastN_handler(
		const NATIVE_INT_TYPE portNum, const U8 num_messages, MessageModule::Message_Batch &lastMessages)
	{
		U8 num_messages_to_load{num_messages}; // Number of messages to load
		if (Message_Batch_Size < num_messages_to_load) // Check whether num_messages_to_load is too large
		{
			num_messages_to_load = Message_Batch_Size; // Set num_messages_to_load to max allowed value
		}
		MessageModule::Message_Array messages_batch{}; // New array to write info to
		// Get iterator of lastSuccessfullyStoredIndices pointing from the back to the first index to load
		auto iterator = this->lastSuccessfullyStoredIndices.crbegin();
		int num_messages_loaded{0}; // Number of messages loaded
		// Iterate over lastSuccessfullyStoredIndices until num_messages_loaded == num_messages_to_load or
		while (num_messages_loaded < num_messages_to_load && iterator != this->lastSuccessfullyStoredIndices.crend())
		{
			const U32 index_to_load = *iterator; // Get index to load from iterator	
			// Get BBSModuleerence to message to load into
			MessageModule::Message &message_to_load_into = messages_batch[num_messages_loaded];
			// Load message into message_to_load_into
			const bool success = this->loadMessage(index_to_load, message_to_load_into);
			// telem load count is incremented in loadMessage()
			this->tlmWrite_LOAD_COUNT(++this->numLoadAttempts);
			if (success)
			{
				++num_messages_loaded; // Increment number of messages loaded
			} // endif
			++iterator; // Move iterator to next index
		} // endWhile
		lastMessages.setmessages(messages_batch); // Write dirty copy back
		lastMessages.setnumValidMessages(num_messages_loaded); // followed by number of valid messages
		return num_messages_loaded;
	} // loadMessageLastN_handler()

	// ----------------------------------------------------------------------
	// Private member functions
	// ----------------------------------------------------------------------

	/** 
	 * storeMessage stores a message to the file system and returns whether the operation was successful.
	 * 
	 * @param index The index of the message to store.
	 * @param data The message to store in the form of a Fw::Serializable.
	 */
	bool MessageStorage::storeMessage(const U32 index, const Fw::Serializable &data)
	{
		StackBuffer stackBuff{};
		Os::File::Status file_op_status; // Status of file operations
		const std::string file_name_absolute = this->indexToAbsoluteFilePath(index); // Absolute file name of file to store to
		this->createStorageDirectoryIfNotExists(); // Create storage directory if it does not exist yet
			try // Intentionally large try block to handle abort due to all kinds of MessageWriteError exceptions equally
			{
				/*
				*	Open file
				*/
				// Check whether file exists: file is not automatically created when opening for read
				Os::File testExistsFile{}; // File to test whether file exists 
				// Open file for read
				file_op_status = testExistsFile.open(file_name_absolute.c_str(), Os::File::OPEN_READ);
				// If file exists
				if (file_op_status != Os::File::DOESNT_EXIST)
				{
					// Abort storing and throw exception
					this->log_WARNING_HI_MESSAGE_STORE_FAILED(index, MessageWriteError::FILE_EXISTS, file_op_status);
					throw MessageWriteError(MessageWriteError::FILE_EXISTS);
				}
				// File is automatically created when opening for write
				Os::File file{};
				// Open file for write
				file_op_status = file.open(file_name_absolute.c_str(), Os::File::OPEN_SYNC_WRITE);
				// Abort storing and throw exception if opening failed
				if (file_op_status != Os::File::OP_OK)
				{
					// log file_op_status 
					this->log_WARNING_HI_MESSAGE_STORE_FAILED(index, MessageWriteError::OPEN, file_op_status);
					throw MessageWriteError(MessageWriteError::OPEN);
				}
				/*
				* Write delimiter
				*/
				U8 delimiter = MESSAGESTORAGE_MSGFILE_DELIMITER;
				NATIVE_INT_TYPE write_size = sizeof(delimiter);
				this->writeRawBufferToFile(&delimiter, file, write_size, index,
										MessageWriteError::DELIMITER_WRITE,
										MessageWriteError::DELIMITER_SIZE);
				/*
				* Write checksum
				*/
				// TODO Optional: Implement checksum
				/*
				*	Write message size = length of message type
				*/
				// Serialize message 1st time just to get its size
				stackBuff.safeSerialize(data);
				const U32 message_size = stackBuff.getBuffLength();
				stackBuff.safeSerialize(message_size);
				write_size = sizeof(message_size);
				this->writeSerializeBufferToFile(stackBuff, file, write_size, index,
												MessageWriteError::MESSAGE_SIZE_WRITE,
												MessageWriteError::MESSAGE_SIZE_SIZE);
				/*
				*	Write message
				*/
				// Serialize message 2nd time to write it to file
				stackBuff.safeSerialize(data);
				write_size = message_size;
				this->writeSerializeBufferToFile(stackBuff, file, write_size, index,
												MessageWriteError::MESSAGE_CONTENT_WRITE,
												MessageWriteError::MESSAGE_CONTENT_SIZE);
				/*
				*	Done
				*/
				this->addIndexToLastSuccessfullyStoredIndices(index);
				this->log_ACTIVITY_LO_MESSAGE_STORE_COMPLETE(index);
				return true;
				// file and stackBuffer are closed / deallocated automatically by their destructors
			}
			catch (const MessageWriteError &e) // This can only be triggered by the explicity throw statements above
			{
				/*
				* Clean Up upon fail
				*/
				// Delete file if file was created but storing failed
				if (e != MessageWriteError::FILE_EXISTS)
				{
					Os::FileSystem::Status delete_status = Os::FileSystem::removeFile(file_name_absolute.c_str());
					if (delete_status != Os::FileSystem::OP_OK)
					{
						this->log_WARNING_HI_MESSAGE_STORE_FAILED(index, MessageWriteError::CLEANUP_DELETE, delete_status);
					};
				}
				return false;
			}
	} // storeMessage()

	bool MessageStorage::loadMessage(const U32 index, Fw::Serializable &data)
	{
		StackBuffer stackBuff{};
		Os::File::Status file_op_status;
		NATIVE_INT_TYPE read_size;
		const std::string file_name_absolute = this->indexToAbsoluteFilePath(index);
		try // Same method as for storeMessage(): Throw + catch MessageReadError if an operation fails
		{
			/* Open file */
			Os::File file{};
			file_op_status = file.open(file_name_absolute.c_str(), Os::File::OPEN_READ);
			if (file_op_status != Os::File::OP_OK)
			{
				this->log_WARNING_LO_MESSAGE_LOAD_FAILED(index, MessageReadError::OPEN, file_op_status);
				throw MessageReadError(MessageReadError::OPEN);
			}
			/* Read delimiter + check whether it is the expected BBSMESSAGESTORAGE_MSGFILE_DELIMITER */
			U8 delimiter;
			this->readRawBufferFromFile(&delimiter, file, sizeof(delimiter), index,
										MessageReadError::DELIMITER_READ,
										MessageReadError::DELIMITER_SIZE);
			if (delimiter != MESSAGESTORAGE_MSGFILE_DELIMITER)
			{
				this->log_WARNING_LO_MESSAGE_LOAD_FAILED(index, MessageReadError::DELIMITER_CONTENT, delimiter);
				throw MessageReadError(MessageReadError::DELIMITER_CONTENT);
			}
			/*
			 *	Read checksum
			 */
			// TODO Optional: Implement checksum
			/* Read message size */ 
			U32 message_size{0};
			read_size = sizeof(message_size);
			this->readRawBufferFromFile(stackBuff.getBuffAddr(), file, read_size, index,
										MessageReadError::MESSAGE_SIZE_READ,
										MessageReadError::MESSAGE_SIZE_SIZE);
			/**
			 * TODO: Needs to be tested
			 * 
			 * Desc.: set_length_failure_callback_size is called if setBuffLength() fails
			 * If reading the deserialized set length fails, the index and message_size are logged
			 * a MessageReadError is thrown to abort deserialization
			 */
			const std::function<void()> set_length_failure_callback_size = [&]()
			{
				this->log_WARNING_LO_MESSAGE_LOAD_FAILED(index, MessageReadError::MESSAGE_SIZE_DESER_SET_LENGTH,
														 message_size);
				throw MessageReadError(MessageReadError::MESSAGE_SIZE_DESER_SET_LENGTH);
			};

			/** 
			 * TODO: Needs to be tested
			 * 
			 * Desc.: deserialize_status_failure_callback_size is called if safeDeserialize() fails
			 * If reading the deserialized status fails, the index, MESSAGE_SIZE_DESER_EXCECUTE and error_code are logged
			 * a MessageReadError is thrown to abort deserialization
			 */
			const std::function<void(const NATIVE_UINT_TYPE)> deserialize_status_failure_callback_size = [&](const NATIVE_UINT_TYPE error_code)
			{
				this->log_WARNING_LO_MESSAGE_LOAD_FAILED(index, MessageReadError::MESSAGE_SIZE_DESER_EXCECUTE,
														 error_code);
				throw MessageReadError(MessageReadError::MESSAGE_SIZE_DESER_EXCECUTE);
			};

			/** 
			 * TODO: Needs to be tested
			 * 
			 * Desc.: deserialize_length_failure_callback_size is called if safeDeserialize() fails
			 * If reading the deserialized length fails, the index, MESSAGE_SIZE_DESER_READ_LENGTH and error_code are logged
			 * a MessageReadError is thrown to abort deserialization
			 */
			const std::function<void(const NATIVE_UINT_TYPE)> deserialize_length_failure_callback_size = [&](const NATIVE_UINT_TYPE error_code)
			{
				this->log_WARNING_LO_MESSAGE_LOAD_FAILED(index, MessageReadError::MESSAGE_CONTENT_DESER_READ_LENGTH,
														 error_code);
				throw MessageReadError(MessageReadError::MESSAGE_CONTENT_DESER_READ_LENGTH);
			};

			// Deserialize message size from stackBuff into message_size
			stackBuff.safeDeserialize(message_size, read_size, set_length_failure_callback_size,
									  deserialize_status_failure_callback_size,
									  deserialize_length_failure_callback_size);

			// Check whether message will fit into stackBuff
			if (message_size > stackBuff.getBuffCapacity())
			{
				// log message size as read_size, because it is the size of the message content
				this->log_WARNING_LO_MESSAGE_LOAD_FAILED(index, MessageReadError::MESSAGE_SIZE_EXCEEDS_BUFFER,
														 message_size);
				// Throw exception to abort deserialization
				throw MessageReadError(MessageReadError::MESSAGE_SIZE_EXCEEDS_BUFFER);
			} // end if

			// Check whether message size is 0. Could not procede if it is, because we would try to read 0 bytes
			// for the message content's serialization
			if (message_size == 0)
			{
				// log message size as read_size, because it is the size of the message content
				this->log_WARNING_LO_MESSAGE_LOAD_FAILED(index, MessageReadError::MESSAGE_SIZE_ZERO, 0);
				// Throw exception to abort deserialization
				throw MessageReadError(MessageReadError::MESSAGE_SIZE_ZERO);
			} // end if 

			/*
			 *	Read message
			 */
			read_size = message_size;
			// Read message content into stackBuff, providing the size of the message content as read_size
			this->readRawBufferFromFile(stackBuff.getBuffAddr(), file, read_size, index,
										MessageReadError::MESSAGE_CONTENT_READ,
										MessageReadError::MESSAGE_CONTENT_SIZE);
			// Deserialize message content from stackBuff

			/** 
			 * TODO: Needs to be tested
			 */
			const std::function<void()> set_length_failure_callback_content = [&]()
			{
				// log message size as read_size, because it is the size of the message content
				this->log_WARNING_LO_MESSAGE_LOAD_FAILED(index, MessageReadError::MESSAGE_CONTENT_DESER_SET_LENGTH, read_size);
				// Throw exception to abort deserialization
				throw MessageReadError(MessageReadError::MESSAGE_CONTENT_DESER_SET_LENGTH);
			}; // Deserialize message content from stackBuff

			// Deserialize message content from stackBuff
			const std::function<void(const NATIVE_UINT_TYPE)> deserialize_status_failure_callback_content = [&](const NATIVE_UINT_TYPE error_code)
			{
				// log message size as read_size, providing index, MESSAGE_CONTENT_DESER_EXCECUTE and error_code
				this->log_WARNING_LO_MESSAGE_LOAD_FAILED(index, MessageReadError::MESSAGE_CONTENT_DESER_EXCECUTE, error_code);
				// Throw exception to abort deserialization
				throw MessageReadError(MessageReadError::MESSAGE_CONTENT_DESER_EXCECUTE);
			}; // Deserialize message content from stackBuff
			// Deserialize message content from stackBuff
			const std::function<void(const NATIVE_UINT_TYPE)> deserialize_length_failure_callback_content = [&](const NATIVE_UINT_TYPE error_code)
			{
				// log message size as read_size, providing index, MESSAGE_CONTENT_DESER_READ_LENGTH and error_code
				this->log_WARNING_LO_MESSAGE_LOAD_FAILED(index, MessageReadError::MESSAGE_CONTENT_DESER_READ_LENGTH, error_code);
				throw MessageReadError(MessageReadError::MESSAGE_CONTENT_DESER_READ_LENGTH);
			};
			// Deserialize message content from stackBuff
			stackBuff.safeDeserialize(data, read_size, set_length_failure_callback_content,
									  deserialize_status_failure_callback_content,
									  deserialize_length_failure_callback_content);

			/*
			 *	Done
			 */

			// Check whether file has been read to the end by trying to read one more byte and expecting it to read none
			read_size = 1;
			FwSignedSizeType read_size_signed = static_cast<FwSignedSizeType>(read_size); 
			const Os::File::Status file_status = file.read(stackBuff.getBuffAddr(), read_size_signed, Os::FileInterface::WaitType::WAIT);

			// If file_status is not OP_OK or read_size is not 0, the file has not been read to the end
			if (file_status != Os::File::OP_OK || read_size != 0)
			{
				// log file_status
				this->log_WARNING_LO_MESSAGE_LOAD_FAILED(index, MessageReadError::FILE_END, file_status);
				// Throw exception to abort deserialization
				throw MessageReadError(MessageReadError::FILE_END);
			} // end if
			// If we reach this point, the file has been read to the end
			this->log_ACTIVITY_LO_MESSAGE_LOAD_COMPLETE(index);
			return true;
		} // end try
		catch (const MessageReadError &e)
		{
			// Put common handling of file open errors here if needed
			// Try-catch is kept, even though it is empty for now, for consistency with storeMessage()

			return false;
		}
	} // loadMessage()

	bool MessageStorage::restoreIndexFromHighestStoredIndexFoundInDirectory()
	{
		this->createStorageDirectoryIfNotExists();
		Os::Directory storage_dir;
		Os::Directory::Status dir_status;
		dir_status = storage_dir.open(MESSAGESTORAGE_MSGFILE_DIRECTORY.c_str(), Os::Directory::OpenMode::READ); 
		if (dir_status != Os::Directory::OP_OK)
		{
			/** TODO: needs to be tested */
			this->log_WARNING_HI_INDEX_RESTORE_FAILED(IndexRestoreError::STORAGE_DIR_OPEN, dir_status);
			return false;
		}
		const U32 &readSize = MESSAGESTORAGE_MSGFILE_NAME_MAXLENGTH;
		char file_name[readSize + 1]; // +1 to have space for null terminator
		file_name[readSize] = '\0';	  // Set null terminator to make sure that string is always terminated even after read
		std::vector<std::string> existing_file_names{};
		do
		{
			dir_status = storage_dir.read(file_name, readSize);
			if (dir_status == Os::Directory::OP_OK)
			{
				existing_file_names.push_back(std::string{file_name});
			}
		} while (dir_status == Os::Directory::OP_OK);
		// Trigger warning event if directory read finished with error instead of reaching the end of the directory
		if (dir_status != Os::Directory::NO_MORE_FILES)
		{
			/** TODO: Needs to be tested */
			this->log_WARNING_HI_INDEX_RESTORE_FAILED(IndexRestoreError::STORAGE_DIR_READ, dir_status);
			return false; // Fail early: We could continue here but chose to fail
		}
		// Iterate over existing_file_names and remove ".bbsmsg" suffix from each string
		std::vector<U32> existing_file_indices{};
		for (std::string &file_name : existing_file_names)
		{
			// Check for correct format with REGEX
			if (std::regex_match(file_name, MESSAGESTORAGE_MSGFILE_FILE_NAME_REGEX) == false)
			{
				// Not parsable. Try next file_name
				continue;
			}
			// Remove suffix
			file_name.erase(file_name.length() - MESSAGESTORAGE_MSGFILE_FILE_EXTENSION.length(),
							MESSAGESTORAGE_MSGFILE_FILE_EXTENSION.length());

			// Parse remaining string to U32
			U32 parsed_index{0};
			try
			{
				parsed_index = std::stoul(file_name);
			}
			catch (const std::exception &e) // Catch all exceptions to prevent crash
			{
				// Not parsable. Try next file_name
				continue;
			}
			existing_file_indices.push_back(static_cast<U32>(parsed_index));
		} // end for
		// Restore lastSuccessfullyStoredIndices from the parsed indices
		std::sort(existing_file_indices.begin(), existing_file_indices.end());
		this->lastSuccessfullyStoredIndices.clear();
		const std::size_t restored_history_size = std::min(
			static_cast<size_t>(MESSAGESTORAGE_STORED_INDEX_HISTORY_SIZE), existing_file_indices.size());
		this->lastSuccessfullyStoredIndices.resize(restored_history_size);
		std::copy(existing_file_indices.cend() - restored_history_size,
				  existing_file_indices.cend(),
				  this->lastSuccessfullyStoredIndices.begin());
		// Restore nextIndexCounter from last index in existing_file_indices
		if (existing_file_indices.empty())
		{
			// No files found
			this->nextIndexCounter = MESSAGESTORAGE_INITIAL_INDEX;
			this->log_ACTIVITY_LO_INDEX_RESTORE_COMPLETE(0, 0);
		}
		else
		{
			this->nextIndexCounter = existing_file_indices.back() + 1;
			this->log_ACTIVITY_LO_INDEX_RESTORE_COMPLETE(existing_file_indices.size(), this->nextIndexCounter - 1);
		}
		this->tlmWrite_NEXT_STORAGE_INDEX(this->nextIndexCounter);
		return true;
	} // restoreIndexFromHighestStoredIndexFoundInDirectory()

	U32 MessageStorage::nextIndex()
	{
		const U32 &index = this->nextIndexCounter++;
		// Handle index wrap around
		if (this->nextIndexCounter == 0)
		{
			// TODO Optional: Remove old BBS messages at indices 0,1,2,... if they still exist 
			// -> e.g. schedule FileRecycler?
			this->log_WARNING_LO_INDEX_WRAP_AROUND();
		}
		this->tlmWrite_NEXT_STORAGE_INDEX(index + 1);
		return index;
	} // nextIndex()

	void MessageStorage::addIndexToLastSuccessfullyStoredIndices(const U32 index)
	{
		if (this->lastSuccessfullyStoredIndices.size() >= MESSAGESTORAGE_STORED_INDEX_HISTORY_SIZE)
		{
			this->lastSuccessfullyStoredIndices.pop_front();
			// TODO Optional: We could tell the file recycler at this point that the file is no longer needed
		}
		this->lastSuccessfullyStoredIndices.push_back(index);
	} // addIndexToLastSuccessfullyStoredIndices()

	std::string MessageStorage::indexToAbsoluteFilePath(const U32 index)
	{
		return MESSAGESTORAGE_MSGFILE_DIRECTORY +
			   std::to_string(index) +
			   MESSAGESTORAGE_MSGFILE_FILE_EXTENSION;
	} // indexToAbsoluteFilePath()

	void MessageStorage::writeSerializeBufferToFile(
		Fw::SerializeBufferBase &serializeBuffer, Os::File &file,
		const NATIVE_INT_TYPE expected_write_size, const U32 &index,
		const MessageWriteError write_error_stage,
		const MessageWriteError size_error_stage)
	{
		NATIVE_INT_TYPE actual_write_size = serializeBuffer.getBuffLength();
		FW_ASSERT(actual_write_size == expected_write_size, actual_write_size);
		this->writeRawBufferToFile(serializeBuffer.getBuffAddr(), file, actual_write_size,
								   index, write_error_stage, size_error_stage);
	} // writeSerializeBufferToFile()

	void MessageStorage::writeRawBufferToFile(
		const void *const buffer_address, Os::File &file,
		const NATIVE_INT_TYPE expected_write_size, const U32 &index,
		const MessageWriteError write_error_stage,
		const MessageWriteError size_error_stage)
	{
		NATIVE_INT_TYPE write_size{expected_write_size}; // Might be overwritten by file.write()
		const U8* buffer_u8 = static_cast<const U8*>(buffer_address);
		FwSignedSizeType write_size_signed = static_cast<FwSignedSizeType>(write_size);
		const Os::File::Status file_op_status = file.write(buffer_u8, write_size_signed, Os::FileInterface::WaitType::WAIT); 
		if (file_op_status != Os::File::OP_OK)
		{
			/** TODO: Needs to be tested */
			this->log_WARNING_HI_MESSAGE_STORE_FAILED(index, write_error_stage, file_op_status);
			throw write_error_stage;
		}
		// file.write() overwrites write_size with the number of bytes actually written
		if (write_size != expected_write_size)
		{
			/** TODO: Needs to be tested */
			this->log_WARNING_HI_MESSAGE_STORE_FAILED(index, size_error_stage, write_size);
			throw size_error_stage;
		}
	} // writeRawBufferToFile()

	/** 
	 * readRawBufferFromFile reads a raw buffer from a file and throws a MessageReadError if the operation fails.
	 *  
	 * @param buffer_address The address of the buffer to read into.
	 * @param file The file to read from.
	 * @param expected_read_size The size of the buffer to read.
	 * @param index The index of the message to read.
	 * @param read_error_stage The stage at which the error occured.
	 * @param size_error_stage The stage at which the error occured.
	 */
	void MessageStorage::readRawBufferFromFile(
		void *const buffer_address, Os::File &file,
        const NATIVE_INT_TYPE expected_read_size, 
		const U32 &index,
		const MessageReadError read_error_stage,
	    const MessageReadError size_error_stage
    ) {
		NATIVE_INT_TYPE read_size{expected_read_size}; // Will be overwritten by file.read()
		U8* buffer_u8 = static_cast<U8*>(buffer_address);
		FwSignedSizeType read_size_signed = static_cast<FwSignedSizeType>(read_size);
		const Os::File::Status file_op_status = file.read(buffer_u8, read_size_signed, Os::FileInterface::WaitType::WAIT);
		if (file_op_status != Os::File::OP_OK)
		{
			/** TODO: Needs to be btested. */
			// log file_op_status
			this->log_WARNING_LO_MESSAGE_LOAD_FAILED(index, read_error_stage, file_op_status);
			throw read_error_stage;
		}
		// file.read() overwrites read_size with the number of bytes actually read
		if (read_size != expected_read_size)
		{
			// log read_size
			this->log_WARNING_LO_MESSAGE_LOAD_FAILED(index, size_error_stage, read_size);
			throw size_error_stage;
		}
	} // readRawBufferFromFile()


	/** 
	 * createStorageDirectoryIfNotExists creates the storage directory if it does not exist yet.
	 * 
	 * check if directory exists; if not, create it and log creation status.
	 */
	void MessageStorage::createStorageDirectoryIfNotExists()
	{
		// Check whether directory exists
		Os::FileSystem::Status dir_create_status = Os::FileSystem::createDirectory(
			MESSAGESTORAGE_MSGFILE_DIRECTORY.c_str());
		// if status is ALREADY_EXISTS, directory already exists and we are done
		if (dir_create_status != Os::FileSystem::ALREADY_EXISTS)
		{
			const bool creation_successful{dir_create_status == Os::FileSystem::OP_OK};
			// Log warning if directory could not be created
			this->log_WARNING_LO_STORAGE_DIRECTORY_WARNING(Fw::String(MESSAGESTORAGE_MSGFILE_DIRECTORY.c_str()), 
														   creation_successful);
		}
	} // createStorageDirectoryIfNotExists()

} // end namespace BBSModule