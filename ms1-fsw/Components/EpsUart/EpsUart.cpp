// ======================================================================
// \title  EpsUart.cpp
// \author g83r
// \brief  cpp file for EpsUart component implementation class
// ======================================================================

#include "Components/EpsUart/EpsUart.hpp"
#include "FpConfig.hpp"
#include <Fw/Types/BasicTypes.hpp>
#include "Fw/Types/StringUtils.hpp"
#include <ctype.h> 
// Testing import -- Start 
#include <sstream> 
#include <iomanip> 
#include <string> 
#include <iostream> 
#include <stdio.h> 
// Testing import -- End 

namespace EpsModule {

  // ----------------------------------------------------------------------
  // Component construction and destruction
  // ----------------------------------------------------------------------

  EpsUart ::
    EpsUart(const char *const compName) : 
      EpsUartComponentBase(compName)
      , m_uartRecvBytes(0)
      , m_vbattVoltage(0.0)
      , m_vbattCurrent(0.0)
      , m_cellVoltage(0.0)
      , m_cellCurrent(0.0)
      , m_battTemp(0.0)
      , m_solarPanelVoltage(0.0)
      , m_solarPanelCurrent(0.0)
      , m_solarPanelTemp(0.0)
      , m_voltage5v0(0.0)
      , m_current5v0(0.0)
      , m_voltage3v3(0.0)
      , m_current3v3(0.0)
      , m_switchState(0) 
      {}

  void 
  EpsUart ::
    init(
      NATIVE_INT_TYPE queueDepth, 
      NATIVE_INT_TYPE instance
      ) 
  {
    EpsUartComponentBase::init(queueDepth, instance);
    // Initialize the semaphore
    sem_init(&this->m_uartSem, 0, 1);
  } // init

  EpsUart ::
    ~EpsUart() {}

  // -- 
  // Preamble 
  // -- 

  void 
  EpsUart ::
    Preamble()
  {
    for (NATIVE_INT_TYPE buffer = 0; buffer < NUM_UART_BUFFERS; buffer++)
    {
      // Assign the raw data to the buffer. Make sure to include the size of the region assigned. 
      this->m_recvBuffers[buffer].setData(this->m_uartBuffers[buffer]);
      this->m_recvBuffers[buffer].setSize(UART_READ_BUFF_SIZE);
      // Invoke the port to send the buffer out. 
      this->UartBuffers_out(0, this->m_recvBuffers[buffer]);
    } // for
    // this->requestTelemetry();
  } // Preamble

  // ----------------------------------------------------------------------
  // Handler implementations for user-defined typed input ports
  // ----------------------------------------------------------------------

  void 
  EpsUart ::
    ExternCommandBuffer_handler(
      FwIndexType portNum, 
      const EpsModule::EPS_CommandRecv &command
    ) 
  {
    switch (command) /* switch on command */
    {
      case EpsModule::EPS_CommandRecv::HEARTBEAT_PKT: 
      case EpsModule::EPS_CommandRecv::CRUISE_PKT: 
      case EpsModule::EPS_CommandRecv::IDLE_PKT:
      case EpsModule::EPS_CommandRecv::RESET_PKT:
        DispatchPacket(command);
        break;
      default:
        this->log_ACTIVITY_HI_EPS_UartMsgIn(Fw::String("UNKNOWN_CMD"));
        break;
    } // switch
  } // ExternCommandBuffer_handler

