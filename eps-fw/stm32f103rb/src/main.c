#include <stdio.h>
#include <string.h>

#include <zephyr/device.h>
#include <zephyr/drivers/i2c.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/uart.h>
#include <zephyr/kernel.h>

#include "include/stateMachine.h"
#include "include/obcComm.h"
#include "include/sensors.h"

struct k_timer heartbeat_timer;
struct k_timer obc_quiesc_timer; 

K_THREAD_DEFINE(
    obc_comm_thread_id, // thread id
    1024,               // stack size
    obcCommService,     // entry point
    NULL,               // parameter 1
    NULL,               // parameter 2
    NULL,               // parameter 3
    7,                  // priority
    K_ESSENTIAL,        // options
    0                   // delay
);


K_THREAD_DEFINE(
    state_machine_id, 
    1024, 
    stateMachineService, 
    NULL, 
    NULL, 
    NULL, 
    7, 
    K_ESSENTIAL, 
    0
);

/*
K_THREAD_DEFINE(
    sensor_thread_id,
    1024,
    sensorService,
    NULL,
    NULL,
    NULL,
    7,
    K_FP_REGS,
    0
);
*/


K_SEM_DEFINE(
    obc_comm_sem, 
    0, 
    1
)

K_SEM_DEFINE(
    state_sem, 
    0, 
    1
)

K_SEM_DEFINE(
    sensor_sem, 
    0, 
    1
)

K_MUTEX_DEFINE(lock);

int 
main()
{
    k_thread_start(obc_comm_thread_id);
    k_thread_start(state_machine_id);
    // k_thread_start(sensor_thread_id);
    return 0;
}