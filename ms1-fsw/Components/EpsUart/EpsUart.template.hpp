// ======================================================================
// \title  EpsUart.hpp
// \author g83r
// \brief  hpp file for EpsUart component implementation class
// ======================================================================

#ifndef EpsModule_EpsUart_HPP
#define EpsModule_EpsUart_HPP

#include "Components/EpsUart/EpsUartComponentAc.hpp"

namespace EpsModule {

class EpsUart : public EpsUartComponentBase {

public:
  // ----------------------------------------------------------------------
  // Component construction and destruction
  // ----------------------------------------------------------------------

  //! Construct EpsUart object
  EpsUart(const char *const compName //!< The component name
  );

  //! Destroy EpsUart object
  ~EpsUart();

PRIVATE:
  // ----------------------------------------------------------------------
  // Handler implementations for user-defined typed input ports
  // ----------------------------------------------------------------------

  //! Handler implementation for ExternCommandBuffer
  //!
  //! Input port for components to send commands to the EPS
  void ExternCommandBuffer_handler(
      FwIndexType portNum, //!< The port number
      const EpsModule::EPS_CommandRecv &command) override;

  //! Handler implementation for UartRead
  //!
  //! Input port for receiving UART data
  void UartRead_handler(FwIndexType portNum, //!< The port number
                        Fw::Buffer &recvBuffer,
                        const Drv::RecvStatus &recvStatus) override;

PRIVATE:
  // ----------------------------------------------------------------------
  // Handler implementations for commands
  // ----------------------------------------------------------------------

  //! Handler implementation for command EPS_ReportStatus
  //!
  //! Command to report the status of the EPS
  void EPS_ReportStatus_cmdHandler(FwOpcodeType opCode, //!< The opcode
                                   U32 cmdSeq //!< The command sequence number
                                   ) override;

  //! Handler implementation for command EPS_ListenToRadio
  //!
  //! Command to tell eps to listen to the Radio
  void
  EPS_ListenToRadio_cmdHandler(FwOpcodeType opCode, //!< The opcode
                               U32 cmdSeq, //!< The command sequence number
                               U8 listen   //!< 1 to listen, 0 to stop listening
                               ) override;

  //! Handler implementation for command EPS_TriggerReset
  //!
  //! Command to tell eps to reset
  void EPS_TriggerReset_cmdHandler(FwOpcodeType opCode, //!< The opcode
                                   U32 cmdSeq //!< The command sequence number
                                   ) override;

  //! Handler implementation for command EPS_AdjustParameter
  //!
  //! Command to tell eps to adjust a parameter
  void EPS_AdjustParameter_cmdHandler(
      FwOpcodeType opCode, //!< The opcode
      U32 cmdSeq,          //!< The command sequence number
      U8 parameter,        //!< The parameter to adjust
      U8 value             //!< The value to adjust the parameter to
      ) override;
};

} // namespace EpsModule

#endif
