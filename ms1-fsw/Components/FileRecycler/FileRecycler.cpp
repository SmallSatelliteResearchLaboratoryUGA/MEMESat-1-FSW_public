// ======================================================================
// \title  FileRecycler.cpp
// \author g83r
// \brief  cpp file for FileRecycler component implementation class
// ======================================================================

#include "Components/FileRecycler/FileRecycler.hpp"
#include "FpConfig.hpp"
#include <cstdio>
#include <dirent.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <locale.h>
#include <unistd.h>
#include <Fw/Types/String.hpp>
#include <queue>
#include <set>
#include <inttypes.h>
#include <fcntl.h>
#include <time.h>
#include <iostream>
#include <fstream>
#include <utility>
#include <cstring>
#include <vector>
#include <algorithm>
#include <exception>
#include <FpConfig.hpp>
#include "Fw/Types/BasicTypes.hpp"

#if __has_include(<filesystem>) 
#include <filesystem>
#else
#include <experimental/filesystem>
#endif

namespace FileRecyclerModule {

  // ----------------------------------------------------------------------
  // Component construction and destruction
  // ----------------------------------------------------------------------

  FileRecycler ::
    FileRecycler(const char* const compName) :
      FileRecyclerComponentBase(compName)
  {
    this->shutdown = false;
  } // FileRecycler 

  void FileRecycler :: 
    init( 
      const NATIVE_INT_TYPE queueDepth,
      const NATIVE_INT_TYPE instance
    )
  {
    FileRecyclerComponentBase::init(queueDepth, instance);
  } // init

  FileRecycler ::
    ~FileRecycler()
  {

  }

  void FileRecycler :: 
    preamble()
  {
    Fw::ParamValid valid; 
    // Gets the SCHEDULE parameter 
    const Fw::ParamString schedule_param = this->paramGet_FR_Schedule(valid);
    const char* schedule = schedule_param.toChar();
    FW_ASSERT(
      valid.e == Fw::ParamValid::VALID || valid.e == Fw::ParamValid::DEFAULT,
      valid.e
    );
    this->sendSchedule_out(0, Fw::String("FileRecycler"), Fw::String(schedule), SchedulerModule::ScheduleOp::START);
  } // preamble

  void FileRecycler :: 
    parameterUpdated(FwPrmIdType id)
  {
    if (id == 1) 
    { 
      Fw::ParamValid valid; 
      // Gets the SCHEDULE parameter 
      const Fw::ParamString schedule_param = this->paramGet_FR_Schedule(valid);
      const char* schedule = schedule_param.toChar();
      FW_ASSERT(
        valid.e == Fw::ParamValid::VALID || valid.e == Fw::ParamValid::DEFAULT,
        valid.e
      );
      this->sendSchedule_out(0, Fw::String("FileRecycler"), Fw::String(schedule), SchedulerModule::ScheduleOp::START);
      this->log_DIAGNOSTIC_FR_ScheduleChangedTo(Fw::String(schedule)); 
    } // if
  } // parameterUpdated

  void FileRecycler ::
    recycler_call_on_reset_handler()
  {
    this->log_WARNING_LO_FR_RecyclerStoppedBySysReset();
  } // recycler_call_on_reset_handler

  // -- 
  // Recycler 
  // -- 

