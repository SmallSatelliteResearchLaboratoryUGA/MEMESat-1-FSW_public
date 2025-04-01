// ======================================================================
// \title  FileRecycler.hpp
// \author g83r
// \brief  hpp file for FileRecycler component implementation class
// ======================================================================

#ifndef FileRecyclerModule_FileRecycler_HPP
#define FileRecyclerModule_FileRecycler_HPP

#include "Components/FileRecycler/FileRecyclerComponentAc.hpp"
#include <Fw/Types/BasicTypes.hpp>
#include <FpConfig.hpp>
#include <Os/Directory.hpp> 
#include <Os/FileSystem.hpp> 
#include <array> 
#include <string> 
#include <vector>
#include <utility> 
#include <mutex> 
#include <unistd.h> 

#define MEMESAT_STORAGE_DIR_MAX_LEN 256

#ifdef __arm__
  #define MEMESAT_STORAGE_DIR "/storage/images"
#else
  #ifdef __APPLE__ 
      #define MEMESAT_STORAGE_DIR "/Users/REPLACEME" 
   // #define MEMESAT_STORAGE_DIR "/Users/aiden/MEMESat-1-FlightSoftware/Ref/storage/"
  #endif
  #ifdef __linux__
    #define MEMESAT_STORAGE_DIR "/storage/images"
  #endif
#endif

constexpr std::array<const char*, 12> months = { 
  "January", "February", "March", "April", "May", "June",
  "July", "August", "September", "October", "November", "December"
}; 

namespace FileRecyclerModule {

  class FileRecycler :
    public FileRecyclerComponentBase
  {

    public:

      struct FauxOsStatus {
        Fw::String status; 
        Fw::String status_message; 
        void setstatus(const char* status) { this->status = status; }
        void setstatus_message(const char* status_message) { this->status_message = status_message; }
      };

      // ----------------------------------------------------------------------
      // Component construction and destruction
      // ----------------------------------------------------------------------

      //! Construct FileRecycler object
      FileRecycler(
          const char* const compName //!< The component name
      );

      //! Initialize FileRecycler object
      void init(
          const NATIVE_INT_TYPE queueDepth, //!< The queue depth
          const NATIVE_INT_TYPE instance = 0 //!< The instance number
      );

      //! Destroy FileRecycler object
      ~FileRecycler();

    PRIVATE:

      void preamble() override; 

      void parameterUpdated(
          FwPrmIdType id //!< The parameter ID
      ) override;

      void recycler_call_on_reset_handler(); 

      template <typename T>
      FauxOsStatus convertOsStatusToFaux(const T& status);

      void handleStatError(int valueFromStat); 

      void logRecyclerOutput(
        std::vector<std::tuple<time_t, std::string, FauxOsStatus>> return_value
      );


      // ----------------------------------------------------------------------
      // Handler implementations for user-defined typed input ports
      // ----------------------------------------------------------------------

      //! Handler implementation for cleanupDataPathRecv
      //!
      //! Port for receiving the path to clean up
      void cleanupDataPathRecv_handler(
          FwIndexType portNum, //!< The port number
          const FileRecyclerModule::CleanUpData& path //!< The path to the directory to be recycled
      ) override;

      //! Handler implementation for fileRecyclerOpIn
      //!
      //! Port for recieving the operation request
      void fileRecyclerOpIn_handler(
          FwIndexType portNum, //!< The port number
          U32 count, //!< File attribute that determines which files are deleted
          const FileRecyclerModule::FileRecyclerOp& op
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
          const SchedulerModule::ScheduleStatus& status
      ) override;

    PRIVATE:

      // ----------------------------------------------------------------------
      // Handler implementations for commands
      // ----------------------------------------------------------------------

      //! Handler implementation for command FR_CleanUpData
      //!
      //! Clean up the files
      void FR_CleanUpData_cmdHandler(
          FwOpcodeType opCode, //!< The opcode
          U32 cmdSeq, //!< The command sequence number
          U32 count, //!< file attribute that determines which files are deleted
          FileRecyclerModule::FileRecyclerOp op
      ) override;

      bool shutdown; 
      Os::Directory directory; 
      std::vector<std::tuple<time_t, std::string, FileRecycler::FauxOsStatus>> recycler(U32 count, FileRecyclerModule::FileRecyclerOp &op);
      std::mutex directory_mutex;

  };

}

#endif
