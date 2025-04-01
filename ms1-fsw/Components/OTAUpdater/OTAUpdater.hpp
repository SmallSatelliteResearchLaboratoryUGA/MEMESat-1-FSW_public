// ======================================================================
// \title  OTAUpdater.hpp
// \author g83r
// \brief  hpp file for OTAUpdater component implementation class
// ======================================================================

#ifndef UpdaterModule_OTAUpdater_HPP
#define UpdaterModule_OTAUpdater_HPP

#include "Components/OTAUpdater/OTAUpdaterComponentAc.hpp"
#include "Fw/Types/BasicTypes.hpp"
#include "Fw/Types/Assert.hpp"
#include "Fw/Types/Serializable.hpp"
#include "Fw/Types/String.hpp"
#include "FpConfig.hpp"
#include "Os/Directory.hpp"
#include "Os/File.hpp"
#include "Os/FileSystem.hpp"
#include "Os/Mutex.hpp"
#include <algorithm>
#include <fstream>
#include <iomanip> 
#include <iostream> 
#include <sstream> 
#include <string>
#include <vector>
#include <ctype.h>
#include <map> 
#include <openssl/md5.h> // Ref : https://www.geeksforgeeks.org/get-the-md5-hash-of-a-file-in-cpp/

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"


namespace UpdaterModule {

class OTAUpdater : public OTAUpdaterComponentBase {

public:

  // namespace variables 
  const char* UPDATE_DIR_PATH = "./update_dir"; 
  const char* TRACKING_FILE_PATH = "./update_dir/tracking.txt";

  // ----------------------------------------------------------------------
  // Component construction and destruction
  // ----------------------------------------------------------------------

  //! Construct OTAUpdater object
  OTAUpdater(const char *const compName //!< The component name
  );

  //! Destroy OTAUpdater object
  ~OTAUpdater();

  //! Initialize OTAUpdater object
  void init(const NATIVE_INT_TYPE queueDepth, //!< The queue depth
            const NATIVE_INT_TYPE instance //!< The instance number
  );

PRIVATE:
  // ----------------------------------------------------------------------
  // Handler implementations for user-defined typed input ports
  // ----------------------------------------------------------------------

  void preamble() override; 

  void parameterUpdated( 
    FwPrmIdType id //!< The parameter ID
  ) override;

  //! Handler implementation for readyForPowerOff
  //!
  //! Port for receiving the power off status
  void
  readyForPowerOff_handler(FwIndexType portNum, //!< The port number
                           const StateMachineModule::SM_ReadyForPowerOff
                               &result //!< The result of the power off request
                           ) override;

  //! Handler implementation for scheduledHandler
  //!
  //! Port for handing the schedule
  void scheduledHandler_handler(
      FwIndexType portNum, //!< The port number
      const SchedulerModule::ScheduleStatus &status) override;

  //! Handler implementation for updaterOpIn
  //!
  //! Port receiving the operation request
  void updaterOpIn_handler(FwIndexType portNum, //!< The port number
                           const UpdaterModule::UpdateOp &op) override;

PRIVATE:
  // ----------------------------------------------------------------------
  // Handler implementations for commands
  // ----------------------------------------------------------------------

  //! Handler implementation for command OTA_PerformAudit
  //!
  //! Which boot pin to use
  //! Which state the reset pin should be in
  //! The time to wait for the STM to boot
  //! The time to wait for the STM to reset
  //! Command to run an audit of the current files in scope
  void OTA_PerformAudit_cmdHandler(FwOpcodeType opCode, //!< The opcode
                                   U32 cmdSeq, //!< The command sequence number
                                   const Fw::CmdStringArg &dir_path) override;

  //! Handler implementation for command OTA_PerformFileOperation
  //!
  //! Command to perform a file operation on a group of files in scope
  void OTA_PerformFileOperation_cmdHandler(
      FwOpcodeType opCode, //!< The opcode
      U32 cmdSeq,          //!< The command sequence number
      U32 operation, const Fw::CmdStringArg &file_path) override;

  //! Handler implementation for command OTA_SelectBootPin
  //!
  //! Command to select the boot pin
  void OTA_SelectBootPin_cmdHandler(FwOpcodeType opCode, //!< The opcode
                                    U32 cmdSeq, //!< The command sequence number
                                    U32 bootOutNum) override;

  //! Handler implementation for command OTA_SelectRstState
  //!
  //! Command to select the reset pin state
  void
  OTA_SelectRstState_cmdHandler(FwOpcodeType opCode, //!< The opcode
                                U32 cmdSeq, //!< The command sequence number
                                U32 rstState) override;

  PRIVATE: 

  void perform_audit();
  void evaluate_directory(std::string path); 


  PRIVATE: 
    
    // ----------------------------------------------------------------------
    // Variables 
    // ----------------------------------------------------------------------
    Os::Directory update_dir; 
    Os::Directory::Status dir_status;
    Os::File tracking_file; 
    Os::File::Status tracking_open_status; 
    Os::FileSystem::Status touch_status;
    Os::File::Status tracking_size_status; 
    Os::File::Status tracking_read_status;
    Os::Directory::Status dir_size_status; 
    Os::Directory::Status dir_read_status;
    //std::vector<std::pair<Fw::String, std::vector<std::pair<Fw::String, Fw::String>>>> dir_idx_param_vec;
    std::map<Fw::String, std::vector<std::pair<Fw::String, Fw::String>>> dir_idx_param_map;


};

} // namespace UpdaterModule

#endif