  void 
  EpsUart::
    UartRead_handler(
      FwIndexType portNum, 
      Fw::Buffer &recvBuffer,
      const Drv::RecvStatus &recvStatus
    ) 
  {
    /* grab semaphore */
    if (sem_wait(&this->m_uartSem) != 0) 
    {
        perror("Semaphore wait failed");
        return;
    } // if
    int status = 0; 
    // Grab the size (used amount of the buffer) and a pointer to the data in the buffer 
    NATIVE_INT_TYPE size = recvBuffer.getSize();
    U8 *data = recvBuffer.getData();
    // check for invalid read status, log an error, return buffer and abort if there is a problem 
    if (recvStatus != Drv::RecvStatus::RECV_OK)
    {
      // We must return the buffer or the serial driver won't be able to reuse it. The same buffer send call is used 
      // as we did in "preamble". Since the buffer's size was overwritten to hold the actual data size, we need to 
      // reset it to the full data block size before returning it
      recvBuffer.setSize(UART_READ_BUFF_SIZE);
      this->UartBuffers_out(0, recvBuffer);
      return;
    } // if 
    else if (recvStatus == Drv::RecvStatus::RECV_OK)
    {
      this->m_uartRecvBytes += size;
      this->ProcessBuffer(data, size);
      recvBuffer.setSize(UART_READ_BUFF_SIZE);  
      this->UartBuffers_out(0, recvBuffer); 
    } // else if
    else
    {
      this->log_ACTIVITY_HI_EPS_UartMsgIn(Fw::String("Invalid read status"));
      recvBuffer.setSize(UART_READ_BUFF_SIZE);
      this->UartBuffers_out(0, recvBuffer);
    } // else
    /* release semaphore */
    if (sem_post(&this->m_uartSem) != 0) 
    {
        perror("Semaphore post failed");
        return;
    } // if 
  } // UartRead_handler 

  // ----------------------------------------------------------------------
  // Handler implementations for commands
  // ----------------------------------------------------------------------

  void 
  EpsUart::
    EPS_ReportStatus_cmdHandler(
      FwOpcodeType opCode, 
      U32 cmdSeq
    ) 
  {
    // TODO
    this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
  } // EPS_ReportStatus_cmdHandler

  void 
  EpsUart::
    EPS_ListenToRadio_cmdHandler(
      FwOpcodeType opCode, 
      U32 cmdSeq,
      U8 listen
    ) 
  {
    // TODO
    this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
  } // EPS_ListenToRadio_cmdHandler

  void 
  EpsUart::
    EPS_TriggerReset_cmdHandler(
      FwOpcodeType opCode, 
      U32 cmdSeq
    ) 
  {
    // TODO
    this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
  } // EPS_TriggerReset_cmdHandler

  void
  EpsUart::
    EPS_AdjustParameter_cmdHandler(
      FwOpcodeType opCode, 
      U32 cmdSeq,
      U8 parameter, 
      U8 value
    ) 
  {
    // TODO
    this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
    uint8_t data[8] = {0xBE, 0xEF, 0x07, 0x02};
    data[4] = parameter;
    data[5] = value;
    uint16_t csum = CalcChecksum(data, 6);
    data[6] = (csum >> 8) & 0xFF;
    data[7] = csum & 0xFF;
    this->outBuff.setData(data);
    this->outBuff.setSize(8);
    this->UartBuffers_out(0, this->outBuff);
  } // EPS_AdjustParameter_cmdHandler

  uint16_t 
  EpsUart::
    CalcChecksum(
      uint8_t *data, 
      uint16_t length
    ) 
  {
    checksum = 0;
    for (uint8_t i = 0; i < length; i++)
    {
        lsb = checksum;
        msb = (checksum >> 8) + data[i];
        lsb += msb;
        checksum = ((uint16_t)msb << 8) | (uint16_t)lsb;
    } // for
    return checksum;
  } // CalcChecksum

  uint16_t 
  EpsUart::
    CalcChecksum(
      std::vector<U8> buffer
    ) 
  {
    // TODO
    return 0;
  } // CalcChecksum

  void
  EpsUart:: 
    ResendPacket()
  {
    // TODO
  } // ResendPacket

  void 
  EpsUart::
  ParseUart(
    uint8_t *data, 
    long size
    )
  {
    // TODO
  } // ParseUart

  void 
  EpsUart::
    Resend()
  {
    // TODO
  } // Resend

