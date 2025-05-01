#ifndef STATEMACHINE_H
#define STATEMACHINE_H

#include <zephyr/drivers/gpio.h>
#include <zephyr/kernel.h>

#define BURN_PIN 24
#define BEAT_INTERVAL K_MSEC(1000)

extern struct k_timer heartbeat_timer;

/*
enum State
{
    INIT = 0x00,
    IDLE = 0x01,
    RUN = 0x02,
    ERR = 0x03,
    LOPWR = 0x04,
    CTRL = 0x05,
};
*/

enum State 
{
    SOV = 0x00, 
    IDL = 0x01, 
    CRU = 0x02, 
    CPM = 0x03, 
    AUT = 0x04
}; 

// define state variables
extern enum State state;
extern enum State prevState;

// define semaphore
extern struct k_sem state_sem;
extern struct k_sem error_sem;

// define timers
extern struct k_timer startup;
extern struct k_timer burn;

void initStateMachine(void); 
void stateMachineService(void); 
uint8_t changeState(uint8_t target_state);
uint8_t getState(void);
void quiesc_expire_cb(struct k_timer *obc_quiesc_timer);

#endif // STATEMACHINE_H