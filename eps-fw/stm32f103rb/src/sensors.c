#include "sensors.h" 

K_MSGQ_DEFINE(sensor_msgq, 64, 10, 8);

/**
const struct device *inaDevices[7] = {
    DEVICE_DT_GET(DT_INST(0, ti_ina219)), 
    DEVICE_DT_GET(DT_INST(1, ti_ina219)), 
    DEVICE_DT_GET(DT_INST(2, ti_ina219)), 
    DEVICE_DT_GET(DT_INST(3, ti_ina219)), 
    DEVICE_DT_GET(DT_INST(4, ti_ina219)), 
    DEVICE_DT_GET(DT_INST(5, ti_ina219)), 
    DEVICE_DT_GET(DT_INST(6, ti_ina219))
};
*/
int inaStatus[7] = {0}; 
const uint8_t inaAddresses[7] = {
    0x40, 
    0x41, 
    0x42, 
    0x43, 
    0x44, 
    0x45, 
    0x46
};
const uint8_t primaryRegs[3] = {
    INA219_REG_BUS_VOLTAGE, 
    INA219_REG_POWER, 
    INA219_REG_CURRENT
};
static struct sensor_value v_bus, power, current; 
double inaReadings[3] = {0};
double regval[3] = {0};

void sensorService()
{
    printk("DEBUG -- sensorService\n");
    initSensorService();
    while(1)
    {
        //printk("DEBUG -- sensorService loop\n");
        /* TESTING */
        readFromDevice(NULL, 0, regval);
        /* allow other threads to do work */
        k_sleep(K_MSEC(500));
    } // while
} // sensorService

void initSensorService()
{
    printk("DEBUG -- initSensorService\n");
} // initSensorService

void 
readFromDevice(
    const struct device *dev, 
    uint8_t devAddr,
    double *regval
    )
{
    //printk(" -- DEBUG -- : readFromDevice()\n");
    /**
    int ret;
    printk(" -- DEBUG -- : readFromDevice() :: devAddr: %X\n", devAddr);
    sensor_channel_get(dev, primaryRegs[0], &v_bus);
    sensor_channel_get(dev, primaryRegs[1], &power);
    sensor_channel_get(dev, primaryRegs[2], &current);
    regval[0] = sensor_value_to_double(&v_bus);
    regval[1] = sensor_value_to_double(&power);
    regval[2] = sensor_value_to_double(&current);
    char buffer[32];
    char message[80];
    */ 
    char buffer[32];
    float data[3] = {0};
    /* generate fake sensor data */
    generateFakeSensorData(data);
    /* add data to regval */
    for (int i = 0; i < 3; i++)
    {
        regval[i] = data[i];
    } // for
    // add message to queue
    k_msgq_put(&sensor_msgq, regval, K_NO_WAIT);
    // try to get message from queue
    if (k_msgq_get(&sensor_msgq, regval, K_NO_WAIT) == 0)
    {
        printk("Message: ");
        for (int i = 0; i < 3; i++)
        {
            snprintf(buffer, 32, "%f", regval[i]);
            printk("%s ", buffer);
        } // for
        printk("\n");
    } // if
    else 
    {
        printk("Failed to get message from queue\n");
    } // else
} // readFromDevice

void 
generateFakeSensorData(
    float *data
    )
{
    //printk(" -- DEBUG -- : generateFakeSensorData()\n");
    for (int i = 0; i < 3; i++)
    {
        data[i] = ((float)sys_rand32_get() / UINT32_MAX) * 10.0f;
    } // for
} // generateFakeSensorData
