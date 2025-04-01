#include "include/stateMachine.h"   
#include "include/obcComm.h"

enum State state;
enum State prevState;
uint32_t errors;
bool beat = true; 

void
initStateMachine()
{
    prevState = -1;
    state = SOV;
    k_timer_init(
        &heartbeat_timer,  // timer
        heartbeat_cb,      // callback
        NULL               // data
    );
    k_timer_init(
        &obc_quiesc_timer, // timer
        quiesc_expire_cb,  // callback
        NULL               // data
    );
    k_timer_start(
        &heartbeat_timer,  // timer
        BEAT_INTERVAL,     // duration
        K_NO_WAIT          // options
    );
    k_timer_start(
        &obc_quiesc_timer, // timer 
        K_SECONDS(1),      // duration
        K_NO_WAIT          // options
    );
} // initStateMachine

void 
stateMachineService() 
{
    initStateMachine();
    while(1)
    { 
        /* debug print state */
        printk("DEBUG -- state: %d\n", state);
        /* switch on state */
        switch(state)
        { 
            case SOV:
                /* ensure heartbeat emission */
                /* check if heartbeat timer has expired */
                if (k_timer_status_get(&heartbeat_timer) > 0 && beat)
                {
                    k_timer_start(&heartbeat_timer, K_SECONDS(1), K_NO_WAIT);
                } // if
                break;
            case IDL:
                break;
            case CRU:
                break;
            case CPM:
                break;
            case AUT:
                break;
        }
        /* allow other threads to do work */
        k_sleep(K_MSEC(1000));
    }
} // stateMachineService

/**
 * @brief Change the state of the state machine
 */
uint8_t 
changeState(uint8_t target_state)
{
    /* grab semaphore to change data */
    k_sem_take(&state_sem, K_NO_WAIT);
    // do stuff
    switch(target_state)
    {
        case 0x00: /* Sovreign */
            prevState = state;
            state = SOV;
            beat = true; 
            k_timer_start(&heartbeat_timer, BEAT_INTERVAL, K_NO_WAIT);  
            break;
        case 0x01: /* Idle */
            prevState = state;
            state = IDL;
            beat = false; 
            k_timer_start(&obc_quiesc_timer, K_SECONDS(5), K_NO_WAIT);
            break;
        case 0x02: /* Cruise */
            prevState = state;
            state = CRU;
            beat = false;
            k_timer_start(&obc_quiesc_timer, K_SECONDS(5), K_NO_WAIT);
            break;
        case 0x03: /* Critical Power Mode */
            prevState = state;
            state = CPM;
            break;
        case 0x04: /* Autonomous */
            prevState = state;
            state = AUT;
            break;
        default: 
            return 1;
    }
    /* release semaphore */
    k_sem_give(&state_sem);
}

uint8_t
getState()
{
    return state;
}

void
quiesc_expire_cb(struct k_timer *obc_quiesc_timer)
{
    printk("DEBUG -- quiesc_expire_cb\n");
    /* change state back to sovreign */
    changeState(SOV);
} // quiesc_expire_cb