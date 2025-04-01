module EpsModule { 

    port EPS_ChangeState(
        $state: EPS_State
    )   

    port RequestState(
        Request: bool
    )

    #! Note 
    #! Please consider changing this in the future. 
    #! This was done during development debugging to allow the 
    #! state machine to send discrete U8 size values to the EPS. 
    #! Its previous implementation was made with respect to sending Fw::Buffer 
    #! objects to the EPS. I was unsure of this implementation as cross component 
    #! communications may not need to include packetized data. 
    port EPS_CommandRecvPort(
        $command: EPS_CommandRecv
    )


}