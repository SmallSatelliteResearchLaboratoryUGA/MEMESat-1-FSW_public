// ======================================================================
// \title  EpsUart.cpp
// \author g83r
// \brief  cpp file for EpsUart component implementation class
// ======================================================================

#include "Components/EpsUart/EpsUart.hpp"
#include "FpConfig.hpp"

namespace EpsModule {

// ----------------------------------------------------------------------
// Component construction and destruction
// ----------------------------------------------------------------------

EpsUart ::EpsUart(const char *const compName)
    : EpsUartComponentBase(compName) {}

EpsUart ::~EpsUart() {}

// ----------------------------------------------------------------------
// Handler implementations for user-defined typed input ports
// ----------------------------------------------------------------------

void EpsUart ::ExternCommandBuffer_handler(
    FwIndexType portNum, const EpsModule::EPS_CommandRecv &command) {
  // TODO
}

void EpsUart ::UartRead_handler(FwIndexType portNum, Fw::Buffer &recvBuffer,
                                const Drv::RecvStatus &recvStatus) {
  // TODO
}

// ----------------------------------------------------------------------
// Handler implementations for commands
// ----------------------------------------------------------------------

void EpsUart ::EPS_ReportStatus_cmdHandler(FwOpcodeType opCode, U32 cmdSeq) {
  // TODO
  this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
}

void EpsUart ::EPS_ListenToRadio_cmdHandler(FwOpcodeType opCode, U32 cmdSeq,
                                            U8 listen) {
  // TODO
  this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
}

void EpsUart ::EPS_TriggerReset_cmdHandler(FwOpcodeType opCode, U32 cmdSeq) {
  // TODO
  this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
}

void EpsUart ::EPS_AdjustParameter_cmdHandler(FwOpcodeType opCode, U32 cmdSeq,
                                              U8 parameter, U8 value) {
  // TODO
  this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
}

} // namespace EpsModule
