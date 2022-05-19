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
 #include <ti/drivers/I2C.h>
#include <xdc/runtime/Error.h>
#include <xdc/runtime/System.h>
#include <driverlib/sysctl.h>
#include <driverlib/pin_map.h>
#include <ti/sysbios/hal/Timer.h>

#include "driverlib/rom.h"
#include "driverlib/rom_map.h"
#include "drivers/pinout.h"
#include "grlib/grlib.h"
#include "grlib/widget.h"
#include "grlib/canvas.h"
#include "grlib/checkbox.h"
#include "grlib/container.h"
#include "grlib/pushbutton.h"
#include "grlib/radiobutton.h"
#include "grlib/slider.h"
#include "utils/ustdlib.h"
#include "drivers/Kentec320x240x16_ssd2119_spi.h"
#include "drivers/touch.h"
#include "drivers/frame.h"


#include "driverlib/udma.h"




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
#include "Clock.h"
#include"Interface/GraphFunctions.h"
#include "drivers/Sensors/SensorsClock.h"
//#include <ti/sysbios/family/arm/msp432/Seconds.h>
#include <ti/sysbios/hal/Seconds.h>
#include "Images/GUIImg.h"
#include "Images/Images.h"
#include "drivers/i2cOptDriver.h"
#include "Interface/GlobalDraw.h"
#include <ti/sysbios/knl/Semaphore.h>
#include <ti/sysbios/gates/GateMutex.h>
#include <ti/sysbios/gates/GateMutexPri.h>

#define TASKSTACKSIZE   2048

#ifdef ewarm
#pragma data_alignment=1024
tDMAControlTable psDMAControlTable[64];
#elif defined(ccs)
#pragma DATA_ALIGN(psDMAControlTable, 1024)
tDMAControlTable psDMAControlTable[64];
#else
tDMAControlTable psDMAControlTable[64] __attribute__ ((aligned(1024)));
#endif

// Graphics


uint8_t motorStartStop = 1;
UART_Handle uart;


FT datetime;

Timer_Handle timerclock;
bool HA=false;
bool HB=false;
bool HC=false;

Task_Struct task0Struct;
Task_Struct taskTouchStruct;
Char task0Stack[TASKSTACKSIZE];
Char taskTouchStack[TASKSTACKSIZE];
char tempc[30];

//tCanvasWidget     g_sBackground;
tPushButtonWidget g_sStartStopBttn;

