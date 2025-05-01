#include "obcComm.h"
#include "stateMachine.h"

RING_BUF_DECLARE(in_buf, RING_BUF_SIZE); 
K_MSGQ_DEFINE(obc_comm_msgq, sizeof(MessagePacket), 10, 4);

const struct device *usart1 = DEVICE_DT_GET(DT_NODELABEL(usart1));
uint8_t msg_type, data_len, pkt_msb, pkt_lsb;
uint8_t buf[UART_BUF_SIZE]; // buffer to store data 
int ret = 0; // return value for ring buffer put
int bytes_read = 0; // number of bytes read from uart
uint8_t *msg = NULL;
uint8_t *last_msg = NULL; // inimplemented 
size_t current_size = 0;
int len = 0;
uint8_t target_state;
extern struct k_timer heartbeat_timer;
extern struct k_timer obc_quiesc_timer;

int
obcCommService()
{
    printk("DEBUG -- uartService\n");
    initObcComm();
    uint8_t data[64];
    while(1) // loop
    { 
        /* fetch sem for uart cb data */
        k_sem_take(&obc_comm_sem, K_MSEC(200));
        len = ring_buf_get(&in_buf, data, sizeof(data));
        if (len > 0) 
        {
            parseData(data, len);
        } // if
        /* Perform other work here */
        k_sleep(K_MSEC(200));
    } // while
    return 0;
} // uartService

void 
initObcComm()
{
    printk("DEBUG -- initUart\n");
    while(!device_is_ready(usart1))
    {
        printk("Waiting for UART device to be ready\n");
        k_sleep(K_MSEC(200));
    } // while
    printk("UART device %s is ready\n", usart1->name);
    /* configure interrupt and callback to receive data */
    int ret = uart_irq_callback_user_data_set(usart1, serial_obc_cb, NULL);
    if (ret < 0)
    {
        if (ret == -ENOTSUP)
            printk("Interrupt-driven UART API support not enabled\n");
        else if (ret == -ENOSYS)
            printk("Callback API not implemented\n");
        else
            printk("Unknown error: %d\n", ret);
        return;
    } // if
    printk("UART callback set\n");
    /* enable rx interrupts */
    uart_irq_rx_enable(usart1);
} // initUart

void 
serial_obc_cb(
    const struct device *dev, 
    void *user_data
    )
{
    memset(buf, 0, sizeof(buf)); // clear buffer
    bytes_read = 0; // reset bytes read
    while(uart_irq_update(dev) && uart_irq_is_pending(dev))
    {
        if(uart_irq_rx_ready(dev))
        {
            bytes_read = uart_fifo_read(dev, buf, sizeof(buf));
            if(bytes_read > 0)
            {
                ret = ring_buf_put(&in_buf, buf, bytes_read);
                if(ret != bytes_read)
                {
                    printk("Ring buffer full: %d\n", ret);
                } // if 
                k_sem_give(&obc_comm_sem); // signal semaphore
            } // if 
        } // if 
    } // while 
} // serial_obc_cb

