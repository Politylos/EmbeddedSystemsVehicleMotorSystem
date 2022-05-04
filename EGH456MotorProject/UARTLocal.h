/*
 * UARTLocal.h
 *
 *  Created on: 5 May 2022
 *      Author: Sophia Politylo
 */




#ifndef DRIVERS_UARTLOCAL_H_

#include <ti/drivers/UART.h>
#include "Board.h"
#include <stdint.h>
#include <stdbool.h>

extern bool initUART(UART_Handle *uart);
extern void WriteUART(UART_Handle uart, char *text);

#define DRIVERS_UARTLOCAL_H_



#endif /* DRIVERS_UARTLOCAL_H_ */
