// ======================================================================
// \title  StateMachine.hpp
// \author lunar lizard
// \brief  hpp file for StateMachine component
// ======================================================================

#ifndef StateMachineModule_StateMachine_HPP
#define StateMachineModule_StateMachine_HPP

#include "Components/StateMachine/StateMachineComponentAc.hpp"
#include "FpConfig.hpp"
#include "Fw/Buffer/Buffer.hpp"
#include "Fw/Types/BasicTypes.hpp"
#include "Fw/Types/Assert.hpp"
#include "Os/File.hpp"
#include <atomic> 
#include <chrono> // added for debug
#include <iostream> 
#include <map>
#include <string> 
#include <sys/reboot.h>
#include <thread>
#include <unistd.h>

//! may not need 
//#include <linux/reboot.h>

namespace StateMachineModule {

  class StateMachine :
    public StateMachineComponentBase
  {

    public:

      // ----------------------------------------------------------------------
      // Component construction and destruction
      // ----------------------------------------------------------------------

      //! Construct StateMachine object
      StateMachine(
          const char* const compName //!< The component name
      );

      //! Initialize object StateMachine 
      void init(
        const NATIVE_INT_TYPE queue_depth, /*!< The queue depth */
        const NATIVE_INT_TYPE instance = 0 /*!< The instance number */
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
      void Run_handler(
          FwIndexType portNum, //!< The port number
          U32 context //!< The call order
      ) override;

      //! Handler implementation for SM_ChangeState
      //!
      //! Port for manually changing the state of the StateMachine
      void SM_ChangeState_handler(
          FwIndexType portNum, //!< The port number
          const StateMachineModule::SM_State& state //!< The state of the component
      ) override;

      //! Handler implementation for SM_EpsCommandIn
      //!
      //! Port to receive a command from the eps of a state change
      void SM_EpsCommandIn_handler(
          FwIndexType portNum, //!< The port number
          const EpsModule::EPS_CommandRecv& command //!< The command from the EPS
      ) override;
      

    PRIVATE:

      // ----------------------------------------------------------------------
      // Handler implementations for commands
      // ----------------------------------------------------------------------

      //! Handler implementation for command SM_ChangeState
      //!
      //! Command to change state of the StateMachine
      //! read: https://github.com/nasa/fprime/releases/tag/v3.5.0
      //! section: FPP Changes
      void SM_ChangeState_cmdHandler(
          FwOpcodeType opCode, //!< The opcode
          U32 cmdSeq, //!< The command sequence number
          StateMachineModule::SM_State state
      ) override;

      //! Handler implementation for command SM_SetCruiseMode
      //!
      //! Command from Ground State to set the cruise mode
      void SM_SetCruiseMode_cmdHandler(
          FwOpcodeType opCode, //!< The opcode
          U32 cmdSeq, //!< The command sequence number
          StateMachineModule::SM_CruiseMode mode //!< Cruise mode to set to from the Ground Station
      ) override;

        //! Handler implementation for command SM_EpsCommandOut
      //!
      //! Command to send a command to the EPS
      void SM_EpsCommandOut_cmdHandler(
          FwOpcodeType opCode, //!< The opcode
          U32 cmdSeq,          //!< The command sequence number
          StateMachineModule::SM_EpsCommand command
      ) override;

    PRIVATE: 

      // ----------------------------------------------------------------------
      // Private helper functions
      // ----------------------------------------------------------------------

      //! Function to set thes tate of the StateMachine 
      void setState(StateMachineModule::SM_State state);

      //! Function to get the state of the StateMachine
      StateMachineModule::SM_State getState();

      //! Function to set the error of the StateMachine
      //void setError(StateMachineModule::SM_Error error);
      void setError(int error);

      //! Function to get the error of the StateMachine
      //StateMachineModule::SM_Error getError();
      int getError();

      //! Check if the state transition is valid 
      void CheckState(StateMachineModule::SM_State state);

      //! Function to send the current state to the MCU
      void SendState(StateMachineModule::SM_State state);

      //! Update to cruise mode 
      void UpdateCruise();

      //! Update to safe mode
      void UpdateAnom();

      //! Update to critical sage
      void UpdateCritPower();

      //! Updates the state of the state machine
      void Update();

      //! Gets the system ready for power off 
      void ReadyForPowerOff(); 

      //! Restarts the system 
      void Restart(); 

      //! Shuts down the system 
      void Shutdown();

    PRIVATE: 
     
      // ----------------------------------------------------------------------
      // Private variables
      // ----------------------------------------------------------------------

      // Initialize the StateMachine State to be in START_UP
      StateMachineModule::SM_State currentState = SM_State::START_UP;
      StateMachineModule::SM_State nextState; // next state of the StateMachine
      // Initialize the StateMachine Cruise Mode to be in NOMINAL
      StateMachineModule::SM_CruiseMode cruiseMode = SM_CruiseMode::NOMINAL;
      // Map object for persistentData
      std::map<std::string, double> persistentData;
      // Iterator 
      //std::map<std::string, double>::iterator it; 
      const U32 MEME_BAUD = 40000;
      const U32 BBS_BAUD = 9600;
      bool command = false;
      int error = -1;
      Fw::String logMessage;

      bool heartbeat = false; 

  };

}

#endif
