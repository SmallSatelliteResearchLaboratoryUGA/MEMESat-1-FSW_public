#ifndef SENSOR_H
#define SENSOR_H

#include <zephyr/device.h>
#include <zephyr/drivers/i2c.h>
#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <zephyr/sys/util.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/random/random.h> 
#include <zephyr/types.h>
#include <string.h> 
#include <stdio.h> 

#define NUM_SOLAR_PANELS 6 
#define INA219_REG_CONFIG (0x00)
#define INA219_REG_SHUNT_VOLTAGE (0x01)
#define INA219_REG_BUS_VOLTAGE (0x02)
#define INA219_REG_POWER (0x03)
#define INA219_REG_CURRENT (0x04)
#define INA219_REG_CALIBRATION (0x05)

void sensorService(); 
void initSensorService();
void readFromDevice(const struct device *dev, uint8_t devAddr, double *regval);
void generateFakeSensorData(float *data);


#endif // SENSOR_H