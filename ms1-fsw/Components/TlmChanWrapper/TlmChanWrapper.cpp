// ======================================================================
// \title  TlmChanWrapper.cpp
// \author g83r
// \brief  cpp file for TlmChanWrapper component implementation class
// ======================================================================

#include "Components/TlmChanWrapper/TlmChanWrapper.hpp"
#include "FpConfig.hpp"

namespace TlmChanModule {

  // ----------------------------------------------------------------------
  // Component construction and destruction
  // ----------------------------------------------------------------------

  TlmChanWrapper ::
    TlmChanWrapper(const char* const compName) :
      TlmChanWrapperComponentBase(compName)
  {

  }

  TlmChanWrapper ::
    ~TlmChanWrapper()
  {

  }

  void TlmChanWrapper :: 
    preamble()
  {
    Fw::ParamValid valid;
    Fw::ParamString val = this->paramGet_TLMWR_Schedule(valid);
    const char* schedule_parameter = val.toChar();
    FW_ASSERT(
      valid.e == Fw::ParamValid::VALID || valid.e == Fw::ParamValid::DEFAULT, 
      valid.e
    );
    this->sendSchedule_out(0, Fw::String("TlmChan"), Fw::String(schedule_parameter), SchedulerModule::ScheduleOp::START);
  }

  // ----------------------------------------------------------------------
  // Handler implementations for user-defined typed input ports
  // ----------------------------------------------------------------------

  void TlmChanWrapper ::
    scheduledHandler_handler(
        FwIndexType portNum,
        const SchedulerModule::ScheduleStatus& status
    )
  {
    if (status == SchedulerModule::ScheduleStatus::FAILED || status == SchedulerModule::ScheduleStatus::STOPPED) 
    {
      this->log_WARNING_LO_TLMWR_ScheduleError(status);
      return;
    }
    this->tlmChanOut_out(0,0);
  }

  // ----------------------------------------------------------------------
  // Command handler implementations
  // ----------------------------------------------------------------------

  void TlmChanWrapper :: 
    parameterUpdated(
        FwPrmIdType id
    )
  {
    if (id == 1) 
    {
      Fw::ParamValid valid; 
      const Fw::ParamString schedule_param = this->paramGet_TLMWR_Schedule(valid);
      const char* schedule = schedule_param.toChar();
      FW_ASSERT(
        valid.e == Fw::ParamValid::VALID || valid.e == Fw::ParamValid::DEFAULT, 
        valid.e
      );
      this->sendSchedule_out(0, Fw::String("TlmChan"), Fw::String(schedule), SchedulerModule::ScheduleOp::START);
      this->log_ACTIVITY_LO_TLMWR_ScheduleChangedTo(Fw::String(schedule));
    }
  }

}
