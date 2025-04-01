module UpdaterModule {

    @ Responsible for updating the software of the system. 
    active component OTAUpdater { 

        # --- 
        # Types
        # ---

        enum StmBootOutNum {
            BOOT_0 = 0
            BOOT_1 = 1
        }

        enum StmRstState {
            LOW = 0
            HIGH = 1
        }

        # ---
        # General ports
        # ---

        @ Port receiving the operation request
        async input port updaterOpIn: UpdaterModule.UpdateOpPort

        @ Output port to writing GPIO values
        output port gpioWrite: [2] Drv.GpioWrite

        @ Port for sending the schedule
        output port sendSchedule: SchedulerModule.SendSchedulePort

        @ Port for handing the schedule
        async input port scheduledHandler: SchedulerModule.RunSchedulePort

        @ Port for sending a segment directory to be cleaned by the file recycler
        output port cleanupDataPath: FileRecyclerModule.CleanUpDataPort

        @ Port for receiving the power off status 
        sync input port readyForPowerOff: StateMachineModule.SM_ReadyForPowerOff_port

        # maybe have an input port which receives the status of the operation from the recycler? 

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
        time get port timeCaller

        @ Parameter get port 
        param get port paramGetOut

        @ Parameter set port
        param set port paramSetOut

        # --- 
        # Parameters 
        # --- 

        @ Schedule format 
        param OTA_Schedule: string default "0 * * * * ?" id 1 \
            set opcode 0x0A00 \
            save opcode 0x0A01

        @ Which boot pin to use
        #param StmBootOutNum bootOutNum: StmBootOutNum.BOOT_0
        
        @ Which state the reset pin should be in
        #param rstState: StmRstState.LOW

        @ The time to wait for the STM to boot
        #param stmBootTime: 2000 # ms 

        @ The time to wait for the STM to reset
        #param stmRstTime: 1000 # ms

        # --- 
        # Commands
        # ---

        @ Command to run an audit of the current files in scope 
        async command OTA_PerformAudit (
            dir_path: string # may provide an * to check all files.
            ) \
            opcode 0

        @ Command to perform a file operation on a group of files in scope
        async command OTA_PerformFileOperation (
            operation: U32 # Create custom message type ?  
            file_path: string
            ) \
            opcode 1

        @ Command to select the boot pin
        async command OTA_SelectBootPin (
            bootOutNum: U32 #StmBootOutNum
            ) \
            opcode 2

        @ Command to select the reset pin state 
        async command OTA_SelectRstState (
            rstState: U32 #StmRstState
            ) \
            opcode 3

        # --- 
        # Events
        # ---

        @ Event for when the audit reports a state 
        event OTA_AuditStatus (
            message: string
            ) \ 
            severity warning low \
            id 0 \ 
            format "Audit status: {}"
        
        @ Event for when the audit has been completed 
        event OTA_AuditComplete (
            message: string
            segments_present: U32
            segments_missing: U32
            objects_tracked: U32
            ) \
            severity warning low \
            id 1 \
            format "Audit Complete:: \nmsg, {}; segments_present, {}; segments_missing, {}; objects_tracked, {};"
        
        @ Event for when the audit has been interrupted
        event OTA_AuditInterrupted (
            message: string
            status: U32
            ) \
            severity warning low \
            id 2 \
            format "Audit Interrupted:: \nmsg, {}; status, {};"

        @ Event for when a file operation has been completed 
        event OTA_FileOperationComplete (
            message: string
            ) \
            severity warning low \
            id 3 \
            format "File operation complete:: \nmsg, {};"

        @ Event for when the boot pin has been selected
        event OTA_BootPinSelectTrigger (
            bootOutNum: StmBootOutNum
            ) \
            severity warning high \
            id 4 \
            format "Boot pin selected:: \nbootOutNum, {};"

        @ Event for when the reset pin state has been selected
        event OTA_RstStateChangeTrigger (
            rstState: StmRstState
            ) \
            severity warning high \
            id 5 \
            format "Reset state selected:: \nrstState, {};"

        @ Event for getting the stm reset pin state 
        event OTA_StmRstGetTrigger (
            value: StmRstState
            ) \
            severity warning high \
            id 6 \
            format "StmRstGetState read as {}"

        @ Invalid pin command requested
        event OTA_InvalidPinCommand (
            val: U32
            ) \
            severity warning high \
            id 7 \
            format "Invalid pin command {} requested"

        @ Schedule error 
        event OTA_ScheduleError (
            status: SchedulerModule.ScheduleStatus
            ) \
            severity warning low \
            id 8 \
            format "Schedule error:: \nstatus, {};"

        @ Schedule ran
        event OTA_ScheduleRan () \
            severity diagnostic \
            id 9 \
            format "Schedule ran"

        @ Schedule changed to
        event OTA_ScheduleChangedTo (
            schedule: string
            ) \
            severity diagnostic \
            id 10 \
            format "Schedule changed to {};"

        # --- 
        # Telemetry
        # ---

        @ Telemetry for the number of objects currently tracked 
        telemetry OTA_ObjectsTracked: U32 \
            id 0x0A02 \ 
            format "Objects tracked: {}"

        @ Telemetry for the number of files which have been built and do not have a predetermined target location 
        telemetry OTA_CompleteDormantFileCount: U32 \
            id 0x0A03 \
            format "Complete Dormant count: {}"

        @ Telemetry for the number of objects which have been adjusted
        telemetry OTA_ObjectAdjustmentCount: U32 \
            id 0x0A04 \
            format "Objects adjusted: {}"

        @ Telemetry for the number of segment directories which have been cleaned by the file recycler 
        telemetry OTA_SegmentCleanCount: U32 \
            id 0x0A05 \
            format "Segment clean count: {}"

    }


}