// ======================================================================
// \title  StateMachine.hpp
// \author g83r
// \brief  hpp file for StateMachine component implementation class
// ======================================================================

#ifndef StateMachineModule_StateMachine_HPP
#define StateMachineModule_StateMachine_HPP

#include "Components/StateMachine/StateMachineComponentAc.hpp"

namespace StateMachineModule {

class StateMachine : public StateMachineComponentBase {

public:
  // ----------------------------------------------------------------------
  // Component construction and destruction
  // ----------------------------------------------------------------------

  //! Construct StateMachine object
  StateMachine(const char *const compName //!< The component name
  );

  //! Destroy StateMachine object
  ~StateMachine();

PRIVATE:
  // ----------------------------------------------------------------------
  // Handler implementations for user-defined typed input ports
  // ----------------------------------------------------------------------

  //! Handler implementation for Run
  //!
  //! Port receiving calls from the rate group
  void Run_handler(FwIndexType portNum, //!< The port number
                   U32 context          //!< The call order
                   ) override;

  //! Handler implementation for SM_ChangeState
  //!
  //! Port for manually changing the state of the StateMachine
  void SM_ChangeState_handler(
      FwIndexType portNum,                      //!< The port number
      const StateMachineModule::SM_State &state //!< The state of the component
      ) override;

  //! Handler implementation for SM_EpsCommandIn
  //!
  //! Port to receive a command from the eps of a state change
  void
  SM_EpsCommandIn_handler(FwIndexType portNum, //!< The port number
                          const EpsModule::EPS_CommandRecv &command) override;

PRIVATE:
  // ----------------------------------------------------------------------
  // Handler implementations for commands
  // ----------------------------------------------------------------------

  //! Handler implementation for command SM_ChangeState
  //!
  //! Command to change state of the StateMachine
  //! read: https://github.com/nasa/fprime/releases/tag/v3.5.0
  //! section: FPP Changes
  void SM_ChangeState_cmdHandler(FwOpcodeType opCode, //!< The opcode
                                 U32 cmdSeq, //!< The command sequence number
                                 StateMachineModule::SM_State state) override;

  //! Handler implementation for command SM_SetCruiseMode
  //!
  //! Command from Ground State to set the cruise mode
  void SM_SetCruiseMode_cmdHandler(
      FwOpcodeType opCode, //!< The opcode
      U32 cmdSeq,          //!< The command sequence number
      StateMachineModule::SM_CruiseMode
          mode //!< Cruise mode to set to from the Ground Station
      ) override;

  //! Handler implementation for command SM_EpsCommandOut
  //!
  //! Command to send a command to the EPS
  void SM_EpsCommandOut_cmdHandler(
      FwOpcodeType opCode, //!< The opcode
      U32 cmdSeq,          //!< The command sequence number
      StateMachineModule::SM_EpsCommand command) override;
};

} // namespace StateMachineModule

#endif
