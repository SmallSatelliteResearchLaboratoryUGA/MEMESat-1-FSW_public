// ======================================================================
// \title  StateMachine.cpp
// \author lunar lizard
// \brief  cpp file for StateMachine component 
// ======================================================================

#include "Components/StateMachine/StateMachine.hpp"
#include "FpConfig.hpp"

namespace 
StateMachineModule {

  // ----------------------------------------------------------------------
  // Component construction and destruction
  // ----------------------------------------------------------------------

  StateMachine ::
    StateMachine(const char* const compName) :
      StateMachineComponentBase(compName)
  {
    // DEBUG //
    //this->log_ACTIVITY_HI_SM_Debug_Event(Fw::String("StateMachine"));
    Os::File file; 
    Os::File::Status status; 
    /* try to open 'persistent_data.txt' */
    if ((status = file.open("persistent_data.txt", Os::File::OPEN_READ)
        ) == Os::File::Status::OP_OK) 
    {
      // DEBUG //
      this->log_ACTIVITY_HI_SM_Debug_Event(Fw::String("File opened"));
      FwSignedSizeType size = 256;
      U8* buffer = new U8[size];
      /* try to read from file */
      if ((status = file.read(buffer, size)) == Os::File::Status::OP_OK)
      {
        std::string line, param;
        double value;
        for (int i = 0; i < size; i++)
        {
          if (buffer[i] == '\n')
          {
            param = line.substr(0, line.find(':')); // seek line to first colon
            value = std::stod(line.substr(line.find(':') + 1)); // convert string to double
            persistentData[param] = value; // add to map
            line = "";  // clear line
          }
          else 
            line += buffer[i]; // add to line
        }
        file.close(); // close file
      } 
      /* if file was not read properly ; log err*/
      else 
        this->log_WARNING_LO_SM_Persistent_Data_Error();
      file.close(); // close file
    }
    /* if file was not opened ; log err */
    else 
      this->log_WARNING_LO_SM_Persistent_Data_Error();
  } // StateMachine

  void 
  StateMachine ::
    init(
      const NATIVE_INT_TYPE queueDepth,
      const NATIVE_INT_TYPE instance
    )
  {
    StateMachineComponentBase::init(queueDepth, instance);
  } // init 

  StateMachine ::
    ~StateMachine()
  { }

  // ----------------------------------------------------------------------
  // Handler implementations for user-defined typed input ports
  // ----------------------------------------------------------------------

  void 
  StateMachine ::
    Run_handler(
      FwIndexType portNum,
      U32 context
    )
  {
    this->Update(); 
    this->SendState(this->currentState);
    /* if heartbeat flag is true and ack counter is permitting */
    if (heartbeat) 
    {
      this->SM_EpsCommand_out(0, EpsModule::EPS_CommandRecv::HEARTBEAT_PKT); // send command to eps
    } // if 
  } // Run_handler

  void 
  StateMachine ::
    SM_ChangeState_handler(
        FwIndexType portNum,
        const StateMachineModule::SM_State& state
    )
  {
    // DEBUG //
    //this->log_ACTIVITY_HI_SM_Debug_Event(Fw::String("SM_ChangeState_handler"));
    /* check if state provided is not cruise mode */
    if (state != SM_State::CRUISE)
    {
      this->nextState = state; // set next state to the state provided
      this->log_ACTIVITY_HI_SM_State_Change(state); // log state change
    }
    /* if state provided is cruise mode 
     * note: 
     * if the change state handler function provides cruise mode, 
     * this means that some other component has requested a state change 
     * (to cruise mode). This seems like it should not be allowed as we 
     * should (probably) only be able to change to cruise mode from the 
     * ground station.
     */
    else 
      this->log_WARNING_HI_SM_Invalid_State_Change(state); // log warning
  } // SM_ChangeState_handler

  void
  StateMachine ::
    SM_EpsCommandIn_handler(
      FwIndexType portNum,
      const EpsModule::EPS_CommandRecv& command
    )
  {
    this->log_ACTIVITY_HI_SM_Debug_Event(Fw::String("SM_EpsCommandIn_handler"));
    // DEBUG //
    switch (command) 
    { 
      case EpsModule::EPS_CommandRecv::HEARTBEAT_PKT:
        this->log_ACTIVITY_HI_SM_Debug_Event(Fw::String("EPS -> HEARTBEAT"));
        /* set heartbeat flag to true */
        heartbeat = true; 
        /* start heartbeat ack counter */
        break;
      default: 
        break; 
    } // switch
  } // SM_EpsCommandIn_handler

  // ----------------------------------------------------------------------
  // Handler implementations for commands
  // ----------------------------------------------------------------------

  void 
  StateMachine ::
    SM_ChangeState_cmdHandler(
        FwOpcodeType opCode,
        U32 cmdSeq,
        StateMachineModule::SM_State state
    )
  {
    // DEBUG //
    //this->log_ACTIVITY_HI_SM_Debug_Event(Fw::String("SM_ChangeState_cmdHandler"));
    this->log_ACTIVITY_HI_SM_State_Change(state); // log state change
    this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK); // send command response
    //! TESTING 
    if (state == SM_State::CRUISE)
    {
      this->nextState = state;
      this->log_ACTIVITY_HI_SM_State_Change(state); // log state change
      this->SM_EpsCommand_out(0, EpsModule::EPS_CommandRecv::CRUISE_PKT);
    }
    else if (state == SM_State::IDLE)
    {
      this->nextState = state;
      this->log_ACTIVITY_HI_SM_State_Change(state); // log state change
      this->SM_EpsCommand_out(0, EpsModule::EPS_CommandRecv::IDLE_PKT);
    }
    else if (state == SM_State::RESET)
    {
      this->nextState = state;
      this->log_ACTIVITY_HI_SM_State_Change(state); // log state change
      this->SM_EpsCommand_out(0, EpsModule::EPS_CommandRecv::RESET_PKT);
    }
    //! END TESTING 
  } // SM_ChangeState_cmdHandler

  void 
  StateMachine ::
    SM_SetCruiseMode_cmdHandler(
        FwOpcodeType opCode,
        U32 cmdSeq,
        StateMachineModule::SM_CruiseMode mode
    )
  {
    // DEBUG // 
    //this->log_ACTIVITY_HI_SM_Debug_Event(Fw::String("SM_SetCruiseMode_cmdHandler"));
    this->log_ACTIVITY_HI_SM_Cruise_Mode_Change(this->cruiseMode); // log cruise mode change
    this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK); // send command response
    this->SM_EpsCommand_out(0, EpsModule::EPS_CommandRecv::CRUISE_PKT); // send command to eps
  } // SM_SetCruiseMode_cmdHandler

  void 
  StateMachine ::
    SM_EpsCommandOut_cmdHandler(
      FwOpcodeType opCode, U32 cmdSeq,
      StateMachineModule::SM_EpsCommand command
    ) 
  {
    // DEBUG // 
    //this->log_ACTIVITY_HI_SM_Debug_Event(Fw::String("SM_EpsCommandOut_cmdHandler"));
    // DEBUG // 
    switch(command) /* switch on command */
    {
      default: 
        this->log_ACTIVITY_HI_SM_Debug_Event(Fw::String("UNKNOWN_CMD -> EPS"));
        break;
    } // switch   
    this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
  } // SM_EpsCommandOut_cmdHandler

  // ----------------------------------------------------------------------
  // Private helper functions
  // ----------------------------------------------------------------------

  void 
  StateMachine ::
    setState(StateMachineModule::SM_State state)
  {
    // DEBUG // 
    //this->log_ACTIVITY_HI_SM_Debug_Event(Fw::String("setState"));
    // TODO
  }

  StateMachineModule::SM_State 
  StateMachine ::
    getState(void)
  {
    // DEBUG //
    //this->log_ACTIVITY_HI_SM_Debug_Event(Fw::String("getState"));
    // TODO
    //return StateMachineModule::SM_State::SM_STATE_OFF;
    return StateMachineModule::SM_State::ANON;
  } // getState

  void 
  StateMachine ::
    setError(int error)
  {
    // DEBUG //
    //this->log_ACTIVITY_HI_SM_Debug_Event(Fw::String("setError"));
    // TODO
  } // setError

  int 
  StateMachine ::
    getError(void)
  {
    // DEBUG //
    //this->log_ACTIVITY_HI_SM_Debug_Event(Fw::String("getError"));
    // TODO
    return 0;
  } // getError

  void 
  StateMachine ::
    CheckState(StateMachineModule::SM_State state)
  {
    // DEBUG //
    //this->log_ACTIVITY_HI_SM_Debug_Event(Fw::String("CheckState"));
    /* check if 
     * current state parameter is set to safe critical power 
     * and the state provided is cruise mode
     * note: 
     * this should not be the case as a state change from safe
     * critical power to cruise mode may cause complications. 
     * safe critical power is a state that is entered when the
     * eps has detected an issue with the power system and has
     * relayed this information to the obc.
     */
    if ((this->currentState == SM_State::SAFE_CRIT_PWR) && 
        (state == SM_State::CRUISE))
      this->log_WARNING_HI_SM_Invalid_State_Change(state); // log warning
    /* check if the current state.e is not the state.e of the provided state 
     * and that the state.e provided is not 'start_up'
     */
    else if ((this->currentState.e != state.e) && 
             (state.e != SM_State::START_UP))
    {
      this->currentState = this->nextState; // set current state to next state
      this->tlmWrite_SM_State(this->currentState); // write to telemetry 
    } // else if
  } // CheckState

  void 
  StateMachine ::
    SendState(StateMachineModule::SM_State state)
  {
    // DEBUG //
    //this->log_ACTIVITY_HI_SM_Debug_Event(Fw::String("SendState"));
    U8 msg[4] = {
      0x04, // type  
      0x0E, // length
      0x01, // ack
      0x00, // state
    };
    /* set 4th byte to state provided */
    switch(state.e)
    {
      case SM_State::CRUISE: 
        msg[3] = 0x02; 
        break;
      case SM_State::SAFE_ANOM:
        msg[3] = 0x01;
        break;
      case SM_State::SAFE_CRIT_PWR: 
        msg[3] = 0x05;
      case SM_State::START_UP:
        msg[3] = 0x00;
        break;
      case SM_State::RESTART:
        msg[3] = 0x05;
        break;
      case SM_State::SHUTDOWN:
        msg[3] = 0x05;
        break;
      default:  
        msg[3] = 0x01;
        break;
    }
    Fw::Buffer buffer(msg, 4); 
    // this->SM_EpsCommand_out(0, buffer); // send state to eps
  } // SendState

  void 
  StateMachine ::
    UpdateCruise(void)
  {
    // DEBUG //
    //this->log_ACTIVITY_HI_SM_Debug_Event(Fw::String("UpdateCruise"));
    // need to allow people other than us to uplink 
    // this will have to be done in the radio component 
    // make sure the radio baud is 9600
    this->tlmWrite_SM_CruiseMode(this->cruiseMode); // write to telemetry the cruise mode state 
    // if (this->cruiseMode == SM_CruiseMode::MEME)
    //   this->SM_RadioCommand_out(0, this->currentState, MEME_BAUD); // send state to radio port 
    // else if (this->cruiseMode == SM_CruiseMode::BBS)
    //   this->SM_RadioCommand_out(0, this->currentState, BBS_BAUD); // send state to radio port
  } // UpdateCruise

  void 
  StateMachine ::
    UpdateAnom(void)
  {
    // DEBUG //
    //this->log_ACTIVITY_HI_SM_Debug_Event(Fw::String("UpdateAnom"));
    // restrict communication to ssrl ground state - not sure how to do this 
    // no more bbs comm until move back to cruise mode - same as above 
    // baud rate is set to 9600 why are we changing the baud again instead of keeping it high 
    // this->SM_RadioCommand_out(0, this->currentState, 9600); // send state to radio port
  } // UpdateAnom

  void 
  StateMachine ::
    UpdateCritPower()
  {
    // DEBUG //
    //this->log_ACTIVITY_HI_SM_Debug_Event(Fw::String("UpdateCritPower"));
    this->Shutdown(); // trigger system shutdown 
  } // UpdateCritPower

  void 
  StateMachine ::
    Update()
  {
    // DEBUG //
    //this->log_ACTIVITY_HI_SM_Debug_Event(Fw::String("Update"));
    this->CheckState(this->nextState); // check the state
    switch(this->currentState.e)
    {
      case SM_State::CRUISE: 
        this->UpdateCruise(); // update cruise mode
        break;
      case SM_State::SAFE_ANOM:
        this->UpdateAnom(); // update safe anomalous mode
        break;
      case SM_State::SAFE_CRIT_PWR:
        this->UpdateCritPower(); // update safe critical power mode
        break;
      case SM_State::START_UP:
        this->nextState = SM_State::SAFE_ANOM; // set next state to safe anomalous mode
        break;
      case SM_State::RESTART:
        this->Restart(); // restart the system
        break;
      case SM_State::SHUTDOWN:
        this->Shutdown(); // shutdown the system
        break;
      default:
        this->nextState = SM_State::SAFE_ANOM; // set next state to safe anomalous mode
        this->UpdateAnom();
        break;
    } // switch
  } // Update

  void 
  StateMachine ::
    ReadyForPowerOff()
  {
    // DEBUG //
    //this->log_ACTIVITY_HI_SM_Debug_Event(Fw::String("ReadyForPowerOff"));
    /* save persistent data to the system */
    Os::File file; 
    Os::File::Status status;
    /* try to open 'persistent_data.txt' */
    if ((status == file.open("persistent_data.txt", Os::File::OPEN_WRITE))
         == Os::File::Status::OP_OK)
    {
      for (auto const &x : this->persistentData)
      {
        std::string line = x.first + ":" + std::to_string(x.second) + "\n";
        FwSignedSizeType size = line.length();
        if ((status == file.write((U8*)line.c_str(), size, Os::File::WaitType::WAIT))
            != Os::File::Status::OP_OK)
          this->log_WARNING_LO_SM_Persistent_Data_Error(); // log warning
      }
      file.close(); // close file
    } // if
    /* if opening the file does not occur */
    else 
      this->log_WARNING_LO_SM_Persistent_Data_Error(); // log warning
    // send shutdown command to file recycler port
    this->SM_ReadyForPowerOff_out(0, true);
    // send shutdown command to eps 
    U8 msg[4] = {
      0x04, // message type - set value 
      0x0E, // message length - 14 ? 
      0x01, // ack received 
      0x05, // state to switch the mcu to - STATE_STM_CTL
    };
    Fw::Buffer shutdownMsg(msg, 4);
    // this->SM_EpsCommand_out(0, shutdownMsg); // send shutdown message to eps port
    //this->SM_EpsCommand_out(0, EpsModule::EPS_CommandRecv::RECV_CMD_1); // send command to eps EXAMPLE
  } // ReadyForPowerOff

  void 
  StateMachine ::
    Restart(void)
  {
    // DEBUG //
    //this->log_ACTIVITY_HI_SM_Debug_Event(Fw::String("Restart"));
    this->persistentData["restarts"]++; // increment restarts
    this->ReadyForPowerOff(); // prepare to power off
    this->tlmWrite_SM_RestartCount(this->persistentData["restarts"]);
    this->log_ACTIVITY_HI_SM_Restart();
    sync(); // sync
    reboot(RB_AUTOBOOT); // reboot sys call
  } // Restart

  void 
  StateMachine ::
    Shutdown(void)
  {
    // DEBUG //
    //this->log_ACTIVITY_HI_SM_Debug_Event(Fw::String("Shutdown"));
    this->ReadyForPowerOff(); // prepare to power off
    this->persistentData["shutdowns"]++; // increment shutdowns
    this->tlmWrite_SM_ShutdownCount(this->persistentData["shutdowns"]);
    this->log_ACTIVITY_HI_SM_Shutdown();
    sync();
    reboot(RB_POWER_OFF);
  } // Shutdown
} // namespace StateMachineModule
