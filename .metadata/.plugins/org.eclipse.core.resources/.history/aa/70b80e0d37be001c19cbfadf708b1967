#include<stdint.h>
#include<stdbool.h>
#include<string.h>
#include"inc/hw_memmap.h"
#include"inc/hw_types.h"
#include"driverlib/gpio.h"
#include"driverlib/pin_map.h"
#include"driverlib/sysctl.h"
#include"driverlib/uart.h"
#include"utils/uartstdio.h"
#include"utils/uartstdio.c"


int main(void)
{

    int x=0,len=0;
    SysCtlClockSet(SYSCTL_SYSDIV_5|SYSCTL_USE_PLL|SYSCTL_OSC_MAIN|SYSCTL_XTAL_16MHZ);

    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART1);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
    GPIOPinConfigure(GPIO_PB0_U1RX); // PB0 IS CONFIGURED TO UART1 RX
    GPIOPinConfigure(GPIO_PB1_U1TX); // PB0 IS CONFIGURED TO UART1 RX
    GPIOPinTypeUART(GPIO_PORTB_BASE, GPIO_PIN_0 | GPIO_PIN_1);
    UARTClockSourceSet(UART1_BASE, UART_CLOCK_PIOSC);
    UARTStdioConfig(1, 9600, 16000000);

    char text[]="Welcome to UART Communication Protocol\n";
    len=strlen(text);
    for(x=0; x<len; x++)
    {
       UARTCharPut(UART1_BASE,text[x]);
    }
}
