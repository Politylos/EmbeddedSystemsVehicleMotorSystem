/*
 * Copyright (c) 2015, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*
 *  ======== empty.c ========
 */
/* XDCtools Header files */
#include <xdc/std.h>
#include <xdc/runtime/System.h>

/* BIOS Header files */
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Task.h>

#include <time.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <math.h>


#include "inc/hw_memmap.h"

/* TI-RTOS Header files */
#include <ti/drivers/GPIO.h>
#include <driverlib/gpio.h>
#include <ti/sysbios/hal/Hwi.h>
#include <ti/sysbios/knl/swi.h>
#include <ti/sysbios/knl/Clock.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Event.h>
#include <ti/sysbios/knl/Queue.h>
#include <ti/sysbios/knl/Semaphore.h>
#include <xdc/runtime/Error.h>
#include <xdc/runtime/System.h>
#include <driverlib/sysctl.h>
#include <driverlib/pin_map.h>
#include <ti/sysbios/hal/Timer.h>



#include "driverlib/rom.h"
#include "driverlib/rom_map.h"
#include "grlib/grlib.h"
#include "drivers/frame.h"
#include "drivers/kentec320x240x16_ssd2119_spi.h"
#include "drivers/pinout.h"
#include "grlib/widget.h"
#include "grlib/canvas.h"
#include "grlib/pushbutton.h"
#include "drivers/touch.h"
#include <driverlib/sysctl.h>
#include "utils/ustdlib.h"
#include <math.h>
#include <string.h>
#include "inc/hw_memmap.h"
#include <driverlib/gpio.h>
#include <ti/sysbios/hal/Seconds.h>
#include <time.h>
#include <drivers/motorlib.h>
#include <ti/drivers/UART.h>
#include "UARTLocal.h"
/* Board Header file */
#include "Board.h"
#include "empty.h"
//#include <ti/sysbios/family/arm/msp432/Seconds.h>
#include <ti/sysbios/hal/Seconds.h>

#define TASKSTACKSIZE   1024

typedef struct FullTime{
    int year;
    int months;
    int days;
    int hours;
    int minutes;
    int seconds;
}FT;

// Graphics
tContext sContext;
uint8_t motorStartStop = 1;
UART_Handle uart;
FT datetime;

Timer_Handle timerclock;


Task_Struct task0Struct;
Char task0Stack[TASKSTACKSIZE];


tCanvasWidget     g_sBackground;
tPushButtonWidget g_sStartStopBttn;

void StartStopBttnPress(tWidget *psWidget);

// The canvas widget acting as the background to the display.
Canvas(g_sBackground, WIDGET_ROOT, 0, &g_sStartStopBttn,
       &g_sKentec320x240x16_SSD2119, 10, 25, 300, (240 - 25 -10),
       CANVAS_STYLE_FILL, ClrBlack, 0, 0, 0, 0, 0, 0);

RectangularButton(g_sStartStopBttn, &g_sBackground, 0, 0,
                  &g_sKentec320x240x16_SSD2119, 50, 200, 100, 25,
                  (PB_STYLE_OUTLINE | PB_STYLE_TEXT_OPAQUE | PB_STYLE_TEXT |
                   PB_STYLE_FILL | PB_STYLE_RELEASE_NOTIFY),
                   ClrDarkBlue, ClrBlue, ClrWhite, ClrWhite,
                   g_psFontCmss16b, "Start", 0, 0, 0, 0, StartStopBttnPress);



void StartStopBttnPress(tWidget *psWidget)
{
    motorStartStop = !motorStartStop;

    if(motorStartStop)
    {
        //
        // Change the button text to indicate the new function.
        //
        PushButtonTextSet(&g_sStartStopBttn, "Stop");

        //
        // Repaint the pushbutton and all widgets beneath it (in this case,
        // the welcome message).
        //
        WidgetPaint((tWidget *)&g_sStartStopBttn);
    }
    else
    {
        //
        // Change the button text to indicate the new function.
        //
        PushButtonTextSet(&g_sStartStopBttn, "Start");

        WidgetPaint((tWidget *)&g_sStartStopBttn);
    }
}



