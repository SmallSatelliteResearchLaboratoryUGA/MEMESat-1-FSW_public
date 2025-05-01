#ifndef OBCCOMM_H
#define OBCCOMM_H

#include <zephyr/device.h>
#include <zephyr/drivers/uart.h>
#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <zephyr/sys/ring_buffer.h>
#include <zephyr/sys/util.h>
#include <string.h> 

#define UART_BUF_SIZE 64
#define RING_BUF_SIZE 512
#define MSG_SIZE 64
#define BEAT_INTERVAL K_MSEC(1000)

extern struct k_sem obc_comm_sem;
extern struct k_timer heartbeat_timer;
extern struct k_timer obc_quiesc_timer;

typedef struct {
    uint8_t *data; 
    long size; 
} MessagePacket; 

int obcCommService(void);
void initUart(void); 
void serial_obc_cb(const struct device *dev, void *user_data);
void parseData(uint8_t *data, long len);
void consumePacket(uint8_t *data, long size);
uint8_t* constructPacket(uint8_t *data, long size);
void heartbeat_cb(struct k_timer *heartbeat_timer);
uint16_t computeChecksum(uint8_t *data, uint16_t size);

#endif // OBCCOMM_H