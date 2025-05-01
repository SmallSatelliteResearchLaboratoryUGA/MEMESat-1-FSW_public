# State Machine Type Definitions

module StateMachineModule {
    
    @ State Machine States
    enum SM_State {
        ANON             @< anonymous state /// may potentially remove
        START_UP         @< Start Up State
        CRUISE           @< Cruise State
        SAFE_ANOM        @< Safe Anomaly State
        SAFE_CRIT_PWR    @< Critical Power State
        RESTART          @< Restart State
        SHUTDOWN         @< Shutdown State
        IDLE @< Idle State (TEST)
        RESET @< Reset State (TEST)
    }

    @ Cruise Mode
    enum SM_CruiseMode {
        NOMINAL          @< Nominal Cruise Mode
        SAFE             @< Safe Cruise Mode
        MEME             @< MEME Cruise Mode 
        BBS              @< BBS Cruise Mode
    }

    @ Eps Commands
    #! Note
    #! The command values are arbitrary and are not defined in the StateMachine
    #! documentation. 
    #! Please consider adapting these in the future
    enum SM_EpsCommand {
        SND_CMD_1 = 0x00 @< Model command 1
        SND_CMD_2 = 0x01 @< Model command 2
        SND_CMD_3 = 0x02 @< Model command 3 .. etc.
    }

    struct SM_ReadyForPowerOff {
        ready: bool @< Ready for power off
    }

    
}