void 
parseData(
    uint8_t *data, 
    long size
    )
{
    //printk("DEBUG -- parseUart\n");
    /* grab semaphore */
    k_sem_take(&obc_comm_sem, K_MSEC(200));
    // Ensure we're not allocating a zero-size buffer.
    if (size <= 0) 
    {
        printk("Invalid size\n"); // log error
        return;
    } // if
    // If the allocated memory size is not sufficient, or not allocated yet, reallocate
    if (msg == NULL || current_size < size) 
    {
        if (msg != NULL) 
        {
            k_free(msg); // Free previously allocated memory if it's not enough
        } // if
        msg = (uint8_t *)k_malloc(size); 
        if (msg == NULL) 
        {
            printk("Failed to allocate memory\n");
            return;
        } // if
       current_size = size;
    } // if
    // Proceed
    bool sync = false;
    // iterate through the data 
    size_t i = 0; 
    while (i < size - 1) 
    {
        if (data[i] == 0xBE && data[i + 1] == 0xEF) 
        {
            //printk("Found sync bytes\n");
            sync = true;
            // Create a message packet
            MessagePacket packet;
            packet.size = data[i + 3] + 6;
            packet.data = (uint8_t *)k_malloc(packet.size);
            if (packet.data == NULL) 
            {
                printk("Failed to allocate memory for message\n");
                break;
            } // if 
            memcpy(packet.data, data + i, packet.size);
            // Enqueue the packet
            if (k_msgq_put(&obc_comm_msgq, &packet, K_NO_WAIT) != 0) 
            {
                printk("Failed to put message in queue\n");
                k_free(packet.data);
                break;
            } // if 
            break;
        } // if 
        i++;
    } // if 
    if (sync) 
    {
        MessagePacket packet;
        // Try to dequeue the message
        if (k_msgq_get(&obc_comm_msgq, &packet, K_NO_WAIT) == 0) 
        {
            printk("Message: ");
            for (int j = 0; j < packet.size; j++) 
            {
                printk("0x%02X ", packet.data[j]);
            }
            printk("\n");
            // Process the message
            consumePacket(packet.data, packet.size);
            // Free allocated memory
            k_free(packet.data);
        } // if 
        else 
        {
            printk("Failed to get message from queue\n");
        } // if 
    } // if 
    else 
    {
        //printk("No sync bytes found\n");
    } // else 
    /* release semaphore */
    k_sem_give(&obc_comm_sem);
} // parseData

/**  
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * REVISED PACKET STRUCTURE ( OBC -> STM )
 * -+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
 * [ Sync High      | 1            | 0xBE    ]
 * [ Sync Low       | 1            | 0xEF    ]
 * [ Message Type   | 1            | 0x03    ]
 * [ Message Length | 1            | 0x0A    ]
 * [ Data           | 1 (maybe 2)  | 0x00    ]
 * [ Checksum msb   | 1            | 0x00    ]
 * [ Checksum lsb   | 1            | 0x00    ]
 * -+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * Message Types: 
 * -+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
 * [ 0x00: ack (report packet ack)                                               ] 
 * [ 0x01: err (signal to stm to switch to safe anom state)                      ]
 * [ 0x02: rst (signal to stm to restart the fsm)                                ]
 * [ 0x03: rsnd (signal to stm to resend last packet)                            ]
 * [ 0x04: hb (send a heartbeat signal from the state machine to the stm)        ]
 * [ 0x05: stChng (send a state change signal from the state machine to the stm) ]
 * [ 0x06: valid (confirm or deny a request)                                     ]
 * [ 0x07: setVal (set a value in the state machine)                             ]
 * [ 0x0A: reqPrm (request a parameter from the state machine)                   ]
 * -+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * [ Heartbeat packet ack structure ] 
 * -+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
 * [ 0xBE, 0xEF, 0x00, 0x01, 0x04, 0xB2, 0x78 ]
 * -+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
 * [ Heartbeat packet structure ] 
 * -+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
 * [ 0xBE, 0xEF, 0x04, 0x00, 0xB1, 0xCD ]
 */
