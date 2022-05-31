/*
 * opt3001.c
 *
 * Modified from Driver for the Texas Instruments OP3001 Optical Sensor
 *
 *  Created on: 23 May 2022
 *      Author: Michael Baker
 */


#include <stdbool.h>
#include <stdint.h>
#include <math.h>
#include "i2cDriver.h"
#include "opt3001.h"


/* Slave address */
#define OPT3001_I2C_ADDRESS             0x47

/* Register addresses */
#define REG_RESULT                      0x00
#define REG_CONFIGURATION               0x01
#define REG_LOW_LIMIT                   0x02
#define REG_HIGH_LIMIT                  0x03

#define REG_MANUFACTURER_ID             0x7E
#define REG_DEVICE_ID                   0x7F

/* Register values */
#define MANUFACTURER_ID                 0x5449  // TI
#define DEVICE_ID                       0x3001  // Opt 3001

#define CONFIG_RESET                    0xC810
#define CONFIG_TEST                     0xCC10

#define CONFIG_ENABLE                   0x10C4 // equivalent to 0xC410 as upper and lower bytes are received in reverse (100 ms, continuous)
#define CONFIG_DISABLE                  0x10C0 //  equivalent to 0xC010 as upper and lower bytes are received in reverse  (100 ms, shutdown)

/* Bit values */
#define DATA_RDY_BIT                    0x0080  // Data ready




/**************************************************************************************************
 * @fn          sensorOpt3001Test
 *
 * @brief       Run a sensor self-test
 *
 * @return      TRUE if passed, FALSE if failed
 **************************************************************************************************/
bool opt3001_read(uint16_t *rawData)
{
    bool success;
    uint16_t val;

    success = readI2C(OPT3001_I2C_ADDRESS, REG_CONFIGURATION, (uint8_t *)&val, 1, 2);


    if (success)
    {
        success = ((val>>8 &0xFF) & DATA_RDY_BIT) == DATA_RDY_BIT;
    }

    if (success)
    {
        success = readI2C(OPT3001_I2C_ADDRESS, REG_RESULT, (uint8_t *)&val, 1, 2);
    }

    if (success)
    {
        // Swap bytes
        *rawData = (val << 8) | (val>>8 &0xFF);
    }
    else
    {
        //    sensorSetErrorData
    }

    return (success);
}

/**************************************************************************************************
 * @fn          sensorOpt3001Convert
 *
 * @brief       Convert raw data to object and ambience temperature
 *
 * @param       rawData - raw data from sensor
 *
 * @param       convertedLux - converted value (lux)
 *
 * @return      none
 **************************************************************************************************/
void opt3001_convert(uint16_t rawData, double *convertedLux)
{
    uint16_t e, m;

    m = rawData & 0x0FFF;
    e = (rawData & 0xF000) >> 12;

    *convertedLux = m * (0.01 * exp2(e));
}


/*
 * Initialise opt3001 sensor
 */
bool opt3001_init(void)
{
    uint16_t val;

    // Check manufacturer ID
    readI2C(OPT3001_I2C_ADDRESS, REG_MANUFACTURER_ID, (uint8_t *)&val, 1, 2);
    val = (val << 8) | (val>>8 &0xFF);
    if (val != MANUFACTURER_ID)
    {
        return (false);
    }
    // Check device ID
    readI2C(OPT3001_I2C_ADDRESS, REG_DEVICE_ID, (uint8_t *)&val, 1, 2);
    val = (val << 8) | (val>>8 &0xFF);
    if (val != DEVICE_ID)
    {
        return (false);
    }
    // Enable sensor
    val = CONFIG_ENABLE;
    writeI2C(OPT3001_I2C_ADDRESS, REG_CONFIGURATION, (uint8_t*)&val, 3, 0);

    return true;
}
