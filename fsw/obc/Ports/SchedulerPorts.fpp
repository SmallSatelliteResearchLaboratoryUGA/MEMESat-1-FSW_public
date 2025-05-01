module SchedulerModule { 

    @ Port for sending a schedule
    port SendSchedulePort(
        name: string @< Name of schedule for libcron task
        schedule: string @< Schedule in libcron format
        action: ScheduleOp
    )

    @ Port for running a schedule
    port RunSchedulePort(
        status: ScheduleStatus
    )


}