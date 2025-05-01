// ======================================================================
// \title  EpsUart.hpp
// \author g83r
// \brief  hpp file for EpsUart component implementation class
// ======================================================================

#ifndef EpsModule_EpsUart_HPP
#define EpsModule_EpsUart_HPP

#include "Components/EpsUart/EpsUartComponentAc.hpp"
#include <semaphore.h> 
#include <vector>
#include <fcntl.h> 
#include <pthread.h> 

#define NUM_UART_BUFFERS 5
#define UART_READ_BUFF_SIZE 40 

namespace EpsModule {

  class 
  EpsUart : 
    public EpsUartComponentBase 
  {

    public:
      // ----------------------------------------------------------------------
      // Component construction and destruction
      // ----------------------------------------------------------------------

      //! Construct EpsUart object
      EpsUart(
        const char *const compName //!< The component name
      );

      void
      init(
        NATIVE_INT_TYPE queueDepth, //!< The queue depth
        NATIVE_INT_TYPE instance = 0 //!< The instance number
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
      void 
      ExternCommandBuffer_handler(
        FwIndexType portNum, //!< The port number
        const EpsModule::EPS_CommandRecv &command
      ) override;

      //! Handler implementation for UartRead
      //!
      //! Input port for receiving UART data
      void 
      UartRead_handler(
        FwIndexType portNum, //!< The port number
        Fw::Buffer &recvBuffer,
        const Drv::RecvStatus &recvStatus
      ) override;

    PRIVATE:
      // ----------------------------------------------------------------------
      // Handler implementations for commands
      // ----------------------------------------------------------------------

      //! Handler implementation for command EPS_ReportStatus
      //!
      //! Command to report the status of the EPS
      void 
      EPS_ReportStatus_cmdHandler(
        FwOpcodeType opCode, //!< The opcode
        U32 cmdSeq //!< The command sequence number
      ) override;

      //! Handler implementation for command EPS_ListenToRadio
      //!
      //! Command to tell eps to listen to the Radio
      void
      EPS_ListenToRadio_cmdHandler(
        FwOpcodeType opCode, //!< The opcode
        U32 cmdSeq, //!< The command sequence number
        U8 listen   //!< 1 to listen, 0 to stop listening
      ) override;

      //! Handler implementation for command EPS_TriggerReset
      //!
      //! Command to tell eps to reset
      void
      EPS_TriggerReset_cmdHandler(
        FwOpcodeType opCode, //!< The opcode
        U32 cmdSeq //!< The command sequence number
      ) override;

      //! Handler implementation for command EPS_AdjustParameter
      //!
      //! Command to tell eps to adjust a parameter
      void
      EPS_AdjustParameter_cmdHandler(
        FwOpcodeType opCode, //!< The opcode
        U32 cmdSeq,          //!< The command sequence number
        U8 parameter,        //!< The parameter to adjust
        U8 value             //!< The value to adjust the parameter to
      ) override;

      // -------- 
      // Command handler implementations 
      // --------

      uint16_t 
      CalcChecksum(
        uint8_t *data, 
        uint16_t length
      ); 

      uint16_t 
      CalcChecksum(
        std::vector<U8> buffer 
      );

      void 
      ResendPacket(); 

      void
      ParseUart(
        uint8_t *data, 
        long size
      );

      void Resend(); 

      void Preamble(); 

      void ProcessBuffer(uint8_t *buffer, long size);

      void DispatchPacket(EpsModule::EPS_CommandRecv command);

      // --- 
      // Private Member Variables 
      // --- 

      // telemetry values: 
      U32 m_uartRecvBytes;
      Fw::TlmString m_LastMsg;
      F32 m_vbattVoltage;
      F32 m_vbattCurrent;
      F32 m_cellVoltage;
      F32 m_cellCurrent;
      F32 m_battTemp;
      F32 m_solarPanelVoltage;
      F32 m_solarPanelCurrent;
      F32 m_solarPanelTemp;
      F32 m_voltage5v0;
      F32 m_current5v0;
      F32 m_voltage3v3;
      F32 m_current3v3;
      U8  m_switchState;
      // Serial Buffers
      Fw::Buffer outBuff;
      Fw::Buffer lastBuff;
      std::vector<U8> packetBuff;
      uint8_t packetSize;

      Fw::Buffer m_recvBuffers[NUM_UART_BUFFERS];
      BYTE m_uartBuffers[NUM_UART_BUFFERS][UART_READ_BUFF_SIZE];
      uint8_t heartbeat_ack_pkt[7] = {0xBE, 0xEF, 0x00, 0x01, 0x04, 0xB2, 0x78}; 
      uint8_t heartbeat_pkt[6] = {0xBE, 0xEF, 0x04, 0x00, 0xB1, 0xCD}; 
      uint8_t cruise_pkt[7] = {0xBE, 0xEF, 0x05, 0x01, 0x00, 0xB3, 0x83};
      uint8_t idle_pkt[7] = {0xBE, 0xEF, 0x05, 0x01, 0x01, 0xB4, 0x84};
      uint8_t reset_pkt[7] = {0xBE, 0xEF, 0x05, 0x01, 0x02, 0xB5, 0x85};
      uint8_t msg_type, payload_len, pkt_msb, pkt_lsb, lsb, msb;
      uint16_t local_checksum, checksum;

      // Semaphore
      sem_t m_uartSem;

  };

} // namespace EpsModule

#endif