void 
consumePacket(
    uint8_t *data, 
    long size
)
{
    printk("DEBUG -- consumePacket\n");
    msg_type = data[2];
    data_len = data[3];
    pkt_msb = data[data_len + 4]; /* msb */
    pkt_lsb = data[data_len + 5]; /* lsb */
    /* validate checksum */ 
    uint16_t checksum = ((uint16_t)pkt_msb << 8) | (uint16_t)pkt_lsb;
    uint16_t computed_checksum = computeChecksum(data, size - 2);
    /* reset quescent timer */
    if (checksum != computed_checksum)
    {
        printk("Checksum mismatch\n");
        return;
    } // if
    printk("Checksum validated\n");
    uint8_t prm, val;
    /* switch on data type */
    switch (msg_type) 
    { 
        case 0x00: /* ack received */
            printk(" (ack) \n");
            /* nullify last message */
            last_msg = NULL;
            /* check if heartbeat ack */
            if (data[4] == 0x04)
            {
                printk(" (ACK - HB) \n");
                /* adjust state machine state */
                changeState(IDL);
            } // if
            break;
        case 0x01: /* error msg received */
            printk(" (err) \n");
            /* send message ack */
            break;
        case 0x02: /* reset signal received */
            printk(" (rst) \n");
            /* send msg ack */
            /* reset stm state machine */
            changeState(SOV);
            break;
        case 0x03: /* resent signal received */
            printk(" (rsnd) \n");
            /* send msg ack */
            /* check data for count value */
            /* if count less than 3 (arbitrary), resend last message */
            break;
        case 0x04: /* heartbeat signal received */ 
            printk(" (hb) \n");
            /* send msg ack */
            if (getState() <= IDL)
            {
                changeState(IDL);
            } else if (getState() == CRU)
            {
                changeState(CRU);
            } // if
            break;
        case 0x05:
            printk(" (stChng) \n");
            /* send msg ack */ 
            /* grab target state from data */
            target_state = data[4];
            /* set stm state machine to target state */
            if (target_state == 0x00)
            {
                changeState(CRU);
            } // if 
            else if (target_state == 0x01) /* switch to idle */
            {
                changeState(IDL);
            } // else if
            else if (target_state == 0x02) /* rst */
            {
                changeState(SOV);
                /* reset stm memory buffers */
                /* ... */
            }
            break;
        case 0x06:
            printk(" (valid) \n");
            /* send msg ack */
            break;
        case 0x07: 
            printk(" (setVal) \n");
            /* send msg ack */
            /* take parameter field */
            prm = data[4];
            if (prm == 0x00) /* example */
            {
                /* debug */
                printk(" (prm: 0x00) \n");
            }
            /* take value for prm */
            val = data[5];
            /* debug */
            printk(" (val: 0x%02X) \n", val);
            break;
        case 0x0A:
            printk(" (reqPrm) \n");
            /* send msg ack */
            prm = data[4];
            if (prm == 0x00) /* example */
            {
                /* debug */
                printk(" (prm-req: 0x00) \n");
            }
            break;
        default: 
            break; 
    } // switch
} // consumePacket


/** 
 * -+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
 * [ Sent packet formatting ]
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * -+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
 * [ Field          | Bytes        | Desc.   ]
 * -+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
 * [ Sync High      | 1            | 0xBE    ]
 * [ Sync Low       | 1            | 0xEF    ]
 * [ Message Type   | 1            | 0x03    ]
 * [ Data Type      | 1            | 0x0A    ] needed to distinguish payload length in bytes
 * [ Payload Length | dynam. (1-2) | 0x00 .. ] 
 * [ Data           | dynam. (1-8?)| 0x00 .. ] 
 * [ Checksum msb   | 1            | 0x00    ]
 * [ Checksum lsb   | 1            | 0x00    ]
 * -+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
 * [ Data Type    | Repr.
 * -+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * [ 0x00         | ACK (acknowledgement) ]
 * [ 0x01         | State Update ( state ) ]
 * [ 0x02         | INA Feed 1 ( current, power ) ]
 * [ 0x03         | INA Feed 2 ( current, power ) ] 
 * [ 0x04         | INA Feed 3 ( current, power ) ]
 * [ 0x05         | INA Feed 4 ( current, power ) ] 
 * [ 0x06         | INA Feed 5 ( vbus, shunt, load, current, power ) ]
 * [ 0x07         | INA Feed 6 ( vbus, shunt, load, current, power ) ]
 * [ 0x08         | INA Feed 7 ( vbus, shunt, load, current, power ) ]
 * -+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
 * -+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
 * REVISED PACKET STRUCTURE ( STM -> OBC )
 * -+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * [ Sync High     | 1            | 0xBE    ]
 * [ Sync Low      | 1            | 0xEF    ]
 * [ Message Type  | 1            | 0x03    ]
 * [ Data Type     | 1            | 0x0A    ]
 * [ Payload Length| 1            | 0x00    ]
 * [ Data          | 1 (maybe 2)  | 0x00    ]
 * [ Checksum msb  | 1            | 0x00    ]
 * [ Checksum lsb  | 1            | 0x00    ]
 * -+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * Message Types:
 * -+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * [ 0x00: ack (report packet ack)                                     ]
 * [ 0x01: hb (send a heartbeat signal from the stm to the obc)        ] 
 * [ 0x02: stChng (send a state change signal from the stm to the obc) ]
 * [ 0x03: sensor (denote a single sensor reading to be sent)          ]
 * [ 0x04: group (denote that a group of sensors is to be specified)   ] 
 * [ 0x05: param (denote that a requested parameter is to be sent)     ] 
 * -+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * Data Types:
 * -+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * [ Type | Descr. | data_type | payload_len ]
 * [ ack  | 0x00   | NULL      | 0x00        ]
 * [ hb   | 0x01   | NULL      | 0x00        ]
 * [ stUp | 0x02   | NULL      | 0x00        ]
 * [ snsr | 0x03   | 0x00      | 0x00        ]
 * [ grp  | 0x04   | 0x00 0x00 | 0x00        ] // ? 
 * [ prm  | 0x05   | 0x00      | 0x00        ]
 * -+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 */
