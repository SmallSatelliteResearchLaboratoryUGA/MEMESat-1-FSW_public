// ======================================================================
// \title  OTAUpdater.cpp
// \author g83r
// \brief  cpp file for OTAUpdater component implementation class
// ======================================================================

#include "Components/OTAUpdater/OTAUpdater.hpp"
#include "FpConfig.hpp"

namespace UpdaterModule {



// ----------------------------------------------------------------------
// Component construction and destruction
// ----------------------------------------------------------------------

OTAUpdater ::OTAUpdater(const char *const compName)
    : OTAUpdaterComponentBase(compName) {}

OTAUpdater ::~OTAUpdater() {}

void OTAUpdater::init(const NATIVE_INT_TYPE queueDepth, const NATIVE_INT_TYPE instance) {
  OTAUpdaterComponentBase::init(queueDepth, instance);
  /* check if 'updater_dir' exists */
  dir_status = this->update_dir.open(UPDATE_DIR_PATH, Os::Directory::OpenMode::CREATE_IF_MISSING);
  if (dir_status != Os::Directory::Status::OP_OK) { 
    /* */
  } 
  /* else the directory was opened - log */
  this->log_WARNING_LO_OTA_AuditStatus(Fw::String("Update Directory exists")); 
  /* try to open the tracking.txt file */
  /* create target path */
  tracking_open_status = this->tracking_file.open(TRACKING_FILE_PATH, Os::File::Mode::OPEN_READ); 
  if (tracking_open_status != Os::File::Status::OP_OK) {
    /* create tracking.txt file */
    touch_status = Os::FileSystem::touch(TRACKING_FILE_PATH);
    if (touch_status != Os::FileSystem::Status::OP_OK) {
      /* */
    }
    /* else the file was created - log */
    this->log_WARNING_LO_OTA_AuditStatus(Fw::String("Tracking file created"));
    /* populate vector with relevant information : .. */ 
    /* TODO */
  } else { 
    /* read tracking file contents into buffer */
    FwSignedSizeType file_size = 0;
    tracking_size_status = this->tracking_file.size(file_size);
    if (tracking_size_status != Os::File::Status::OP_OK) {
      /* */
    }
    /* else the file size was read - log */
    this->log_WARNING_LO_OTA_AuditStatus(Fw::String("Tracking file size read"));
    /* read the file contents */
    U8 buffer[file_size];
    FwSignedSizeType read_size = file_size;
    tracking_read_status = this->tracking_file.read(buffer, read_size);
    if (tracking_read_status != Os::File::Status::OP_OK) {
      /* */
    }
    /* else the file contents were read - log */
    this->log_WARNING_LO_OTA_AuditStatus(Fw::String("Tracking file contents read"));
    /* save values into vector */
    /* TODO */
  }
  /* else the file was opened - log */
  this->log_WARNING_LO_OTA_AuditStatus(Fw::String("Tracking file exists"));
  /* */
  this->log_WARNING_LO_OTA_AuditStatus(Fw::String("OTAUpdater initialized"));
}

// ----------------------------------------------------------------------
// Handler implementations for user-defined typed input ports
// ----------------------------------------------------------------------

void OTAUpdater::preamble() {
  Fw::ParamValid valid; 
  // Gets the SCHEDULE parameter
  const Fw::ParamString schedule_param = this->paramGet_OTA_Schedule(valid);
  const char* schedule = schedule_param.toChar();
  FW_ASSERT(
    valid.e == Fw::ParamValid::VALID || valid.e == Fw::ParamValid::DEFAULT,
    valid.e
  );
  this->sendSchedule_out(
    0, 
    Fw::String("OTAUpdater"), 
    Fw::String(schedule), 
    SchedulerModule::ScheduleOp::START);
} // preamble

void OTAUpdater::parameterUpdated(FwPrmIdType id) {
  if (id == 1) { 
    Fw::ParamValid valid; 
    // Gets the SCHEDULE parameter 
    const Fw::ParamString schedule_param = this->paramGet_OTA_Schedule(valid);
    const char* schedule = schedule_param.toChar();
    FW_ASSERT(
      valid.e == Fw::ParamValid::VALID || valid.e == Fw::ParamValid::DEFAULT,
      valid.e
    );
    this->sendSchedule_out(
      0, 
      Fw::String("OTAUpdater"), 
      Fw::String(schedule), 
      SchedulerModule::ScheduleOp::START);
    this->log_DIAGNOSTIC_OTA_ScheduleChangedTo(Fw::String(schedule));
  } // if
} // parameterUpdated

void OTAUpdater ::readyForPowerOff_handler(
    FwIndexType portNum,
    const StateMachineModule::SM_ReadyForPowerOff &result) {
  // TODO
}

void OTAUpdater ::scheduledHandler_handler(
    FwIndexType portNum, const SchedulerModule::ScheduleStatus &status) {
  // TODO
}

void OTAUpdater ::updaterOpIn_handler(FwIndexType portNum,
                                      const UpdaterModule::UpdateOp &op) {
  // TODO
}

// ----------------------------------------------------------------------
// Handler implementations for commands
// ----------------------------------------------------------------------

void OTAUpdater ::OTA_PerformAudit_cmdHandler(
    FwOpcodeType opCode, U32 cmdSeq, const Fw::CmdStringArg &dir_path) {
  // TODO
  this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
}

void OTAUpdater ::OTA_PerformFileOperation_cmdHandler(
    FwOpcodeType opCode, U32 cmdSeq, U32 operation,
    const Fw::CmdStringArg &file_path) {
  // TODO
  this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
}

void OTAUpdater ::OTA_SelectBootPin_cmdHandler(FwOpcodeType opCode, U32 cmdSeq,
                                               U32 bootOutNum) {
  // TODO
  this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
}

void OTAUpdater ::OTA_SelectRstState_cmdHandler(FwOpcodeType opCode, U32 cmdSeq,
                                                U32 rstState) {
  // TODO
  this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
}

void OTAUpdater::perform_audit() { 
  /* read directory size and store into variable */
  FwSizeType dir_size = 0;
  dir_size_status = this->update_dir.getFileCount(dir_size);
  if (dir_size_status != Os::Directory::Status::OP_OK) {
    /* */
  }
  /* else the directory size was read - log */
  this->log_WARNING_LO_OTA_AuditStatus(Fw::String("Update Directory size read"));
  /* read directory contents and store into vector */
  Fw::String dir_contents[dir_size];
  FwSizeType dir_count = 0;
  dir_read_status = this->update_dir.readDirectory(dir_contents, dir_size, dir_count);
  if (dir_read_status != Os::Directory::Status::OP_OK) {
    /* TODO */
  }
  /* else the directory was read - log */
  this->log_WARNING_LO_OTA_AuditStatus(Fw::String("Update Directory contents read"));
  /* iterate through directory contents */
  for(int i = 0; i < dir_count; i++) { 
    /* ensure path*/
    std::string path = UPDATE_DIR_PATH;
    path += "/";
    path += dir_contents[i].toChar();
    /* get path type */
    Os::FileSystem::PathType path_type = Os::FileSystem::getPathType(path.c_str());
    if (path_type == Os::FileSystem::PathType::DIRECTORY) {
      /* evaluate_directory */
      /* log */
      this->log_WARNING_LO_OTA_AuditStatus(Fw::String("Directory found in Update Directory"));
      this->evaluate_directory(path);
    } else if (path_type == Os::FileSystem::PathType::FILE) {
      /* log */
      this->log_WARNING_LO_OTA_AuditStatus(Fw::String("File found in Update Directory"));
      /* TODO */
    } else {
      /* */
    }
  } // for
} // perform_audit

void OTAUpdater::evaluate_directory(std::string path) { 
  /* try to open dir at path */
  Os::Directory dir;
  Os::Directory::Status dir_status = dir.open(path.c_str(), Os::Directory::OpenMode::READ);
  if (dir_status != Os::Directory::Status::OP_OK) {
    /* */
  }
  /* else the directory was opened - log */
  this->log_WARNING_LO_OTA_AuditStatus(Fw::String("Directory opened"));
  /* read directory size and store into variable */
  FwSizeType dir_size = 0;
  dir_size_status = dir.getFileCount(dir_size);
  if (dir_size_status != Os::Directory::Status::OP_OK) {
    /* */
  }
  /* else the directory size was read - log */
  this->log_WARNING_LO_OTA_AuditStatus(Fw::String("Directory size read"));
  /* read directory contents and store into vector */
  Fw::String dir_contents[dir_size];
  FwSizeType dir_count = 0;
  dir_read_status = dir.readDirectory(dir_contents, dir_size, dir_count);
  if (dir_read_status != Os::Directory::Status::OP_OK) {
    /* */
  }
  /* else the directory was read - log */
  this->log_WARNING_LO_OTA_AuditStatus(Fw::String("Directory contents read"));

  /* black hole ... im tired of writing code for now */

  /* iterate through directory contents */
  for(int i = 0; i < dir_count; i++) { 
    /* ensure_path */
    std::string new_path = path;
    new_path += "/";
    new_path += dir_contents[i].toChar();
    bool tracking_present, concat_present, seg_present;
    std::string concat_path = path;  
    /* only files should be present here */ 
    Os::FileSystem::PathType path_type = Os::FileSystem::getPathType(new_path.c_str());
    if (path_type == Os::FileSystem::PathType::FILE) {
      /* log */
      this->log_WARNING_LO_OTA_AuditStatus(Fw::String("File found in Directory"));
      /* check if file is of name 'tracking.txt' */
      tracking_present = (dir_contents[i] == "tracking.txt");
      std::string filename = dir_contents[i].toChar();
      /* check if file ends with '.concat' */
      concat_present = (filename.find(".concat") != std::string::npos);
      concat_path += '/'; 
      concat_path += filename; 
      /* check if file ends with '.seg' */
      seg_present = (filename.find(".seg") != std::string::npos);
    } else {
      /* TODO: subdir should not be present ... remove ? */
    }
  } // for
  

}


} // namespace UpdaterModule