  void 
  EpsUart:: 
    ProcessBuffer(
      uint8_t *buffer, 
      long size
    )
  {
    /* check for sync bytes */
    if (buffer[0] == 0xBE && buffer[1] == 0xEF)
    {
      msg_type = buffer[2];                /* grab the message type */
      uint8_t data_type[2] = {NULL, NULL}; /* init data type var */
      bool heartbeat, ack, update = false; /* init flags */
      switch (msg_type)                    /* switch on msg type */
      {
        case 0x00: /* ack */
          payload_len = buffer[3];
          ack = true;
          break;
        case 0x01: /* heartbeat */
          payload_len = buffer[3];
          heartbeat = true;
          break;
        case 0x02: /* state update */
          payload_len = buffer[3];
          update = true;
          break; 
        case 0x03: /* single sensor indicator */
          data_type[0] = buffer[3];
          payload_len  = buffer[4];
          break; 
        case 0x04: /* sensor group indicator */
          data_type[0] = buffer[3];
          data_type[1] = buffer[4];
          payload_len  = buffer[5];
          break;
        case 0x05: /* parameter respond indicator */
          data_type[0] = buffer[3];
          payload_len  = buffer[4];
          break;
        default: 
          break;
      } // switch
      if (data_type[1] != NULL) // group
      { 
        /* grab msb and lsb using payload len */
        pkt_msb = buffer[payload_len + 5]; /* msb */
        pkt_lsb = buffer[payload_len + 6]; /* lsb */
        /* compute checksum and compare with msb and lsb */
        local_checksum = CalcChecksum(buffer, size);
        if (local_checksum != ((uint16_t)pkt_msb << 8) | (uint16_t)pkt_lsb)
        {
          this->ResendPacket();
          return;
        } // if
        this->log_ACTIVITY_HI_EPS_UartMsgIn(Fw::String("Checksum validated"));
      } // if 
      else if (data_type[0] != NULL) // single_sensor, prmResp
      { 
        pkt_msb = buffer[payload_len + 4]; /* msb */
        pkt_lsb = buffer[payload_len + 5]; /* lsb */
        /* compute checksum and compare with msb and lsb */ 
        local_checksum = CalcChecksum(buffer, size);
        if (local_checksum != ((uint16_t)pkt_msb << 8) | (uint16_t)pkt_lsb)
        {
          this->ResendPacket();
          return;
        } // if
        this->log_ACTIVITY_HI_EPS_UartMsgIn(Fw::String("Checksum validated"));
      } // else if 
      else // ack, hb, stUp
      { 
        pkt_msb = buffer[4]; /* msb */
        pkt_lsb = buffer[5]; /* lsb */
        /* compute checksum and compare with msb and lsb */
        this->CalcChecksum(buffer, size);
        if (local_checksum != ((uint16_t)pkt_msb << 8) | (uint16_t)pkt_lsb)
        {
          this->ResendPacket();
          return;
        } // if
        this->log_ACTIVITY_HI_EPS_UartMsgIn(Fw::String("Checksum validated"));
        if (heartbeat) /* heartbeat flag */
        {
          this->log_ACTIVITY_HI_EPS_UartMsgIn(Fw::String("Heartbeat received"));
          /* indicate statemachine of heartbeat */
          this->SM_CommandBuffer_out(0, EpsModule::EPS_CommandRecv::HEARTBEAT_PKT); 
          this->DispatchPacket(EpsModule::EPS_CommandRecv::HEARTBEAT_PKT);
        } // if
        else if (ack) /* ack flag */
        {
          this->log_ACTIVITY_HI_EPS_UartMsgIn(Fw::String("Ack received"));
          /* check if ack is for heartbeat */

        } // else if
        else if (update) /* update flag */
        {
          this->log_ACTIVITY_HI_EPS_UartMsgIn(Fw::String("State update received"));
        } // else if
      } // else if 
    } // if 
  } // ProcessBuffer

  void 
  EpsUart::
    DispatchPacket(
      EpsModule::EPS_CommandRecv command
    )
  {
    Fw::Buffer buf;
    switch (command)
    {
      case EpsModule::EPS_CommandRecv::HEARTBEAT_ACK:
        buf = Fw::Buffer(heartbeat_ack_pkt, 7);
        this->UartWrite_out(0, buf);
        break;
      case EpsModule::EPS_CommandRecv::HEARTBEAT_PKT:
        buf = Fw::Buffer(heartbeat_pkt, 6);
        this->UartWrite_out(0, buf);
        break;
      case EpsModule::EPS_CommandRecv::CRUISE_PKT:
        buf = Fw::Buffer(cruise_pkt, 7);
        this->UartWrite_out(0, buf);
        break;
      case EpsModule::EPS_CommandRecv::IDLE_PKT:
        buf = Fw::Buffer(idle_pkt, 7);
        this->UartWrite_out(0, buf);
        break;
      case EpsModule::EPS_CommandRecv::RESET_PKT:
        buf = Fw::Buffer(reset_pkt, 7);
        this->UartWrite_out(0, buf);
        break;
      default:
        break;
    } // switch
  } // constructPacket
} // namespace EpsModule