uint8_t*
constructPacket(
    uint8_t *data, 
    long size
)
{
    printk("DEBUG -- constructPacket\n");
    uint8_t sync_bytes[] = {0xBE, 0xEF};
    uint8_t msg_type; 
    uint8_t data_type;
    uint8_t payload_len;
    //uint8_t payload_data[];
    uint8_t checksum_msb;
    uint8_t checksum_lsb;
} // constructPacket

/**
 * -+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
 * [ Heartbeat packet formatting ]
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * -+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
 * [ Field          | Bytes        | Desc.   ]
 * -+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
 * [ Sync High      | 1            | 0xBE    ]
 * [ Sync Low       | 1            | 0xEF    ]
 * [ Message Type   | 1            | 0x00    ]
 * [ Checksum msb   | 1            | 0x00    ]
 * [ Checksum lsb   | 1            | 0x00    ]
 * -+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-`
 */
void 
heartbeat_cb(struct k_timer *heartbeat_timer)
{
    /* if quescent timer is running, return */
    if (k_timer_status_get(&obc_quiesc_timer) > 0)
    {
        return;
    } // if
    // printk("DEBUG -- sendHeartbeat\n");
    uint8_t pkt[8] = {0xBE, 0xEF}; /* sync bytes */
    pkt[2] = 0x01; // heartbeat message type
    pkt[3] = 0x00; // payload length
    uint16_t checksum = computeChecksum(pkt, sizeof(pkt) - 2);
    pkt[4] = (uint8_t)(checksum >> 8); // msb
    pkt[5] = (uint8_t)checksum; // lsb
    int ret = uart_fifo_fill(usart1, pkt, sizeof(pkt));
    if (ret < 0)
    {
        printk("Failed to send heartbeat\n"); // debug 
    } // if
    else 
    { 
        /* wait for uart to finish transmitting */
        while(!uart_irq_tx_complete(usart1))
        {
            k_yield();
        } // while
    } // else
    printk("Heartbeat sent\n"); // Debug
    memset(pkt, 0, sizeof(pkt)); // clear buffer
} // sendHeartbeat

/** 
 * -+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
 * [ Checksum computation ]
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * -+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
 * Arguments:
 * + data: pointer to data buffer
 * + size: size of data buffer
 * -+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
 */
uint16_t
computeChecksum(
    uint8_t *data, 
    uint16_t length
    )
{
    //printk("DEBUG -- computeChecksum\n");
    uint16_t checksum = 0;
    uint8_t lsb, msb; 
    for (uint8_t i = 0; i < length; i++)
    {
        lsb = checksum;
        msb = (checksum >> 8) + data[i];
        lsb += msb;
        checksum = ((uint16_t)msb << 8) | (uint16_t)lsb;
    } // for
    return checksum;
} // computeChecksum