//void ADC3_Init() //ADC0 on PE3
//{
//    //SysCtlPeripheralEnable( SYSCTL_PERIPH_ADC0 );
//    //SysCtlPeripheralEnable( SYSCTL_PERIPH_GPIOE );
//
//    //Makes GPIO an INPUT and sets them to be ANALOG
//    GPIOPinTypeADC( GPIO_PORTE_BASE, GPIO_PIN_3 );
//
//    //uint32_t ui32Base, uint32_t ui32SequenceNum, uint32_t ui32Trigger, uint32_t ui32Priority
//    ADCSequenceConfigure(ADC0_BASE, 0, ADC_TRIGGER_ALWAYS, 0);
//
//    //uint32_t ui32Base, uint32_t ui32SequenceNum, uint32_t ui32Step, uint32_t ui32Config
//    ADCSequenceStepConfigure( ADC0_BASE, 0, 0, ADC_CTL_IE | ADC_CTL_CH0 | ADC_CTL_END );
//
//    // Set oversampling
////    ADCHardwareOversampleConfigure(ADC0_BASE, 64);
//
//    ADCSequenceEnable( ADC0_BASE, 0 );
//
//    ADCIntClear( ADC0_BASE, 0 );
//
//    current = (Current*) malloc(sizeof(Current));
//}


/*
 *  ======== heartBeatFxn ========
 *  Toggle the Board_LED0. The Task_sleep is determined by arg0 which
 *  is configured for the heartBeat Task instance.
 */
void MotorInit(){
    initMotorLib(20, NULL);
    setDuty(25);
}
void MotorTest(){

    updateMotor(0, 1, 0);
    SysCtlDelay(1000);

    updateMotor(0, 0, 1);

    SysCtlDelay(1000);
    updateMotor(1, 0, 0);
    SysCtlDelay(1000);

}
Void heartBeatFxn(UArg arg0, UArg arg1)
{
    //Timer_start(timerclock);
   // MotorInit();
    // Add the compile-time defined widgets to the widget tree.
    WidgetAdd(WIDGET_ROOT, (tWidget *)&g_sBackground);

    // Paint the widget tree to make sure they all appear on the display.
    WidgetPaint(WIDGET_ROOT);
    while (1) {
        //SysCtlDelay(100);
        //GPIO_toggle(Board_LED0);
        //MotorTest();
   //     WidgetMessageQueueProcess();


//        TouchScreenIntHandler
    }
}

/*
 *  ======== main ========
 */
