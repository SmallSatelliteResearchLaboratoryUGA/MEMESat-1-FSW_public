module FileRecyclerModule { 

    @ Component for the file recycler functions 
    active component FileRecycler { 

        # ---
        # General ports
        # ---

        @ Port for recieving the operation request
        async input port fileRecyclerOpIn: FileRecyclerOpPort

        @ Internal Port 
        output port toFileRecyclerHandler: FileRecyclerOpPort

        @ Port for receiving the power off status 
        sync input port readyForPowerOff: StateMachineModule.SM_ReadyForPowerOff_port

        @ Port for sending the schedule
        output port sendSchedule: SchedulerModule.SendSchedulePort 

        @ Port for handing the schedule
        async input port scheduledHandler: SchedulerModule.RunSchedulePort 

        @ Port for receiving the path to clean up 
        async input port cleanupDataPathRecv: FileRecyclerModule.CleanUpDataPort

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
        param get port prmGetOut

        @ Parameter set
        param set port prmSetOut

        # ---
        # Parameters
        # ---
        
        @ Maximum number of files
        param FR_MaxFiles: U32 default 1000 id 0 \
            set opcode 0x0A00 \
            save opcode 0x0A01

        @ Schedule format 
        param FR_Schedule: string default "0 * * * * ?" id 1 \
            set opcode 0x0A02 \
            save opcode 0x0A03  

        @ System reset 
        param FR_SystemReset: bool default false id 2 \
            set opcode 0x0A04 \
            save opcode 0x0A05

        @ Schedule mode
        param FR_ScheduleMode: FileRecyclerOp \
            id 3 \
            set opcode 0x0A06 \
            save opcode 0x0A07

        # --- 
        # Commands
        # ---

        @ Clean up the files
        sync command FR_CleanUpData (
            count: U32 @< file attribute that determines which files are deleted
            op: FileRecyclerOp
        ) 

        # ---
        # Events
        # ---

        @ File Recycler command received 
        event FR_CommandRecv (
            count: U32 @< file attribute that determines which files are deleted
            op: FileRecyclerOp
            ) \
            severity command \
            format "FileRecycler command received: {d} {}"

        @ Clean up data completed 
        event FR_CleanUpDataCompleted (
            status: Status @< returns the status of completion
            ) \
            severity diagnostic \
            id 1 \
            format "Status: {}"

        @ Schedule error
        event FR_ScheduleError (
            status: SchedulerModule.ScheduleStatus
            ) \
            severity warning low \ 
            id 2 \
            format "FileRecycler scheduledHandler failed with status {}"

        @ Schedule ran
        event FR_ScheduleRan () \
            severity diagnostic \
            id 3 \
            format "FileRecycler schedule ran"

        @ Schedule changed to
        event FR_ScheduleChangedTo (
            schedule: string
            ) \
            severity diagnostic \
            id 4 \
            format "Schedule changed to {}"

        @ Downlinks the name of removed file 
        event FR_FileRemove (
            year: U16
            month: string
            day: U8
            hour: U8
            minute: U8
            second: U8
            name: string
            status: string 
            message: string
            ) \
            severity diagnostic \ 
            id 5 \
            format "[{} {} {} {}:{}:{}]: File ({}) removed with status {} and status message {}"

        @ Num files updated 
        event FR_NumFilesUpdated (
            numFiles: U32
            ) \
            severity diagnostic \
            id 6 \
            format "Number of files updated to {}"

        @ Cannot open directory
        event FR_CannotOpenDir (
            name: string
            ) \
            severity warning high \
            id 7 \
            format "Failed opening directory {}"

        @ Recycler stopped by system reset
        event FR_RecyclerStoppedBySysReset () \
            severity warning low \
            id 8 \
            format "Recycler was stopped by being set for system reset"

        @ stat error 
        event FR_StatError ( 
            errorNumber: U32
            errorName: string
            message: string
            ) \
            severity warning low \
            id 9 \
            format "Stat object returned non-zero value {} ({}) on file read. Message: {}"

        @ Unexpected directory status 
        event FR_UnexpectedDirStatus (
            status: Status
            ) \
            severity warning low \
            id 10 \
            format "The OS::Directory::Status returned {} from reading the storage directory is different than expected."

        @ Delete event failed
        event FR_DeleteEventFailed (
            name: string 
            statTime: string 
            deleteStatus: Status
            op: FileRecyclerOp
            ) \
            severity warning low \
            id 11 \
            format "File {} with recorded creation time {} has a deletion status of {} from op {}"

        # ---
        # Telemetry
        # ---

        @ Operation 
        telemetry FR_Op: FileRecyclerOp \
            id 0

        @ Max number of files
        telemetry FR_MaxFiles: U32 \
            id 1 \
            update on change \
            format "Max number of files {}"

        @ Number of files removed
        telemetry FR_NumFilesRemoved: U32 \
            id 2 \
            format "Number of files: {}"

        @ Recycler Type 
        telemetry FR_RecyclerType: FileRecyclerOp \
            id 3 \
            format "FileRecycler ran with op {}"

        @ CleanUp Data completed
        telemetry FR_CleanUpDataCompleted: Status \
            id 4 \
            update on change \
            format "Clean up data completed: {}"

        @ Delete status
        telemetry FR_DeleteStatus: Status \
            id 5 \
            format "Deletion of file was {}"

    }

}