  template <typename T>
  FileRecycler::FauxOsStatus FileRecycler :: 
    convertOsStatusToFaux(const T& status)
  {
    static_assert(std::is_same<T, Os::Directory::Status>::value || std::is_same<T, Os::FileSystem::Status>::value, 
      "convertOsStatusToFaux only accepts Os::Directory::Status or Os::FileSystem::Status as template parameter");
    FauxOsStatus fauxStatus;
    auto modifyFauxStatus = [&](std::string status, std::string message) 
    { 
      fauxStatus.setstatus(status.c_str()); 
      fauxStatus.setstatus_message(message.c_str());
    }; // modifyFauxStatus
    if constexpr(std::is_same<T, Os::Directory::Status>::value) 
    {
      switch(status) 
      { 
        // Operation was successful 
        case Os::Directory::Status::OP_OK: modifyFauxStatus("Os::Directory:Status::OP_OK", "Operation was successful"); break;
        // Directory doesn't exist 
        case Os::Directory::Status::DOESNT_EXIST: modifyFauxStatus("Os::Directory:Status::DOESNT_EXIST", "Directory doesn't exist"); break;
        // No permission to read directory 
        case Os::Directory::Status::NO_PERMISSION: modifyFauxStatus("Os::Directory:Status::NO_PERMISSION", "No permission to read directory"); break;
        // Directory hasn't been opened yet 
        case Os::Directory::Status::NOT_OPENED: modifyFauxStatus("Os::Directory:Status::NOT_OPENED", "Directory hasn't been opened yet"); break;
        // Path is not a directory 
        case Os::Directory::Status::NOT_DIR: modifyFauxStatus("Os::Directory:Status::NOT_DIR", "Path is not a directory"); break;
        // Directory stream has no more files 
        case Os::Directory::Status::NO_MORE_FILES: modifyFauxStatus("Os::Directory:Status::NO_MORE_FILES", "Directory stream has no more files"); break;
        // A catch-all for other errors. have to look at implementation-specific code 
        default: modifyFauxStatus("Os::Directory:Status::OTHER_ERROR", "A catch-all for other errors. Have to look in implementation-specific code"); break;
      } // switch
    } // if constexpr
    else if (std::is_same<T, Os::FileSystem::Status>::value)
    { 
      switch(status)
      {
        //!< Operation was successful
        case Os::FileSystem::Status::OP_OK: modifyFauxStatus("Os::FileSystem::Status::OP_OK", "Operation was successful"); break; 
        //!< File already exists 
        case Os::FileSystem::Status::ALREADY_EXISTS: modifyFauxStatus("Os::FileSystem::Status::ALREADY_EXISTS", "File already exists"); break;
        //!< No space left 
        case Os::FileSystem::Status::NO_SPACE: modifyFauxStatus("Os::FileSystem::Status::NO_SPACE", "No space left"); break;
        //!< No permission to write 
        case Os::FileSystem::Status::NO_PERMISSION: modifyFauxStatus("Os::FileSystem::Status::NO_PERMISSION", "No permission to write"); break;
        //!< Path is not a directory
        case Os::FileSystem::Status::NOT_DIR: modifyFauxStatus("Os::FileSystem::Status::NOT_DIR", "Path is not a directory"); break;
        //!< Path is a directory
        case Os::FileSystem::Status::IS_DIR: modifyFauxStatus("Os::FileSystem::Status::IS_DIR", "Path is a directory"); break;
        //!< directory is not empty
        case Os::FileSystem::Status::NOT_EMPTY: modifyFauxStatus("Os::FileSystem::Status::NOT_EMPTY", "Directory is not empty"); break;
        //!< Path is too long, too many sym links, doesn't exist, ect
        case Os::FileSystem::Status::INVALID_PATH: modifyFauxStatus("Os::FileSystem::Status::INVALID_PATH", "Path is too long, too many sym links, doesn't exist, ect"); break;
        //!< Too many files or links
        case Os::FileSystem::Status::FILE_LIMIT: modifyFauxStatus("Os::FileSystem::Status::FILE_LIMIT", "Too many files or links"); break;
        //!< Operand is in use by the system or by a process
        case Os::FileSystem::Status::BUSY: modifyFauxStatus("Os::FileSystem::Status::BUSY", "Operand is in use by the system or by a process"); break;
        //!< Other OS-specific error
        default: modifyFauxStatus("Os::FileSystem::Status::OTHER_ERROR", "Other OS-specific error"); break;
      } // switch
    } // else if
    return fauxStatus;
  } // convertOsStatusToFaux

  void FileRecycler :: 
    handleStatError(
      int valueFromStat
    )
  {
    switch(valueFromStat)
    {
      case EACCES:
        this->log_WARNING_LO_FR_StatError(valueFromStat, Fw::String("EACCES"), Fw::String("Permission denied."));
        // Solve with path resolution 
        // https://linux.die.net/man/7/path_resolution
        break;
      case EBADF: 
        this->log_WARNING_LO_FR_StatError(valueFromStat, Fw::String("EBADF"), Fw::String("Invalid file descriptor."));
        break;
      case EFAULT: 
        this->log_WARNING_LO_FR_StatError(valueFromStat, Fw::String("EFAULT"), Fw::String("Bad address."));
        break;
      case ELOOP: 
        this->log_WARNING_LO_FR_StatError(valueFromStat, Fw::String("ELOOP"), Fw::String("Too many symbolic links encountered."));
        break;
      case ENAMETOOLONG:
        this->log_WARNING_LO_FR_StatError(valueFromStat, Fw::String("ENAMETOOLONG"), Fw::String("Path is too long."));
        break;
      case ENOENT:
        this->log_WARNING_LO_FR_StatError(valueFromStat, Fw::String("ENOENT"), Fw::String("No such file or directory."));
        break;
      case ENOMEM:
        this->log_WARNING_LO_FR_StatError(valueFromStat, Fw::String("ENOMEM"), Fw::String("Out of memory."));
        break;
      case ENOTDIR:
        this->log_WARNING_LO_FR_StatError(valueFromStat, Fw::String("ENOTDIR"), Fw::String("Not a directory."));
        break;
      case EOVERFLOW: 
        this->log_WARNING_LO_FR_StatError(valueFromStat, Fw::String("EOVERFLOW"), Fw::String("Value too large for defined data type."));
        break;
      default: 
        this->log_WARNING_LO_FR_StatError(valueFromStat, Fw::String("UNKNOWN"), Fw::String("Default case hit in stat error switch."));
        break;
    } // switch
  } // handleStatError

