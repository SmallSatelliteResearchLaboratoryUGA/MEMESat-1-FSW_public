module MessageModule {

  @ Enum for indicating whether a BBS message was stored successfully or not
  enum MessageStorageStatus {
    ERROR = 0 @< An error occurred while storing the message
    OK = 1 @< A message was successfully stored
  }

  @ Enum for indicating whether a BBS message is valid or not
  enum MessageValid {
    INVALID = 0
    VALID = 1
  }

  @ Port for providing a BBS message which is to be stored
  port MessageSet(
               data: Message @< the BBS message to store
            ) -> MessageStorageStatus @< Indicates whether the message was stored successfully or not

  @ Port for loading a BBS message with a given index from the storage and returning it to the caller 
  port MessageGetFromIndex(
              index: U32 @< the index of the message to get
              ref data: Message @< already allocated BBSMessage object of which will be overwritten with the loaded  
                                   @< message in case loading is successful. 
                                   @<
                                   @< Whatever was stored in the object before calling the port is ignored.
                                   @<
                                   @< The fixed BBSMessage size must be large enough to hold the loaded message. 
                                   @< If the loaded message exceeds the maximum size a BBSMessage can hold,
                                   @< loading will fail and trigger an error event.
            ) -> MessageValid @< Indicates whether the loading was successful or not

  @ Port for loading the last N BBS messages which have been stored by the component.
  @ 
  @ Messages are returned to the caller in a BBSMessage_Batch. The number of messages (N) to load is passed as 
  @ a parameter to the port.
  port MessageGetLastN(
    numberOfMessages: U8     @< The number of messages to load. 
                             @<
                             @< Not necessarily equal to the number of messages actually loaded. The numbe of messages
                             @< actually is returned from this port and available in the numMessages field of the
                             @< returned lastMessages.
                             @<
                             @< The number of actually loaded messages is always smaller equals numberOfMessages
                             @< There are three reasons why less messages than numberOfMessages might be loaded:
                             @< 1. Less than numberOfMessages messages are stored
                             @< 2. The component was not able to successfully load numberOfMessages messages 
                             @< 3. numberOfMessages was larger than maxCountGetLastN which is the maximum capacity of 
                             @<    the referenced lastMessagesArray for returning the messages
                             @< 0 iff no messages were loaded successfully. 
    ref lastMessages: Message_Batch
  ) -> U8 @< the number of messages loaded successfully (This is only additional information for convenience. 
          @<  It is already contained in the numMessages field of the returned lastMessages).
}