#include<stdint.h>
#include<stdbool.h>
#include<string.h>
#include"inc/hw_memmap.h"
#include"inc/tm4c123gh6pm.h"
#include"inc/hw_types.h"
#include"driverlib/gpio.h"
#include"driverlib/pin_map.h"
#include"driverlib/sysctl.h"
#include"driverlib/uart.h"
#include"utils/uartstdio.h"
#include"utils/uartstdio.c"


#include <xdc/std.h>
#include <xdc/runtime/System.h>

/* BIOS Header files */
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/hal/Timer.h>
#include <ti/sysbios/hal/Hwi.h>

#include <ti/drivers/GPIO.h>
#include <ti/drivers/UART.h>
#include "driverlib/sysctl.h"
#include "Board.h"

 unsigned char writ[20];

 UART_Handle uart;
 UART_Params uartParams;
 unsigned char rxBuffer[20];

void Timer()
{
     Hwi_post(22);
     SysCtlDelay(13333333);
     Hwi_post(21);
}

void HWI_0(UArg arg0)
{
    int x=0;
    int len;
    UART_read(uart, rxBuffer, sizeof(rxBuffer));
    SysCtlDelay(13333333*3);
    len=strlen(rxBuffer);
      for(x=0; x<len; x++)
      {
         UARTCharPut(UART1_BASE,rxBuffer[x]);
      }
}

void HWI_1(UArg arg0)
{
    int x=0,i=0;
    int len;
   if(UARTCharsAvail(UART1_BASE))
      {
        writ[i]=UARTCharGet(UART1_BASE);
        i+=1;
      }
   if(i>=sizeof(writ))
   {
       i=0;
   }

   len=strlen(writ);
      for(x=0; x<len; x++)
      {
          UART_write(uart, writ, sizeof(writ));
      }
   SysCtlDelay(13333333*3);
}


int main(void)
{

    Board_initGeneral();
    Board_initGPIO();


    //uart0

       SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
       SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
       GPIOPinConfigure(GPIO_PA0_U0RX); // PB0 IS CONFIGURED TO UART1 RX
       GPIOPinConfigure(GPIO_PA1_U0TX); // PB0 IS CONFIGURED TO UART1 RX
       GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);
       UARTClockSourceSet(UART0_BASE, UART_CLOCK_PIOSC);
       UARTStdioConfig(0, 9600, 16000000);

    // UART 1

    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART1);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
    GPIOPinConfigure(GPIO_PB0_U1RX); // PB0 IS CONFIGURED TO UART1 RX
    GPIOPinConfigure(GPIO_PB1_U1TX); // PB0 IS CONFIGURED TO UART1 RX
    GPIOPinTypeUART(GPIO_PORTB_BASE, GPIO_PIN_0 | GPIO_PIN_1);
    UARTClockSourceSet(UART1_BASE, UART_CLOCK_PIOSC);
    UARTStdioConfig(1, 9600, 16000000);

    BIOS_start();

    return (0);
}