  std::vector<std::tuple<time_t, std::string, FileRecycler::FauxOsStatus>> 
    FileRecycler::recycler(
      U32 count,
      FileRecyclerModule::FileRecyclerOp& op
    )
  {
    std::vector<std::tuple<time_t, std::string, FileRecycler::FauxOsStatus>> deleted_files;
    // if shutting down, send event and don't continue
    if (this->shutdown)
    {
      this->log_WARNING_LO_FR_RecyclerStoppedBySysReset();
      return deleted_files;
    } // if
    // Open directory 
    std::lock_guard<std::mutex> lock(this->directory_mutex);
    Os::Directory::Status status = this->directory.open(MEMESAT_STORAGE_DIR, Os::Directory::READ);
    if (status != Os::Directory::Status::OP_OK)
    {
      // invoke change state port to go to reboot state 
      // error opening directory 
      // should send status to telemetry 
      // Ask state machine to move into abnormal mode? 
      // Add this as a check prior to moving into cruise 
      this->log_WARNING_HI_FR_CannotOpenDir(Fw::String(MEMESAT_STORAGE_DIR));
      return deleted_files;
    } // if
    struct stat file_info;
    // Name of the file 
    char file[MEMESAT_STORAGE_DIR_MAX_LEN]; // maybe make a string or catch the overflow  
    status = this->directory.read(file, MEMESAT_STORAGE_DIR_MAX_LEN);
    std::set<std::pair<time_t, std::string>> files;
    while((status != Os::Directory::Status::NO_MORE_FILES) && 
            (status != Os::Directory::Status::OTHER_ERROR))
    { 
      // absolute path of the file 
      char abspath[MEMESAT_STORAGE_DIR_MAX_LEN];
      strcpy(abspath, MEMESAT_STORAGE_DIR);
      strcat(abspath, file);
      int exist = stat(abspath, &file_info);
      if (exist == 0) 
      { 
        switch(op.e)
        {
          case FileRecyclerOp::BEFORETIME:
            if (file_info.st_mtime < count)
            {
              Os::FileSystem::Status fsStatus = Os::FileSystem::removeFile(abspath);
              deleted_files.push_back(std::make_tuple(file_info.st_mtime, abspath, convertOsStatusToFaux(fsStatus)));
            }
            break;
          case FileRecyclerOp::AFTERTIME:
            if (file_info.st_mtime > count)
            {
              Os::FileSystem::Status fsStatus = Os::FileSystem::removeFile(abspath);
              deleted_files.push_back(std::make_tuple(file_info.st_mtime, abspath, convertOsStatusToFaux(fsStatus)));
            }
            break;
          default: 
            //if (file_info.st_atime < file_info.st_mtime)
              // file was accessed before it was modified, use 
              // st_atime for age
            //  files.emplace(file_info.st_mtime, abspath);
            //else
              // file was modified before it was accessed, use
              // st_mtime for age 
            //  file.emplace(file_info.st_mtime, abspath);
            break;
        } // switch
        status = this->directory.read(file, 100); 
      } // if
      // if value returned from stat != 0
      // stat error
      else
        handleStatError(exist);
    } // while
    this->directory.close();
    if (status != Os::Directory::Status::NO_MORE_FILES)
    {
      //this->log_WARNING_LO_FR_UnexpectedDirStatus((status));
    } // if
    if (op == FileRecyclerOp::BEFORETIME || op == FileRecyclerOp::AFTERTIME)
      return deleted_files;
    // Now that directory is closed, begin deleting files
    Os::FileSystem::Status delete_status;
    // if NUM call, then remove 'count' number of files
    // change to a map iterator might allow us to consolidate these if statements 
    if (op == FileRecyclerOp::NUM)
    {
      U32 deleted_count = 0;
      // this loop can just use a map iterator 
      for (const auto& file : files)
      {
        if (deleted_count == count)
          break;
        // Need to handle error
        delete_status = Os::FileSystem::removeFile(file.second.c_str());
        deleted_files.push_back(std::make_tuple(file.first, file.second, convertOsStatusToFaux(delete_status)));
        // Need to handle error
        if (delete_status != Os::FileSystem::Status::OP_OK)
        {
          // To do
          // restart
        } // if
        else
          deleted_count += 1;
      } // for
    } // if
    // else if MAX call then remove all files until only 'count' remain 
    else if (op == FileRecyclerOp::MAX && files.size() > count)
    {
      std::set<std::pair<time_t, std::string>>::iterator it;
      auto stop_it = std::prev(files.end(), count);
      // files.end() - count
      for (it = files.begin(); it != stop_it; it++)
      {
        std::pair<time_t, std::string> file = *it;
        //std::cout << "DELETING FILE " << file.second.c_str() << std::endl;
        // Need to handle error
        delete_status = Os::FileSystem::removeFile(file.second.c_str());
        deleted_files.push_back(std::make_tuple(file.first, file.second, convertOsStatusToFaux(delete_status)));
      } // for
    } // else if
    return deleted_files;
  } // recycler


