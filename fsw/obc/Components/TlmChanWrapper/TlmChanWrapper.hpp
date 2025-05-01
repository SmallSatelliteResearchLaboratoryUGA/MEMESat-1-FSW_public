// ======================================================================
// \title  TlmChanWrapper.hpp
// \author g83r
// \brief  hpp file for TlmChanWrapper component implementation class
// ======================================================================

#ifndef TlmChanModule_TlmChanWrapper_HPP
#define TlmChanModule_TlmChanWrapper_HPP

#include "Components/TlmChanWrapper/TlmChanWrapperComponentAc.hpp"

namespace TlmChanModule {

  class TlmChanWrapper :
    public TlmChanWrapperComponentBase
  {

    public:

      // ----------------------------------------------------------------------
      // Component construction and destruction
      // ----------------------------------------------------------------------

      //! Construct TlmChanWrapper object
      TlmChanWrapper(
          const char* const compName //!< The component name
      );

      //! Destroy TlmChanWrapper object
      ~TlmChanWrapper();

    PRIVATE:

      void preamble() override; 

      void parameterUpdated(
          FwPrmIdType id
      ) override;

      // ----------------------------------------------------------------------
      // Handler implementations for user-defined typed input ports
      // ----------------------------------------------------------------------

      //! Handler implementation for scheduledHandler
      //!
      //! Input port for scheduled handler
      void scheduledHandler_handler(
          FwIndexType portNum, //!< The port number
          const SchedulerModule::ScheduleStatus& status
      ) override;

      // ----------------------------------------------------------------------
      // Command handler implementations
      // ----------------------------------------------------------------------

      //! Implementation for change_schedule command handler 
      //! clean up data
      void change_schedule_cmdHandler(
          FwOpcodeType opCode, //!< The opcode
          U32 cmdSeq, //!< The command sequence number
          const Fw::CmdStringArg& name, //!< The name of the schedule
          const Fw::CmdStringArg& schedule, //!< The schedule
          const SchedulerModule::ScheduleOp& action //!< The action
      ); //override;


  };

}

#endif
