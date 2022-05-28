/*
 * i2cDriver.c
 *
 *  Created on: 23 May 2022
 *      Author: Michael Baker
 */

#include "i2cDriver.h"

/* Write to I2C device
 * TODO return state of xfer
 * TODO add documentation
 */
bool writeI2C(uint8_t ui8Addr, uint8_t ui8Reg, uint8_t *data, uint8_t writeCount, uint8_t readCount)
{
    I2C_Transaction i2cTransaction;
    uint8_t         txBuffer[3];
    uint8_t         rxBuffer[2];

    txBuffer[0] = ui8Reg;
    txBuffer[1] = data[0];
    txBuffer[2] = data[1];
    i2cTransaction.slaveAddress = ui8Addr;
    i2cTransaction.writeBuf = txBuffer;
    i2cTransaction.writeCount = writeCount;
    i2cTransaction.readBuf = rxBuffer;
    i2cTransaction.readCount = readCount;

//    Semaphore_pend(i2c_semHandle, BIOS_WAIT_FOREVER);
    I2C_transfer(i2c, &i2cTransaction);
//    Semaphore_post(i2c_semHandle);

    return true;
}

/* Read from I2C device
 * TODO return state of xfer
 * TODO add documentation
 */
bool readI2C(uint8_t ui8Addr, uint8_t ui8Reg, uint8_t *data, uint8_t writeCount, uint8_t readCount)
{
    I2C_Transaction i2cTransaction;
    uint8_t         txBuffer[2];
//    uint8_t         rxBuffer[2];

    txBuffer[0] = ui8Reg;
    i2cTransaction.slaveAddress = ui8Addr;
    i2cTransaction.writeBuf = txBuffer;
    i2cTransaction.writeCount = writeCount;
    i2cTransaction.readBuf = data;
    i2cTransaction.readCount = readCount;

    I2C_transfer(i2c, &i2cTransaction);

    return true;
}