  // ----------------------------------------------------------------------
  // Handler implementations for user-defined typed input ports
  // ----------------------------------------------------------------------

  void FileRecycler:: 
    cleanupDataPathRecv_handler(
      FwIndexType portNum,
      const FileRecyclerModule::CleanUpData& path
    )
  {
    // todo 
  } // cleanupDataPathRecv_handler


  void FileRecycler ::
    fileRecyclerOpIn_handler(
        FwIndexType portNum,
        U32 count,
        const FileRecyclerModule::FileRecyclerOp& op
    )
  {
    // Log the command received and number of files to be removed 
    this->log_COMMAND_FR_CommandRecv(count, op);
    // Remove the number of files specified by the command 
    // 
    // 
    // 
    // 
  } // fileRecyclerOpIn_handler

  void FileRecycler ::
    readyForPowerOff_handler(
        FwIndexType portNum,
        const StateMachineModule::SM_ReadyForPowerOff& result
    )
  {
    // The file manager will make sure all file descriptors are closed and reject any attempts to open one 
    // mutex lock on opening files // changing files // etc 
    // make sure mutex is unlocked on reboot / restart
    std::lock_guard<std::mutex> lock(this->directory_mutex); 
    this->directory.close();
    this->shutdown = true;
    //result = true; // what does this do?
  } // readyForPowerOff_handler

  void FileRecycler ::
    scheduledHandler_handler(
        FwIndexType portNum,
        const SchedulerModule::ScheduleStatus& status
    )
  {
    // should this have a higher priority? 
    if (status == SchedulerModule::ScheduleStatus::FAILED || status == SchedulerModule::ScheduleStatus::STOPPED)
    {
      this->log_WARNING_LO_FR_ScheduleError(status);
      return;
    } // if
    Fw::ParamValid valid;
    U32 max_files = this->paramGet_FR_MaxFiles(valid);
    FW_ASSERT(
      valid.e == Fw::ParamValid::VALID || valid.e == Fw::ParamValid::DEFAULT,
      valid.e
    );
    this->toFileRecyclerHandler_out(0, max_files, FileRecyclerOp::MAX);
    this->log_DIAGNOSTIC_FR_ScheduleRan();
  } // scheduledHandler_handler

  // ----------------------------------------------------------------------
  // Handler implementations for commands
  // ----------------------------------------------------------------------

  void FileRecycler ::
    FR_CleanUpData_cmdHandler(
        FwOpcodeType opCode,
        U32 cmdSeq,
        U32 count,
        FileRecyclerModule::FileRecyclerOp op
    )
  {
    this->log_COMMAND_FR_CommandRecv(count, op);
    this->tlmWrite_FR_Op(op);
    std::vector<std::tuple<time_t, std::string, FileRecycler::FauxOsStatus>> ret_value = this->recycler(count, op);
    //this->logRecyclerOutput(ret_value); 
    // U32 num_op_ok = std::count_if(ret_value.begin(), ret_value.end(), [](const std::tuple<time_t, std::string, FileRecycler::FauxOsStatus>& tuple) {
    //  try { 
    //    return std::get<2>(tuple).getstatus() == "Os::FileSystem::Status::OP_OK";
    //  } catch (std::exception& e) {
    //    return false;
    //  }
    // });
    // 
    //! THE BELOW LINE IS NOT A REPLACEMENT, CONSIDER CHANGING IT 
    this->tlmWrite_FR_NumFilesRemoved(ret_value.size());
    this->tlmWrite_FR_CleanUpDataCompleted(Status::COMPLETED); 
    this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
  } // FR_CleanUpData_cmdHandler
} // namespace FileRecyclerModule
