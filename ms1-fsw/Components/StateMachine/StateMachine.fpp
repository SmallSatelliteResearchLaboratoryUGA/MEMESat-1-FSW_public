module StateMachineModule { 
    @ Responsible for reporting the state of the system 
    active component StateMachine { 

        @ Variables 
        constant initNumConnections = 2

        # --- 
        # General ports
        # ---

        @ Port receiving calls from the rate group 
        async input port Run: Svc.Sched
  
        @ Port for manually changing the state of the StateMachine
        async input port SM_ChangeState: SM_ChangeState

        @ Port for declaring when the StateMachine should shutdown
        output port SM_ReadyForPowerOff: [initNumConnections] SM_ReadyForPowerOff_port 

        @ Port to send a command to the eps 
        output port SM_EpsCommand: EpsModule.EPS_CommandRecvPort

        @ Port to send a command to the radio component 
        output port SM_RadioCommand: SM_RadioCommand_port 

        @ Port to receive a command from the eps of a state change 
        async input port SM_EpsCommandIn: EpsModule.EPS_CommandRecvPort
        
        # --- 
        # Special ports
        # ---

        @ Command receive port 
        command recv port cmdIn

        @ Command registration port
        command reg port cmdRegOut

        @ Command response port
        command resp port cmdResponseOut

        @ Event port
        event port eventOut

        @ Telemetry port   
        telemetry port tlmOut

        @ Text event port
        text event port textEventOut

        @ Time get port
        time get port timeGetOut

        # --- 
        # Commands 
        # ---

        @ Command to change state of the StateMachine
        @ read: https://github.com/nasa/fprime/releases/tag/v3.5.0
        @ section: FPP Changes
        async command SM_ChangeState(
            $state: SM_State
        )

        @ Command from Ground State to set the cruise mode 
        async command SM_SetCruiseMode(
            mode: SM_CruiseMode @< Cruise mode to set to from the Ground Station
        )

        @ Command to send a command to the EPS
        async command SM_EpsCommandOut(
            $command: SM_EpsCommand
        )

        # --- 
        # Events
        # ---

        @ StateMachine state change event
        event SM_State_Change(
            $state: SM_State
        ) \
        severity activity high \
        id 0 \
        format "Changing to state {}"

        @ Shutdown event 
        event SM_Shutdown(
        ) \
        severity activity high \
        id 1 \
        format "StateMachine shutdown event triggered"

        event SM_Restart(
        ) \
        severity activity high \
        id 2 \
        format "StateMachine restart event triggered"

        event SM_Persistent_Data_Error(
        ) \
        severity warning low \ 
        id 3 \
        format "Error reading persistent data"

        event SM_Invalid_State_Change(
            $state: SM_State
        ) \
        severity warning high \
        id 4 \
        format "Invalid: Cannot change to state {}"

        event SM_Cruise_Mode_Change(
            cruiseMode: SM_CruiseMode
        ) \
        severity activity high \
        id 5 \
        format "Cruise mode changed to {}"

        #! May remove later 
        event SM_Debug_Event(
            message: string
        ) \
        severity activity high \
        id 6 \
        format "debug: {}"

        # ---
        # Telemetry
        # ---

        @ Current state of the StateMachine
        telemetry SM_State: SM_State \
            id 0 \
            update on change \
            format "StateMachine state: {}"

        @ Last state of the StateMachine
        telemetry SM_LastState: SM_State \
            id 1 \
            update on change \
            format "StateMachine last state: {}"

        @ Number of restarts
        telemetry SM_RestartCount: U32 \
            id 2 \
            update on change \
            format "StateMachine restart count: {}"

        @ Number of shutdowns
        telemetry SM_ShutdownCount: U32 \
            id 3 \
            update on change \
            format "StateMachine shutdown count: {}"

        @ The cruise mode 
        telemetry SM_CruiseMode: SM_CruiseMode \
            id 4 \
            update on change \
            format "Cruise mode: {}"
    }
}