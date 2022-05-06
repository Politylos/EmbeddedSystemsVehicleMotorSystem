/*
 * GUIMain.c
 *
 *  Created on: 5 May 2022
 *      Author: polit
 */

#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>

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
#include "Clock.h"
#include "GUIMain.h"

extern tCanvasWidget Pages[];
char disptime[30];

char *g_pcPanei32Names[] =
{
    "     Main Page     "
};

void OnPrevious(tWidget *psWidget);
void OnNext(tWidget *psWidget);

void MainPage(tWidget *psWidget, tContext *psContext);
void MotorPage(tWidget *psWidget, tContext *psContext);
void SensorsPage(tWidget *psWidget, tContext *psContext);
void GraphingPage(tWidget *psWidget, uint32_t bSelected);

void OnButtonPress(tWidget *psWidget);
void OnRadioChange(tWidget *psWidget, uint32_t bSelected);
void OnSliderChange(tWidget *psWidget, int32_t i32Value);

Canvas(pageMain,Pages , 0, 0, &g_sKentec320x240x16_SSD2119, 0, 24,
       320, 166, CANVAS_STYLE_APP_DRAWN, 0, 0, 0, 0, 0, 0, MainPage);

Canvas(pageMotor,Pages , 0, 0, &g_sKentec320x240x16_SSD2119, 0, 24,
       320, 166, CANVAS_STYLE_APP_DRAWN, 0, 0, 0, 0, 0, 0, MotorPage);

tCanvasWidget Pages[] =
{
    CanvasStruct(0, 0, &pageMain, &g_sKentec320x240x16_SSD2119, 0, 24,
                 320, 166, CANVAS_STYLE_FILL, ClrBlack, 0, 0, 0, 0, 0, 0),
    CanvasStruct(0, 0, &pageMotor, &g_sKentec320x240x16_SSD2119, 0, 24,
                  320, 166, CANVAS_STYLE_FILL, ClrBlack, 0, 0, 0, 0, 0, 0)
};
//#define NUM_PANELS              (sizeof(Pages) / sizeof(Pages[0]))


void updatetimeDisp(FT time){
    sprintf(disptime,"%d:%d:%d %d/%d/%d",time.hours,time.minutes,time.seconds,time.days,time.months,time.year);
}
void MainPage(tWidget *psWidget, tContext *psContext){
    GrContextFontSet(psContext, &g_sFontCm18);
    GrContextForegroundSet(psContext, ClrSilver);
    GrStringDraw(psContext, disptime, -1,
                 0, 32, 0);
}
void MotorPage(tWidget *psWidget, tContext *psContext){
    GrContextFontSet(psContext, &g_sFontCm18);
    GrContextForegroundSet(psContext, ClrSilver);
    GrStringDraw(psContext, disptime, -1,
                 0, 32, 0);
}

void InitScreen(tContext *screen){
    Kentec320x240x16_SSD2119Init(120000000);
    GrContextInit(screen, &g_sKentec320x240x16_SSD2119);
    TouchScreenInit(120000000);
    FrameDraw(screen, disptime);
    //g_ui32Panel = 0;
    //WidgetAdd(WIDGET_ROOT, (tWidget *)Pages);
    //GrContextFontSet(screen, &g_sFontCm18);
    //GrContextForegroundSet(screen, ClrSilver);
    //GrStringDraw(screen, "AAAAAA", -1, 0, 32, 0);
    //CanvasTextSet(&g_sTitle, g_pcPanei32Names[0]);

    //
    // Issue the initial paint request to the widgets.
    //
    //WidgetPaint(WIDGET_ROOT);
}



