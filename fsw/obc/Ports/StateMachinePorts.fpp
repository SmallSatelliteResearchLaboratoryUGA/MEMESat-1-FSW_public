module StateMachineModule { 

    @ Port for confirming the change in state 
    port SM_ChangeState(
        $state: SM_State @< The state of the component 
    )

    port SM_StateConfirm(
        $state: SM_State @< The state of the component
    )

    port SM_ReadyForPowerOff_port(
        result: SM_ReadyForPowerOff @< The result of the power off request
    )

    #! Note 
    #! Please consider changing this in the future. 
    #! As noted in the EpsUartPorts definitions, the EPS_CommandRecvPort
    #! was implemented to allow the state machine to send discrete U8 size
    #! values to the EPS. 
    port SM_EpsCommandPort(
        $command: U8 @< The command to send to the EPS
    )

    port SM_RadioCommand_port(
        $state: SM_State @< The state of the component 
        baud: U32 @< The baud rate to set the radio to
    )

}