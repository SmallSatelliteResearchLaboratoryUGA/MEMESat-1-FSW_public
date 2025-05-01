// ======================================================================
// \title  StateMachine.cpp
// \author g83r
// \brief  cpp file for StateMachine component implementation class
// ======================================================================

#include "Components/StateMachine/StateMachine.hpp"
#include "FpConfig.hpp"

namespace StateMachineModule {

// ----------------------------------------------------------------------
// Component construction and destruction
// ----------------------------------------------------------------------

StateMachine ::StateMachine(const char *const compName)
    : StateMachineComponentBase(compName) {}

StateMachine ::~StateMachine() {}

// ----------------------------------------------------------------------
// Handler implementations for user-defined typed input ports
// ----------------------------------------------------------------------

void StateMachine ::Run_handler(FwIndexType portNum, U32 context) {
  // TODO
}

void StateMachine ::SM_ChangeState_handler(
    FwIndexType portNum, const StateMachineModule::SM_State &state) {
  // TODO
}

void StateMachine ::SM_EpsCommandIn_handler(
    FwIndexType portNum, const EpsModule::EPS_CommandRecv &command) {
  // TODO
}

// ----------------------------------------------------------------------
// Handler implementations for commands
// ----------------------------------------------------------------------

void StateMachine ::SM_ChangeState_cmdHandler(
    FwOpcodeType opCode, U32 cmdSeq, StateMachineModule::SM_State state) {
  // TODO
  this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
}

void StateMachine ::SM_SetCruiseMode_cmdHandler(
    FwOpcodeType opCode, U32 cmdSeq, StateMachineModule::SM_CruiseMode mode) {
  // TODO
  this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
}

void StateMachine ::SM_EpsCommandOut_cmdHandler(
    FwOpcodeType opCode, U32 cmdSeq,
    StateMachineModule::SM_EpsCommand command) {
  // TODO
  this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
}

} // namespace StateMachineModule
