/*
 * i2cDriver.h
 *
 *  Created on: 23 May 2022
 *      Author: Baker
 */

#ifndef SENSORS_I2CDRIVER_H_
#define SENSORS_I2CDRIVER_H_

#include <ti/drivers/I2C.h>

#include <stdbool.h>

extern I2C_Handle i2c;

bool writeI2C(uint8_t ui8Addr, uint8_t ui8Reg, uint8_t *data, uint8_t writeCount, uint8_t readCount);
bool readI2C(uint8_t ui8Addr, uint8_t ui8Reg, uint8_t *data, uint8_t writeCount, uint8_t readCount);


#endif /* SENSORS_I2CDRIVER_H_ */
