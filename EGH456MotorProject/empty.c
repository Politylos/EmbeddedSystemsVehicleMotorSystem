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
#include <ti/sysbios/gates/GateHwi.h>
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
#include "interface/GraphData.h"
#include"Interface/GraphFunctions.h"
//#include <ti/sysbios/family/arm/msp432/Seconds.h>
#include <ti/sysbios/hal/Seconds.h>
#include "Images/GUIImg.h"
#include "Images/Images.h"
#include "Interface/GlobalDraw.h"
#include <ti/sysbios/knl/Semaphore.h>
#include <ti/sysbios/gates/GateMutex.h>
#include <ti/sysbios/gates/GateMutexPri.h>

/* Sensors */
#include "sensors/bmi160.h"
#include "sensors/opt3001.h"
#include "sensors/sensors.h"

#include "drivers/Motor/Estop.h"
#include "drivers/Motor/Motor.h"


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
bool HA = false;
bool HB = false;
bool HC = false;

/* Task handles */
Task_Struct task0Struct;
Task_Struct taskTouchStruct;
Task_Struct readAccelStruct;
Task_Struct readLightStruct;
Task_Struct readCurrentStruct;
Char task0Stack[TASKSTACKSIZE];
Char taskTouchStack[TASKSTACKSIZE];
Char readAccelStack[TASKSTACKSIZE];
Char readLightStack[TASKSTACKSIZE];
Char readCurrentStack[TASKSTACKSIZE];

char tempc[30];

/* Swi handles */


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

bool LowLight = 1;
//tCanvasWidget g_sCanvas2;
tPushButtonWidget brrrr;
bool ongraph = 0;
I2C_Handle i2c;


enum Direction
{
    North = 0, East, South, West
};
enum MotorStat
{
    Idle = 0, Running, eStop
};
enum AccelerationStat
{
    slow = 0, normal, fast
};
int currentDirection = North;
int CurrentMotorStat = Idle;
int CurrentAcceleration = slow;


//*****************************************************************************
//
// The panel that is currently being displayed.
//
//*****************************************************************************
uint32_t g_ui32Panel;

void TouchCheckTask(UArg arg0, UArg arg1)
{
    while (1)
    {
        WidgetMessageQueueProcess();
        //Task_sleep(100);
    }
}



void heartBeatFxn(UArg arg0, UArg arg1)
{
    MainPage();

    g_ui32Panel = 0;
    bool prevStop = 0;

    while (1)
    {

        Task_sleep(1000);
        EStop =checkEstop();
        TopBarDraw(&sContext, datetime, LowLight, currentDirection,
                   CurrentMotorStat, CurrentAcceleration);
        if (LightSecond < 5){
            if (LowLight == 0){
                GPIO_write(Board_LED0, Board_LED_ON);
            }
            LowLight = 1;
        }else{
            if (LowLight == 1){
                GPIO_write(Board_LED0, Board_LED_OFF);
            }
            LowLight = 0;
        }
        if(AccelSecond <15){
            CurrentAcceleration = 0;
        }else if (AccelSecond <22){
            CurrentAcceleration = 1;
        } else{
            CurrentAcceleration = 2;
        }
        if (EStop){
            GPIO_write(Board_LED1, Board_LED_ON);
            CurrentMotorStat = 2;
        } else {
            GPIO_write(Board_LED1, Board_LED_OFF);
            CurrentMotorStat = 1;
        }

        currentDirection = (currentDirection + 1) % 4;
        if (LightGraph){
            UpdateLightGraph();
        }else if (OnSensorPage){
            updateSensorPage();
        }else if (AccelGraph){
            UpdateAccelGraph();
        }
    }
}

/*
 *  ======== main ========
 */


int main(void)
{
    Error_Block testAccError;
    Error_init(&testAccError);
    Error_Block testOptError;
    Error_init(&testOptError);

    FPUEnable();
    FPULazyStackingEnable();

    /* Task parameters */
    Task_Params taskParams;

    /* Swi parameters */
    Swi_Params swiAccelSensorFilterParams;
    Swi_Params swiLightSensorFilterParams;
    Swi_Params swiCurrentSensorFilterParams;


    /* Call board init functions */
    Board_initGeneral();
    Board_initGPIO();
    Board_initI2C();
    Board_initUART();
    MotorMain();
    initEstop();
    PinoutSet(false, false);
    //DrawFrame()

    // Open i2c
    i2c_Open();

    /* Construct heartBeat Task  thread */
    Task_Params_init(&taskParams);
    taskParams.arg0 = 1000;
    taskParams.stackSize = TASKSTACKSIZE;
    taskParams.stack = &task0Stack;
    taskParams.priority = 4;
    Task_construct(&task0Struct, (Task_FuncPtr) heartBeatFxn, &taskParams,
    NULL);

    Task_Params_init(&taskParams);

    //taskParams.arg0 = 1000;
    taskParams.stackSize = TASKSTACKSIZE;
    taskParams.stack = &taskTouchStack;
    taskParams.priority = 3;
    Task_construct(&taskTouchStruct, (Task_FuncPtr) TouchCheckTask, &taskParams,
    NULL);

    /* Task for accel reading */
    Task_Params_init(&taskParams);
    taskParams.stackSize = TASKSTACKSIZE;
    taskParams.stack = &readAccelStack;
    taskParams.priority = 10;
    Task_construct(&readAccelStruct, (Task_FuncPtr) readAccFxn, &taskParams,
                   &testAccError);

    /* Task for light reading */
    Task_Params_init(&taskParams);
    taskParams.stackSize = TASKSTACKSIZE;
    taskParams.stack = &readLightStack;
    taskParams.priority = 10;
    Task_construct(&readLightStruct, (Task_FuncPtr) readOptFxn, &taskParams,
                   &testOptError);

    /* Task for current reading */
    Task_Params_init(&taskParams);
    taskParams.stackSize = TASKSTACKSIZE;
    taskParams.stack = &readCurrentStack;
    taskParams.priority = 10;
    Task_construct(&readCurrentStruct, (Task_FuncPtr) readCurrentFxn,
                   &taskParams, NULL);

    /* Swi for accel sensor filters */
    Swi_Params_init(&swiAccelSensorFilterParams);
    swiAccelSensorFilterParams.priority = 12;
    swiAccelSensorFilterHandle = Swi_create((Task_FuncPtr) accelSensorFilterFxn,
                                            &swiAccelSensorFilterParams, NULL);

    /* Swi for light sensor filters */
    Swi_Params_init(&swiLightSensorFilterParams);
    swiLightSensorFilterParams.priority = 12;
    swiLightSensorFilterHandle = Swi_create((Task_FuncPtr) lightSensorFilterFxn,
                                            &swiLightSensorFilterParams, NULL);

    /* Swi for current sensor filters */
    Swi_Params_init(&swiCurrentSensorFilterParams);
    swiCurrentSensorFilterParams.priority = 12;
    swiCurrentSensorFilterHandle = Swi_create(
            (Task_FuncPtr) currentSensorFilterFxn, &swiCurrentSensorFilterParams,
            NULL);

    // Turn on user LED

    //tContext sContext;


    // GateMutexPri_construct(&ScreenGate, &Gprams);

    //
    // Add the title block and the previous and next buttons to the widget
    // tree.
    //

    if (!initUART(&uart))
    {
        System_printf("EROR\n");
    }
    initGraphBuffers();
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
    timer1sec(&timerclock, &datetime);
    BIOS_start();

    return (0);
}