FT timeConvert(Seconds_Time time){
    FT ft;
    ft.year = floor(1970+time.secs/31556926);//10%5;//((int)time.secs/31556926 - time%31556926);
    int leap = floor((ft.year/4)-(ft.year/100)+(ft.year/400)-((1970/4)-(1970/100)+(1970/400)));
    int normyear = floor((ft.year-1970)-leap);
    //ft.months = ((time.secs%31556926)/2629743);// -time%31556926%2629743;
    int left = floor((((time.secs-(31536000*normyear+leap*31622400)))));
    ft.days = floor((left/86400));

    int l = 0;
    if(ft.year%400+ ft.year%400+ ft.year%4 +ft.year%100 == 0){
        l++;
    }
    ft.months=0;
    int sday = ft.days*24*60*60;
    if ((ft.days - 31) > 0){
        ft.days=ft.days- 31;
        ft.months=1;

        if (((ft.days - 27+l)) > 0){
                ft.days=ft.days- (27+l);
                ft.months=2;

                if (((ft.days - 31)) > 0){
                             ft.days=ft.days- 31;
                             ft.months=3;
                             if ((ft.days - 30) > 0){
                                 ft.days=ft.days- 30;
                                 ft.months=4;
                                 if (((ft.days - 31)) > 0){
                                             ft.days=ft.days- 31;
                                             ft.months=5;

                                             if (((ft.days - 30)) > 0){
                                                         ft.days=ft.days- 30;
                                                         ft.months=6;

                                                         if (((ft.days - 31)) > 0){
                                                                     ft.days=ft.days- 31;
                                                                     ft.months=7;

                                                                     if (((ft.days - 31)) > 0){
                                                                                 ft.days=ft.days- 31;
                                                                                 ft.months=8;

                                                                                 if (((ft.days - 30)) > 0){
                                                                                             ft.days=ft.days- 30;
                                                                                             ft.months=9;

                                                                                             if (((ft.days - 31)) > 0){
                                                                                                         ft.days=ft.days- 31;
                                                                                                         ft.months=10;

                                                                                                         if (((ft.days - 30)) > 0){
                                                                                                                     ft.days=ft.days- 30;
                                                                                                                     ft.months=11;
                                                                                                                     if (((ft.days - 31)) > 0){
                                                                                                                                 ft.days=ft.days- 31;
                                                                                                                                 ft.months=12;
                                                                                                                     }
                                                                                                         }

                                                                                             }

                                                                                 }

                                                                     }

                                                         }

                                             }

                                 }

                             }

                 }

        }

    }
    ft.hours = (((floor(((left%86400))/3600)+10)));//((((time.secs%31556926)%2629743)%86400)/3600);//(time.secs%31556926%2629743%86400/3600) -time%31556926%2629743%86400%3600;
    ft.minutes = floor(((left%86400)%3600)/60);//(((((time.secs%31556926)%2629743)%86400)%3600)/60);// (time.secs%31556926%2629743%86400%3600/60) -time%31556926%2629743%86400%3600%60;
    ft.seconds = floor(((left%86400)%3600)%60);
    return ft;

}
void updatetime(){
    Seconds_Time secs;
    UInt32 test3 =Seconds_getTime(&secs);
    datetime = timeConvert(secs);
}
bool timer1sec(){
    Timer_Params params;
    Timer_Params_init(&params);
    params.period = 1000000;
    // Period of a tick
    params.periodType = ti_sysbios_interfaces_ITimer_PeriodType_MICROSECS;

    // Period type
    params.runMode = ti_sysbios_interfaces_ITimer_RunMode_CONTINUOUS;
    // Timer run mode
    params.startMode = ti_sysbios_interfaces_ITimer_StartMode_AUTO;
    params.arg =  NULL;
    timerclock=Timer_create(1,updatetime,&params,NULL);
    return true;
}
int main(void)
{
    Task_Params taskParams;

    /* Call board init functions */
    Board_initGeneral();
    Board_initGPIO();
    Board_initI2C();
    Board_initUART();
    PinoutSet(false, false);


    /* Construct heartBeat Task  thread */
    Task_Params_init(&taskParams);
    taskParams.arg0 = 1000;
    taskParams.stackSize = TASKSTACKSIZE;
    taskParams.stack = &task0Stack;
    Task_construct(&task0Struct, (Task_FuncPtr)heartBeatFxn, &taskParams, NULL);
    time_t t;
      struct tm *ltm;
      char *curTime;

      Seconds_set(1651752452);//1651751655);

      t = time(NULL);
      ltm = localtime(&t);
      curTime = asctime(ltm);
      System_printf("Time(GMT): %s\n", curTime);
      Seconds_Time  test4;
      Seconds_Module_startup();
    UInt32 test3 = Seconds_getTime(&test4);

    timeConvert(test4);
    // Turn on user LED
    GPIO_write(Board_LED0, Board_LED_ON);

    //tContext sContext;
    Kentec320x240x16_SSD2119Init(120000000);
    GrContextInit(&sContext, &g_sKentec320x240x16_SSD2119);
    TouchScreenInit(120000000);
    TouchScreenCallbackSet(WidgetPointerMessage);
    if (!initUART(&uart)){
        System_printf("EROR\n");
    } else{
        WriteUART(uart,"UART Running/r\n");
       // char temp[40];
        //sprintf(temp,"%s\n\r",curTime);
       // WriteUART(uart,"UART Running\r\n");
      //  WriteUART(uart,temp);
    }



    FrameDraw(&sContext, "Touch Screen Test");

    System_printf("Starting the example\nSystem provider is set to SysMin. "
                  "Halt the target to view any SysMin contents in ROV.\n");
    /* SysMin will only print to the console when you call flush or exit */
    System_flush();

    /* Start BIOS */
    timer1sec();
    BIOS_start();

    return (0);
}
