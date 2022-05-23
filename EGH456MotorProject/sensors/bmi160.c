/*
 * bmi160.c
 *
 *  Created on: 13 May 2022
 *      Author: Michael Baker
 */
#include "bmi160.h"
#include <stdbool.h>
#include <stdint.h>
#include "i2cDriver.h"


// I2C Address
#define BMI160_I2C_ADDRESS      0x69
// Registers
#define BMI160_CHIP_ID_REG      0x00
#define BMI160_PMU_STATUS_REG   0x03
#define BMI160_DATA_ACCX_REG    0x12
#define BMI160_ACC_RANGE_REG    0x41
#define BMI160_COMMAND_REG      0x7E
// Device Commands
#define BMI160_ACC_RANGE_2G     0x03
#define BMI160_ACC_MODE_STDBY   0x10
#define BMI160_ACC_MODE_NORMAL  0x11
#define BMI160_GYR_MODE_STDBY   0x14
#define BMI160_GYR_MODE_NORMAL  0x15
#define BMI160_MAG_MODE_STDBY   0x18
#define BMI160_MAG_MODE_NORMAL  0x19
#define BMI160_SOFTRESET_CMD    0xB6
// Device values
#define BMI160_CHIP_ID          0xD1

//// TODO Maybe make these dynamic, as in able to set and read from device
//const int BMI160_ACC_RANGE        2;

/* Basic delay for initialisation */
void bmi160_delay(uint16_t value)
{
    uint16_t i;
    for (i = 0; i < value; i++);
}

/*
 * Read accelerometer
 */
bool bmi160_readAccel(int16_t *bmi160_accel_values)
{
    bool success;
    uint8_t result[2];
    //uint16_t bmi160_accel_values[3];

//    uint16_t resultx;

    success = readI2C(BMI160_I2C_ADDRESS, BMI160_DATA_ACCX_REG, (uint8_t*)bmi160_accel_values, 1, 6);
    //*rawData = (((uint16_t)result[1]) << 8) | result[0];
    //rawData = bmi160_accel_values;

    return success;
}

/* Convert 2C value to float */
double bmi160_readConvert(int16_t rawValue)
{
//    float value;

    return ((double)(rawValue*(2.0/32768.0)*9.8));
}

bool bmi160_getAccel(bmi160_accel_values_t *values)
{
    bool success;
    int16_t axis_values[3];

    success = readI2C(BMI160_I2C_ADDRESS, BMI160_DATA_ACCX_REG, (uint8_t*)&axis_values, 1, 6);


    values->acc_x = bmi160_readConvert(axis_values[0]);
    values->acc_y = bmi160_readConvert(axis_values[1]);
    values->acc_z = bmi160_readConvert(axis_values[2]);

    return success;
}

/*
 * Initialise the sensor
 */
bool bmi160_init(void)
{
    uint8_t val;
    uint8_t result[2];
    bool success, powerup = false;
    uint16_t dummyRead;

    // Read device ID
    success = readI2C(BMI160_I2C_ADDRESS, BMI160_CHIP_ID_REG, (uint8_t*)&result, 1, 1);

    bmi160_delay(1000);

    // Check device ID matches datasheet specs
    if (success)
    {
        if (result[0] != BMI160_CHIP_ID)
        {
            // Device doesn't have correct ID
            return false;
        }
    }

    // Send soft reset command to device
    val = BMI160_SOFTRESET_CMD;
    success = writeI2C(BMI160_I2C_ADDRESS, BMI160_COMMAND_REG, (uint8_t*)&val, 2, 0);
    bmi160_delay(10000);

    // Start accelerometer, normal power mode
    val = BMI160_ACC_MODE_NORMAL;
    success = writeI2C(BMI160_I2C_ADDRESS, BMI160_COMMAND_REG, (uint8_t*)&val, 2, 0);
    bmi160_delay(10000);

    // Wait for accelerometer to power up
    // TODO add timeout
    while(!powerup)
    {
        success = readI2C(BMI160_I2C_ADDRESS, BMI160_PMU_STATUS_REG, (uint8_t*)&result, 1, 1);
        // Check if acc_pmu_status is normal
        uint8_t bitmask = (1 << 4);
        uint8_t bitresult = result[0];
        if (bitresult & bitmask)
        {
            powerup = true;
        }
        else
        {
            bmi160_delay(1000);
        }
    }

    val = BMI160_ACC_RANGE_2G;
    success = writeI2C(BMI160_I2C_ADDRESS, BMI160_ACC_RANGE_REG, (uint8_t*)&val, 2, 0);
    bmi160_delay(1000);

    //bmi160_readAccel(&dummyRead);

    return success;
}


