module TlmChanModule { 

    @ Component to serve as a wrapper for telemetry channels
    active component TlmChanWrapper { 

        # --- 
        # General ports 
        # --- 
        
        @ Output port for telemetry channel
        output port tlmChanOut: Svc.Sched 

        @ Output port for sending schedule
        output port sendSchedule: SchedulerModule.SendSchedulePort

        @ Input port for scheduled handler
        async input port scheduledHandler: SchedulerModule.RunSchedulePort

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

        @ Parameter get
        param get port paramGet

        @ Parameter set
        param set port paramSet

        # ---
        # Parameters
        # ---
        
        @ Schedule parameter
        param TLMWR_Schedule: string default "0 * * * * ?" id 1 \
            set opcode 0x0002 \
            save opcode 0x0003

        # --- 
        # Events
        # ---

        @ Event for schedule error
        event TLMWR_ScheduleError(
            status: SchedulerModule.ScheduleStatus
        ) \ 
        severity warning low \ 
        id 0 \
        format "Schedule error: {}"


        @ Event for the schedule being updated to 
        event TLMWR_ScheduleChangedTo(
            schedule: string
        ) \
        severity activity low \
        id 1 \
        format "Schedule changed to {}"

        # ---
        # Telemetry 
        # ---

        @ Telemetry for the status of the last schedule change operation 
        @ performed on the TlmChanWrapper
        telemetry TLMWR_StatusOfSchedule: SchedulerModule.ScheduleStatus \
            id 0x10 \
            format "{}"



    }

}