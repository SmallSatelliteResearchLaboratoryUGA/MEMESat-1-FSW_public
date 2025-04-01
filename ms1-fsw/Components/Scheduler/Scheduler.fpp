module SchedulerModule {

    @ Component for scheduling tasks
    active component Scheduler { 

        @ Variables 
        constant initNumConnections = 3
        array schedulerStatusArray = [initNumConnections] ScheduleStatus
        array schedulerScheduleArray = [initNumConnections] string
        array schedulerNameArray = [initNumConnections] string

        # --- 
        # General ports
        # ---

        @ Port for receiving the operation request 
        output port downlinkCurrentSchedules: Svc.SendFileRequest

        @ Port for running the schedule
        output port runSchedule: [initNumConnections] RunSchedulePort

        @ Port for getting the schedule
        async input port getSchedule: [initNumConnections] SendSchedulePort \
            priority 10 \ 
            block 
            match getSchedule with runSchedule 

        @ The rate group scheduler input
        sync input port tick: Svc.Sched

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

        @ Downlinks file containing a list of the current schedules 
        async command SCH_GetScheduleList (
            destFileName: string size 100 @< Path to store downlinked schedule list at
        )

        @ Stops the schedule
        sync command SCH_StopSchedule (
            name: string size 15 @< Name of schedule to stop
        )

        @ Creates a schedule (TESTING) 
        sync command SCH_CreateSchedule (
            name: string size 30 @< Name of schedule to create
            arg_format: string size 40 @< Format of the schedule
            capture_param: string size 30 @< Parameters for the schedule
            lambda_params: string size 30 @< Parameters for the schedule
            lambda_func: string size 512 @< Function to run
        )


        # ---
        # Events
        # ---

        @ Event for getting the schedule list (TODO)
        event SCH_ScheduleListGet (
            ) \
            severity diagnostic \
            format "getting schedule list..."

        @ Event to denote that the schedule port number was invoked incorrectly
        event SCH_PortNumGetScheduleWarn (
            portNum: U8
            ) \
            severity warning low \
            format "Port number {} passed to getSchedule_handler is incorrect. Invoker schedule unchanged."

        @ Event to log warning that the name string is empty
        event SCH_NameStringEmptyWarn (
            portNum: U8
            ) \ 
            severity warning low \
            format "Name string passed to port {} for getSchedule is empty"

        @ Event to log warning if the schedule is incorrect 
        event SCH_ScheduleIncorrectWarn (
            schedule: string
            portNum: U8
            name: string 
            status: ScheduleStatus
            ) \
            severity warning low \
            format "Schedule {} passed to port {} ({}) for getSchedule is formatted incorrectly. Schedule is {}"

        @ Event to log if a task is running 
        event SCH_TaskRunning (
            name: string
            status: ScheduleStatus
            ) \
            severity activity low \
            format "The status of schedule {} is {}"

        @ Event to log that the schedule was downlinked
        event SCH_ScheduleDownlinked (
            destination_file: string
            ) \
            severity diagnostic \
            format "The list of schedules was downlinked to destination file {}"


        @ Event to log the command that was sent to the scheduler (TESTING) 
        event SCH_CommandSent (
            name: string size 30 @< Name of the command
            cmd_string: string size 100 @< Command sent to the scheduler
        ) \
            severity warning low \
            format "Command {} sent to scheduler with name {}"

        @ Event to log that the schedule was processed and is correct (TESTING) 
        event SCH_ScheduleProcessed (
            name: string size 30 @< Name of the schedule
            schedule: string size 100 @< Schedule that was processed
        ) \
            severity warning low \
            format "Schedule {} processed for schedule {}"

        # --- 
        # Telemetry 
        # --- 

        @ Status of the fileRecycler schedule 
        telemetry SCH_FileRecyclerPortStatus: ScheduleStatus \
            id 0 \ 
            update on change 

        @ Status of the tlmChan schedule 
        telemetry SCH_TlmChanPortStatus: ScheduleStatus \
            id 1 \
            update on change

        @ Status of the otaUpdater schedule
        telemetry SCH_OtaUpdaterPortStatus: ScheduleStatus \
            id 2 \
            update on change

    }

}