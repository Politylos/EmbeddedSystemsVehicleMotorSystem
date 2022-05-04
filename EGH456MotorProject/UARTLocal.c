#include <ti/drivers/UART.h>
#include "Board.h"
#include <stdint.h>
#include <stdbool.h>
#include "UARTLocal.h"

bool initUART(UART_Handle *uart){
    UART_Params uartParams;
    /* Create a UART with data processing off. */
    UART_Params_init(&uartParams);
    uartParams.writeDataMode = UART_DATA_BINARY;
    uartParams.readDataMode = UART_DATA_BINARY;
    uartParams.readReturnMode = UART_RETURN_FULL;
    uartParams.readEcho = UART_ECHO_OFF;
    uartParams.baudRate = 115200;
    //uartParams.readMode = UART_MODE_CALLBACK;
    //uartParams.readCallback = &HwiUart;
    *uart = UART_open(Board_UART0, &uartParams);

    if (uart == NULL) {
        //System_abort("Error opening the UART");
        return false;

    }
    return true;
}
void WriteUART(UART_Handle uart, char *text){
    UART_write(uart, text, sizeof(text));
}