void StartStopBttnPress(tWidget *psWidget);
void OnPrevious(tWidget *psWidget);
void OnNext(tWidget *psWidget);
void OnIntroPaint(tWidget *psWidget, tContext *psContext);
void OnPrimitivePaint(tWidget *psWidget, tContext *psContext);
void OnCanvasPaint(tWidget *psWidget, tContext *psContext);
void OnCheckChange(tWidget *psWidget, uint32_t bSelected);
void OnButtonPress(tWidget *psWidget);
void OnRadioChange(tWidget *psWidget, uint32_t bSelected);
void OnSliderChange(tWidget *psWidget, int32_t i32Value);
extern tCanvasWidget g_psPanels[];
tRectangle sRect;
bool LowLight=0;
//tCanvasWidget g_sCanvas2;
tPushButtonWidget brrrr ;
double dd[50]={0,0.0416493127863390,0.166597251145356,0.374843815077051,0.666389004581425,1.04123281965848,1.49937526030821,2.04081632653061,2.66555601832570,3.37359433569346,4.16493127863390,5.03956684714702,5.99750104123282,7.03873386089130,8.16326530612245,9.37109537692628,10.6622240733028,12.0366513952520,13.4943773427738,15.0354019158684,16.6597251145356,18.3673469387755,20.1582673885881,22.0324864639734,23.9900041649313,26.0308204914619,28.1549354435652,30.3623490212412,32.6530612244898,35.0270720533111,37.4843815077051,40.0249895876718,42.6488962932112,45.3561016243232,48.1466055810079,51.0204081632653,53.9775093710954,57.0179092044981,60.1416076634736,63.3486047480217,66.6389004581424,70.0124947938359,73.4693877551020,77.0095793419409,80.6330695543524,84.3398583923365,88.1299458558934,92.0033319450229,95.9600166597251,100};//{0,0.00416493127863390,0.0166597251145356,0.0374843815077051,0.0666389004581425,0.104123281965848,0.149937526030821,0.204081632653061,0.266555601832570,0.337359433569346,0.416493127863390,0.503956684714702,0.599750104123282,0.703873386089130,0.816326530612245,0.937109537692628,1.06622240733028,1.20366513952520,1.34943773427738,1.50354019158684,1.66597251145356,1.83673469387755,2.01582673885881,2.20324864639734,2.39900041649313,2.60308204914619,2.81549354435652,3.03623490212412,3.26530612244898,3.50270720533111,3.74843815077051,4.00249895876718,4.26488962932112,4.53561016243232,4.81466055810079,5.10204081632653,5.39775093710954,5.70179092044981,6.01416076634736,6.33486047480217,6.66389004581425,7.00124947938359,7.34693877551020,7.70095793419409,8.06330695543524,8.43398583923366,8.81299458558934,9.20033319450229,9.59600166597251,10,10};//{1,1.04164931278634,1.16659725114536,1.37484381507705,1.66638900458142,2.04123281965848,2.49937526030821,3.04081632653061,3.66555601832570,4.37359433569346,5.16493127863390,6.03956684714702,6.99750104123282,8.03873386089130,9.16326530612245,10.3710953769263,11.6622240733028,13.0366513952520,14.4943773427738,16.0354019158684,17.6597251145356,19.3673469387755,21.1582673885881,23.0324864639734,24.9900041649313,27.0308204914619,29.1549354435652,31.3623490212412,33.6530612244898,36.0270720533111,38.4843815077051,41.0249895876718,43.6488962932112,46.3561016243232,49.1466055810079,52.0204081632653,54.9775093710954,58.0179092044981,61.1416076634736,64.3486047480217,67.6389004581424,71.0124947938359,74.4693877551020,78.0095793419409,81.6330695543524,85.3398583923365,89.1299458558934,93.0033319450229,96.9600166597251,101,101};//{0,0.5,0.5,0.8,1.1,1.2.1.3,1.4,1.5,1.6,1.7.2,2.2,2.3.5,6,6.5,7,7.5,8,8.5,9,9.5,10,8,7,6,6,6,6.6,6.7,6.8,6.9,7.5,8,8,8,8,8.1,8.1,8,7.9,7.9,1,1,5,3,4,3,0,1,3,3,1,1,5,3,4,3,0,1,3,3,1,1,5,3,4,3,0,1,3,3,1,1,5,3,4,3,0,1,3,3,1,1,5,3,4,3,0,1,3,3,1,1,5,3,4,3,0,1,3,3};
double lux = 100.1;
double objectdist = 1.2;
bool ongraph = 0;
I2C_Handle i2c;



enum Direction{North=0, East, South, West};
enum MotorStat{Idle=0, Running, eStop};
enum AccelerationStat{slow=0,normal,fast};
int currentDirection = North;
int CurrentMotorStat = Idle;
int CurrentAcceleration = slow;






//*****************************************************************************
//
// The panel that is currently being displayed.
//
//*****************************************************************************
uint32_t g_ui32Panel;

void MotorInit(){
    enableMotor();
    initMotorLib(20, NULL);
    //updateMotor(0, 0,1);
    setDuty(75);
}
void MotorTest(){

    updateMotor(0, 1, 0);
    Task_sleep(100);


}






