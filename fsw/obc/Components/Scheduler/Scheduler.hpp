// ======================================================================
// \title  Scheduler.hpp
// \author g83r
// \brief  hpp file for Scheduler component implementation class
// ======================================================================

#ifndef SchedulerModule_Scheduler_HPP
#define SchedulerModule_Scheduler_HPP

#include "Components/Scheduler/SchedulerComponentAc.hpp"
#include "Svc/FileDownlink/FileDownlink.hpp"
#include "Fw/Logger/Logger.hpp"
#include "FpConfig.hpp"
#include "Fw/Types/BasicTypes.hpp"
#include <stdlib.h>
#include <thread> 
#include <chrono>
#include <algorithm> 
#include <functional> 
#include <iostream> 
#include <fstream>
#include <regex>
#include <vector>
#include <unordered_map>
#include "libcron/Task.h"
#include "libcron/TaskQueue.h"
#include "libcron/CronClock.h"
#include "libcron/Cron.h"

std::string formatDuration(std::chrono::system_clock::duration duration);

namespace SchedulerModule {

  class Scheduler :
    public SchedulerComponentBase
  {

    public:

      // ----------------------------------------------------------------------
      // Component construction and destruction
      // ----------------------------------------------------------------------

      //! Construct Scheduler object
      Scheduler(
          const char* const compName //!< The component name
      );


      //! Initialize Scheduler object 
      //! 
      void init(
        const NATIVE_INT_TYPE queueDepth, //!< The queue depth
        const NATIVE_INT_TYPE instance = 0 //!< The instance number
      );

      //! Destroy Scheduler object
      ~Scheduler();

    PRIVATE:

      // ----------------------------------------------------------------------
      // Handler implementations for user-defined typed input ports
      // ----------------------------------------------------------------------

      //! Handler implementation for getSchedule
      //!
      //! Port for getting the schedule
      void getSchedule_handler(
          FwIndexType portNum, //!< The port number
          const Fw::StringBase& name, //!< Name of schedule for libcron task
          const Fw::StringBase& schedule, //!< Schedule in libcron format
          const SchedulerModule::ScheduleOp& action
      ) override;

      //! Handler implementation for tick
      //!
      //! The rate group scheduler input
      void tick_handler(
          FwIndexType portNum,           //!< The port number
          U32 context //!< The call order
      ) override;

    PRIVATE:

      // ----------------------------------------------------------------------
      // Handler implementations for commands
      // ----------------------------------------------------------------------

      //! Handler implementation for command SCH_Get_Schedule_List
      //!
      //! Downlinks file containing a list of the current schedules
      void SCH_GetScheduleList_cmdHandler(
          FwOpcodeType opCode, //!< The opcode
          U32 cmdSeq, //!< The command sequence number
          const Fw::CmdStringArg& destFileName //!< Path to store downlinked schedule list at
      ) override;

      //! Handler implementation for command SCH_Stop_Schedule
      //!
      //! Stops the schedule
      void SCH_StopSchedule_cmdHandler(
          FwOpcodeType opCode, //!< The opcode
          U32 cmdSeq, //!< The command sequence number
          const Fw::CmdStringArg& name //!< Name of schedule to stop
      ) override;

      //! Handler implementation for command SCH_CreateSchedule 
      //!
      //! Creates a schedule
      void SCH_CreateSchedule_cmdHandler(
          FwOpcodeType opCode, //!< The opcode
          U32 cmdSeq, //!< The command sequence number
          const Fw::CmdStringArg& name, //!< Name of schedule to create
          const Fw::CmdStringArg& schedule, //!< Schedule in libcron format
          const Fw::CmdStringArg& capture_param, //!< Capture parameter
          const Fw::CmdStringArg& lambda_param, //!< Lambda parameter
          const Fw::CmdStringArg& lambda_func //!< Lambda function
      ) override;


  };

}

#endif
