/**************************************************************************************************
*  Filename:       i2cOptDriver.c
*  By:             Jesse Haviland
*  Created:        1 February 2019
*  Revised:        23 March 2019
*  Revision:       2.0
*
*  Description:    i2c Driver for use with opt3001.c and the TI OP3001 Optical Sensor
*************************************************************************************************/



// ----------------------- Includes -----------------------
#include "i2cOptDriver.h"
#include "inc/hw_memmap.h"
//#include "driverlib/i2c.h"
#include <ti/drivers/I2C.h>

#include "utils/uartstdio.h"
#include "driverlib/sysctl.h"
#include "Board.h"

I2C_Handle      i2c;
bool InitI2C(){
    /* Create I2C for usage */
        I2C_Params      i2cParams;
        I2C_Params_init(&i2cParams);
        i2cParams.bitRate = I2C_400kHz;
        i2cParams.transferMode = I2C_MODE_CALLBACK;
        i2c = I2C_open(Board_I2C0, &i2cParams);
        if (i2c == NULL) {

            return false;
        }
        else {

            return true;
        }
        return false;
}
/*
 * Sets slave address to ui8Addr
 * Puts ui8Reg followed by two data bytes in *data and transfers
 * over i2c
 */
bool writeI2C(uint8_t ui8Addr, uint8_t ui8Reg, uint8_t *data)
{
//    // Load device slave address
//    I2CMasterSlaveAddrSet(I2C0_BASE, ui8Addr, false);
//
//    // Place the character to be sent in the data register
//    I2CMasterDataPut(I2C0_BASE, ui8Reg);
//    I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_BURST_SEND_START);
//    while(I2CMasterBusy(I2C0_BASE)) { }
//
//    // Send Data
//    I2CMasterDataPut(I2C0_BASE, data[0]);
//    I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_BURST_SEND_CONT);
//    while(I2CMasterBusy(I2C0_BASE)) { }
//
//    I2CMasterDataPut(I2C0_BASE, data[1]);
//    I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_BURST_SEND_FINISH);
//
//    // Delay until transmission completes
//    while(I2CMasterBusBusy(I2C0_BASE)) { }

    I2C_Transaction i2cTransaction;
    uint8_t         txBuffer[3];
    uint8_t         rxBuffer[2];

    txBuffer[0] = ui8Reg;
    txBuffer[1] = data[0];
    txBuffer[2] = data[1];
    i2cTransaction.slaveAddress = ui8Addr;
    i2cTransaction.writeBuf = txBuffer;
    i2cTransaction.writeCount = 3;
    i2cTransaction.readBuf = rxBuffer;
    i2cTransaction.readCount = 1;

    I2C_transfer(i2c, &i2cTransaction);


    return true;
}



/*
 * Sets slave address to ui8Addr
 * Writes ui8Reg over i2c to specify register being read from
 * Reads three bytes from i2c slave. The third is redundant but
 * helps to flush the i2c register
 * Stores first two received bytes into *data
 */
bool readI2C(uint8_t ui8Addr, uint8_t ui8Reg, uint8_t *data)
{
//    uint16_t delay = 1000;
//    uint8_t byteA, byteB;
//
//    // Load device slave address
//    I2CMasterSlaveAddrSet(I2C0_BASE, ui8Addr, false);
//
//    // Place the character to be sent in the data register
//    I2CMasterDataPut(I2C0_BASE, ui8Reg);
//    I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_SINGLE_SEND);
//    while(I2CMasterBusy(I2C0_BASE)) { }
//
//    // Load device slave address
//    I2CMasterSlaveAddrSet(I2C0_BASE, ui8Addr, true);
//
//    // Read two bytes from I2C
//    I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_BURST_RECEIVE_START);
//    while(I2CMasterBusy(I2C0_BASE)) { }
//    byteA = I2CMasterDataGet(I2C0_BASE);
//
//
//    I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_BURST_RECEIVE_FINISH);
//    SysCtlDelay(delay);
//    byteB = I2CMasterDataGet(I2C0_BASE);
//
//    data[0] = byteA;
//    data[1] = byteB;


    I2C_Transaction i2cTransaction;
    uint8_t         txBuffer[1];
    uint8_t         rxBuffer[2];

    txBuffer[0] = ui8Reg;
    i2cTransaction.slaveAddress = ui8Addr;
    i2cTransaction.writeBuf = txBuffer;
    i2cTransaction.writeCount = 1;
    i2cTransaction.readBuf = data;
    i2cTransaction.readCount = 2;

    I2C_transfer(i2c, &i2cTransaction);



    return true;
}