void TouchCheckTask(UArg arg0, UArg arg1){
    while(1){
        WidgetMessageQueueProcess();
        //Task_sleep(100);
    }
}
Void heartBeatFxn(UArg arg0, UArg arg1)
{
    if (InitI2C()){
               if( InitSensors()){

                   if (InitSensorsHWI()){
                                                        System_printf("I2C sensors running\n");
                                                    }else{
                                                        System_printf("Unable to start clock for I2C\n");
                                                    }
               } else {
                   System_printf("Sensors not configed\n");
               }
           } else{
               System_printf("Cant open I2C\n");
           }
    System_flush();

    MotorInit();
    bool LA,LB,LC;
    HA=1;
    //Timer_start(timerclock);
   //MotorInit();
    // Add the compile-time defined widgets to the widget tree.
    //WidgetAdd(WIDGET_ROOT, (tWidget *)&g_sBackground);
    //FrameDraw(&sContext, tempc);


    // Paint the widget tree to make sure they all appear on the display.
    /*WidgetAdd(WIDGET_ROOT, (tWidget *)&g_sPrevious);
    //WidgetAdd(WIDGET_ROOT, (tWidget *)&g_sTitle);
    WidgetAdd(WIDGET_ROOT, (tWidget *)&g_sNext);
    WidgetAdd(WIDGET_ROOT, (tWidget *)&slidertest);
    WidgetAdd(WIDGET_ROOT, (tWidget *)&brrrr);
    WidgetAdd(WIDGET_ROOT, (tWidget *)&brrrr2);
    WidgetPaint(&slidertest);
    WidgetPaint(&brrrr);*/
    MainPage();
    //
    // Add the first panel to the widget tree.
    //
    g_ui32Panel = 0;
   //WidgetAdd(WIDGET_ROOT, (tWidget *)g_psPanels);
    //CanvasTextSet(&g_sTitle, g_pcPanei32Names[0]);

    //
    // Issue the initial paint request to the widgets.
    //
    //WidgetPaint(WIDGET_ROOT);
    //MotorInit();
    //MotorTest();
    double s100hz[20]={10,10,9,9,9.9,10,9,9,10,9,10,10,9,9,9.9,10,9,9,10,9};
    ArrayUpdate(dd,s100hz,20);

   while (1) {
        //Task_sleep(100);
        GPIO_toggle(Board_LED0);
        //MotorTest();



        //TouchScreenIntHandler;
        Task_sleep(1000);
        //updatetime(&datetime);
        //sprintf(tempc,"%d:%d:%d %d/%d/%d \r\n",datetime.hours,datetime.minutes,datetime.seconds,datetime.days,datetime.months,datetime.year);
        //UART_write(uart, tempc, sizeof(tempc));
        //ScreenKey = GateMutexPri_enter(ScreenGate);
        TopBarDraw(&sContext,datetime, LowLight,currentDirection,CurrentMotorStat,CurrentAcceleration);
        LowLight=!LowLight;
        CurrentMotorStat= (CurrentMotorStat+1)%3;
        currentDirection = (currentDirection+1)%4;
        CurrentAcceleration=(CurrentAcceleration+1)%3;
        ArrayUpdate(dd,s100hz,20);
        //if(ongraph){
        //UpdateGraphPlot(dd,x1,x2,y1,y2,50);
        //}
        //GateMutexPri_leave(ScreenGate,ScreenKey);
    }
}

/*
 *  ======== main ========
 */
void HAF(unsigned int index){
    HA=!HA;
    System_printf("A\n");
    System_flush();
}
void HBF(unsigned int index){
    HB=!HB;
    System_printf("B\n");
    System_flush();
}
void HCF(unsigned int index){
    HC=!HC;
    System_printf("C\n");
    System_flush();
}

 int main(void)
{
    FPUEnable();
    FPULazyStackingEnable();
    Task_Params taskParams;

    /* Call board init functions */
    Board_initGeneral();
    Board_initGPIO();
    Board_initI2C();
    Board_initUART();
//    PinoutSet(false, false);
    //DrawFrame()

    /* Construct heartBeat Task  thread */
    Task_Params_init(&taskParams);
    taskParams.arg0 = 1000;
    taskParams.stackSize = TASKSTACKSIZE;
    taskParams.stack = &task0Stack;
    taskParams.priority = 4;
    Task_construct(&task0Struct, (Task_FuncPtr)heartBeatFxn, &taskParams, NULL);
    Task_Params_init(&taskParams);
        //taskParams.arg0 = 1000;
        taskParams.stackSize = TASKSTACKSIZE;
        taskParams.stack = &taskTouchStack;
        taskParams.priority = 3;
       // Task_construct(&taskTouchStruct, (Task_FuncPtr)TouchCheckTask, &taskParams, NULL);
    // Turn on user LED


    GPIO_write(Board_LED0, Board_LED_ON);
    //tContext sContext;


   // GateMutexPri_construct(&ScreenGate, &Gprams);



    //
    // Add the title block and the previous and next buttons to the widget
    // tree.
    //


    if (!initUART(&uart)){
        System_printf("EROR\n");
    }
    InitScreen();

    //FrameDraw(&sContext, tempc);
    //GPIO_setCallback(HALL_A, HAF);
   // GPIO_setCallback(HALL_B, HBF);
    //GPIO_setCallback(HALL_C, HCF);

    System_printf("Starting the example\nSystem provider is set to SysMin. "
                  "Halt the target to view any SysMin contents in ROV.\n");
    /* SysMin will only print to the console when you call flush or exit */
    System_flush();
    /* Start BIOS */
    timer1sec(&timerclock,&datetime);
    BIOS_start();

    return (0);
}

