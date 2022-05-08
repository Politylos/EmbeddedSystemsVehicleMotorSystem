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
//#include <ti/sysbios/family/arm/msp432/Seconds.h>
#include <ti/sysbios/hal/Seconds.h>
#include "Images/GUIImg.h"
#include "Images/Images.h"
#include "Interface/GlobalDraw.h"


#define TASKSTACKSIZE   1024

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
tContext sContext;


uint8_t motorStartStop = 1;
UART_Handle uart;


FT datetime;

Timer_Handle timerclock;
bool HA=false;
bool HB=false;
bool HC=false;

Task_Struct task0Struct;
Char task0Stack[TASKSTACKSIZE];
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
tCanvasWidget g_sCanvas2;
tPushButtonWidget brrrr ;




//*****************************************************************************
//
// The first panel, which contains introductory text explaining the
// application.
//
//*****************************************************************************
Canvas(g_sIntroduction, g_psPanels, 0, 0, &g_sKentec320x240x16_SSD2119, 0, 24,
       320, 166, CANVAS_STYLE_APP_DRAWN, 0, 0, 0, 0, 0, 0, OnIntroPaint);

//*****************************************************************************
//
// The second panel, which demonstrates the graphics primitives.
//
//*****************************************************************************
Canvas(g_sPrimitives, g_psPanels + 1, 0, 0, &g_sKentec320x240x16_SSD2119, 0,
       24, 320, 166, CANVAS_STYLE_APP_DRAWN, 0, 0, 0, 0, 0, 0,
       OnPrimitivePaint);

//*****************************************************************************
//
// The third panel, which demonstrates the canvas widget.
//
//*****************************************************************************
Canvas(g_sCanvas3, g_psPanels + 2, 0, 0, &g_sKentec320x240x16_SSD2119, 205,
       27, 110, 158, CANVAS_STYLE_OUTLINE | CANVAS_STYLE_APP_DRAWN, 0, ClrGray,
       0, 0, 0, 0, OnCanvasPaint);
Canvas(g_sCanvas2, g_psPanels + 2, &g_sCanvas3, 0,
       &g_sKentec320x240x16_SSD2119, 292, 0, 25, 25,
       CANVAS_STYLE_IMG, 0, 0, 0, 0, 0, g_pui8Logo,
       0);
Canvas(g_sCanvas1, g_psPanels + 2, &g_sCanvas2, 0,
       &g_sKentec320x240x16_SSD2119, 5, 27, 195, 76,
       CANVAS_STYLE_FILL | CANVAS_STYLE_OUTLINE | CANVAS_STYLE_TEXT,
       ClrMidnightBlue, ClrGray, ClrSilver, &g_sFontCm22, "Text", 0, 0);

//*****************************************************************************
//
// The fourth panel, which demonstrates the checkbox widget.
//
//*****************************************************************************
tCanvasWidget g_psCheckBoxIndicators[] =
{
    CanvasStruct(g_psPanels + 3, g_psCheckBoxIndicators + 1, 0,
                 &g_sKentec320x240x16_SSD2119, 230, 30, 50, 42,
                 CANVAS_STYLE_IMG, 0, 0, 0, 0, 0, g_pui8LightOff, 0),
    CanvasStruct(g_psPanels + 3, g_psCheckBoxIndicators + 2, 0,
                 &g_sKentec320x240x16_SSD2119, 230, 82, 50, 48,
                 CANVAS_STYLE_IMG, 0, 0, 0, 0, 0, g_pui8LightOff, 0),
    CanvasStruct(g_psPanels + 3, 0, 0,
                 &g_sKentec320x240x16_SSD2119, 230, 134, 50, 42,
                 CANVAS_STYLE_IMG, 0, 0, 0, 0, 0, g_pui8LightOff, 0)
};
tCheckBoxWidget g_psCheckBoxes[] =
{
    CheckBoxStruct(g_psPanels + 3, g_psCheckBoxes + 1, 0,
                   &g_sKentec320x240x16_SSD2119, 40, 30, 185, 42,
                   CB_STYLE_OUTLINE | CB_STYLE_FILL | CB_STYLE_TEXT, 16,
                   ClrMidnightBlue, ClrGray, ClrSilver, &g_sFontCm22, "Select",
                   0, OnCheckChange),
    CheckBoxStruct(g_psPanels + 3, g_psCheckBoxes + 2, 0,
                   &g_sKentec320x240x16_SSD2119, 40, 82, 185, 48,
                   CB_STYLE_IMG, 16, 0, ClrGray, 0, 0, 0, g_pui8Logo,
                   OnCheckChange),
    CheckBoxStruct(g_psPanels + 3, g_psCheckBoxIndicators, 0,
                   &g_sKentec320x240x16_SSD2119, 40, 134, 189, 42,
                   CB_STYLE_OUTLINE | CB_STYLE_TEXT, 16,
                   0, ClrGray, ClrGreen, &g_sFontCm20, "Select",
                   0, OnCheckChange),
};
#define NUM_CHECK_BOXES         (sizeof(g_psCheckBoxes) /   \
                                 sizeof(g_psCheckBoxes[0]))

//*****************************************************************************
//
// The fifth panel, which demonstrates the container widget.
//
//*****************************************************************************
Container(g_sContainer3, g_psPanels + 4, 0, 0, &g_sKentec320x240x16_SSD2119,
          210, 47, 105, 118, CTR_STYLE_OUTLINE | CTR_STYLE_FILL,
          ClrMidnightBlue, ClrGray, 0, 0, 0);
Container(g_sContainer2, g_psPanels + 4, &g_sContainer3, 0,
          &g_sKentec320x240x16_SSD2119, 5, 109, 200, 76,
          (CTR_STYLE_OUTLINE | CTR_STYLE_FILL | CTR_STYLE_TEXT |
           CTR_STYLE_TEXT_CENTER), ClrMidnightBlue, ClrGray, ClrSilver,
          &g_sFontCm22, "Group2");
Container(g_sContainer1, g_psPanels + 4, &g_sContainer2, 0,
          &g_sKentec320x240x16_SSD2119, 5, 27, 200, 76,
          CTR_STYLE_OUTLINE | CTR_STYLE_FILL | CTR_STYLE_TEXT, ClrMidnightBlue,
          ClrGray, ClrSilver, &g_sFontCm22, "Group1");

//*****************************************************************************
//
// The sixth panel, which contains a selection of push buttons.
//
//*****************************************************************************
void btntest(){
    int ahhhhhh=1;
}
tSliderWidget slidertest = SliderStruct(g_psPanels+5, 0, 0,
             &g_sKentec320x240x16_SSD2119, 5, 30, 220, 30, 0, 100, 25,
             (SL_STYLE_FILL | SL_STYLE_BACKG_FILL | SL_STYLE_OUTLINE |
              SL_STYLE_TEXT | SL_STYLE_BACKG_TEXT),
             ClrGray, ClrBlack, ClrSilver, ClrWhite, ClrWhite,
             &g_sFontCm20, "25%", 0, 0, OnSliderChange);
tPushButtonWidget brrrr = RectangularButtonStruct(g_psPanels + 5, 0, 0,
                                                  &g_sKentec320x240x16_SSD2119, 130, 200, 50, 50,
                                                  PB_STYLE_IMG | PB_STYLE_TEXT, 0, 0, 0, ClrSilver,
                                                  &g_sFontCm22, "5", g_pui8Blue50x50,
                                                  g_pui8Blue50x50Press, 0, 0, btntest);
tPushButtonWidget brrrr2 =RectangularButtonStruct(g_psPanels + 5, 0, 0,
                        &g_sKentec320x240x16_SSD2119, 30, 35, 40, 40,
                        PB_STYLE_FILL | PB_STYLE_OUTLINE | PB_STYLE_TEXT,
                        ClrMidnightBlue, ClrBlack, ClrGray, ClrSilver,
                        &g_sFontCm22, "1", 0, 0, 0, 0, btntest);
tCanvasWidget sidertestC[] = {

                                                      CanvasStruct(g_psPanels + 5,sidertestC+3, &brrrr, &g_sKentec320x240x16_SSD2119, 0,
                                                                   85, 50, 50, CANVAS_STYLE_FILL, ClrBlack, 0, 0, 0, 0, 0, 0),
                                                                   CanvasStruct(g_psPanels + 5,sidertestC+4, &brrrr2, &g_sKentec320x240x16_SSD2119, 0,
                                                                                85, 50, 50, CANVAS_STYLE_FILL, ClrBlack, 0, 0, 0, 0, 0, 0),
                              CanvasStruct(g_psPanels + 5, sidertestC+1, 0,
                                           &g_sKentec320x240x16_SSD2119, 0,60, 200, 80,
                                           CANVAS_STYLE_TEXT, 0, 0, ClrSilver, &g_sFontCm12, "MOTOR GO BRRRRR",
                                           0, 0),
                              CanvasStruct(g_psPanels + 5,sidertestC+2, &slidertest, &g_sKentec320x240x16_SSD2119, 0,
                                           65, 220, 30, CANVAS_STYLE_FILL, ClrBlack, 0, 0, 0, 0, 0, 0),




};

tCanvasWidget g_psPushButtonIndicators[] =
{
    CanvasStruct(g_psPanels + 5, g_psPushButtonIndicators + 1, 0,
                 &g_sKentec320x240x16_SSD2119, 40, 85, 20, 20,
                 CANVAS_STYLE_IMG, 0, 0, 0, 0, 0, g_pui8LightOff, 0),
    CanvasStruct(g_psPanels + 5, g_psPushButtonIndicators + 2, 0,
                 &g_sKentec320x240x16_SSD2119, 90, 85, 20, 20,
                 CANVAS_STYLE_IMG, 0, 0, 0, 0, 0, g_pui8LightOff, 0),
    CanvasStruct(g_psPanels + 5, g_psPushButtonIndicators + 3, 0,
                 &g_sKentec320x240x16_SSD2119, 145, 85, 20, 20,
                 CANVAS_STYLE_IMG, 0, 0, 0, 0, 0, g_pui8LightOff, 0),
    CanvasStruct(g_psPanels + 5, g_psPushButtonIndicators + 4, 0,
                 &g_sKentec320x240x16_SSD2119, 40, 165, 20, 20,
                 CANVAS_STYLE_IMG, 0, 0, 0, 0, 0, g_pui8LightOff, 0),
    CanvasStruct(g_psPanels + 5, g_psPushButtonIndicators + 5, 0,
                 &g_sKentec320x240x16_SSD2119, 90, 165, 20, 20,
                 CANVAS_STYLE_IMG, 0, 0, 0, 0, 0, g_pui8LightOff, 0),
    CanvasStruct(g_psPanels + 5, g_psPushButtonIndicators + 6, 0,
                 &g_sKentec320x240x16_SSD2119, 145, 165, 20, 20,
                 CANVAS_STYLE_IMG, 0, 0, 0, 0, 0, g_pui8LightOff, 0),
    CanvasStruct(g_psPanels + 5, g_psPushButtonIndicators + 7, 0,
                 &g_sKentec320x240x16_SSD2119, 190, 35, 110, 24,
                 CANVAS_STYLE_TEXT, 0, 0, ClrSilver, &g_sFontCm20, "Non-auto",
                 0, 0),
    CanvasStruct(g_psPanels + 5, g_psPushButtonIndicators + 8, 0,
                 &g_sKentec320x240x16_SSD2119, 190, 55, 110, 24,
                 CANVAS_STYLE_TEXT, 0, 0, ClrSilver, &g_sFontCm20, "repeat",
                 0, 0),
    CanvasStruct(g_psPanels + 5, g_psPushButtonIndicators + 9, 0,
                 &g_sKentec320x240x16_SSD2119, 190, 115, 110, 24,
                 CANVAS_STYLE_TEXT, 0, 0, ClrSilver, &g_sFontCm20, "Auto",
                 0, 0),
                 CanvasStruct(g_psPanels + 5, 0, 0,
                              &g_sKentec320x240x16_SSD2119, 190, 135, 110, 24,
                              CANVAS_STYLE_TEXT, 0, 0, ClrSilver, &g_sFontCm20, "repeat",
                              0, 0),
                 CanvasStruct(g_psPanels + 5, 0, &slidertest,
                              &g_sKentec320x240x16_SSD2119, 5, 115, 220, 30,
                              CANVAS_STYLE_TEXT, 0, 0, ClrSilver, 0, 0,
                              0, 0),
};

tPushButtonWidget g_psPushButtons[] =
{
    RectangularButtonStruct(g_psPanels + 5, g_psPushButtons + 1, 0,
                            &g_sKentec320x240x16_SSD2119, 30, 35, 40, 40,
                            PB_STYLE_FILL | PB_STYLE_OUTLINE | PB_STYLE_TEXT,
                            ClrMidnightBlue, ClrBlack, ClrGray, ClrSilver,
                            &g_sFontCm22, "1", 0, 0, 0, 0, OnButtonPress),
    CircularButtonStruct(g_psPanels + 5, g_psPushButtons + 2, 0,
                         &g_sKentec320x240x16_SSD2119, 100, 55, 20,
                         PB_STYLE_FILL | PB_STYLE_OUTLINE | PB_STYLE_TEXT,
                         ClrMidnightBlue, ClrBlack, ClrGray, ClrSilver,
                         &g_sFontCm22, "3", 0, 0, 0, 0, OnButtonPress),
    RectangularButtonStruct(g_psPanels + 5, g_psPushButtons + 3, 0,
                            &g_sKentec320x240x16_SSD2119, 130, 30, 50, 50,
                            PB_STYLE_IMG | PB_STYLE_TEXT, 0, 0, 0, ClrSilver,
                            &g_sFontCm22, "5", g_pui8Blue50x50,
                            g_pui8Blue50x50Press, 0, 0, OnButtonPress),
    RectangularButtonStruct(g_psPanels + 5, g_psPushButtons + 4, 0,
                            &g_sKentec320x240x16_SSD2119, 30, 115, 40, 40,
                            (PB_STYLE_FILL | PB_STYLE_OUTLINE | PB_STYLE_TEXT |
                             PB_STYLE_AUTO_REPEAT), ClrMidnightBlue, ClrBlack,
                            ClrGray, ClrSilver, &g_sFontCm22, "2", 0, 0, 125,
                            25, OnButtonPress),
    CircularButtonStruct(g_psPanels + 5, g_psPushButtons + 5, 0,
                         &g_sKentec320x240x16_SSD2119, 100, 135, 20,
                         (PB_STYLE_FILL | PB_STYLE_OUTLINE | PB_STYLE_TEXT |
                          PB_STYLE_AUTO_REPEAT), ClrMidnightBlue, ClrBlack,
                         ClrGray, ClrSilver, &g_sFontCm22, "4", 0, 0, 125, 25,
                         OnButtonPress),
    RectangularButtonStruct(g_psPanels + 5, g_psPushButtonIndicators, 0,
                            &g_sKentec320x240x16_SSD2119, 130, 110, 50, 50,
                            (PB_STYLE_IMG | PB_STYLE_TEXT |
                             PB_STYLE_AUTO_REPEAT), 0, 0, 0, ClrSilver,
                            &g_sFontCm22, "6", g_pui8Blue50x50,
                            g_pui8Blue50x50Press, 125, 25, OnButtonPress),
};
#define NUM_PUSH_BUTTONS        (sizeof(g_psPushButtons) /   \
                                 sizeof(g_psPushButtons[0]))
uint32_t g_ui32ButtonState;

//*****************************************************************************
//
// The seventh panel, which contains a selection of radio buttons.
//
//*****************************************************************************
tContainerWidget g_psRadioContainers[];
tCanvasWidget g_psRadioButtonIndicators[] =
{
    CanvasStruct(g_psRadioContainers, g_psRadioButtonIndicators + 1, 0,
                 &g_sKentec320x240x16_SSD2119, 95, 62, 20, 20,
                 CANVAS_STYLE_IMG, 0, 0, 0, 0, 0, g_pui8LightOff, 0),
    CanvasStruct(g_psRadioContainers, g_psRadioButtonIndicators + 2, 0,
                 &g_sKentec320x240x16_SSD2119, 95, 107, 20, 20,
                 CANVAS_STYLE_IMG, 0, 0, 0, 0, 0, g_pui8LightOff, 0),
    CanvasStruct(g_psRadioContainers, 0, 0,
                 &g_sKentec320x240x16_SSD2119, 95, 152, 20, 20,
                 CANVAS_STYLE_IMG, 0, 0, 0, 0, 0, g_pui8LightOff, 0),
    CanvasStruct(g_psRadioContainers + 1, g_psRadioButtonIndicators + 4, 0,
                 &g_sKentec320x240x16_SSD2119, 260, 62, 20, 20,
                 CANVAS_STYLE_IMG, 0, 0, 0, 0, 0, g_pui8LightOff, 0),
    CanvasStruct(g_psRadioContainers + 1, g_psRadioButtonIndicators + 5, 0,
                 &g_sKentec320x240x16_SSD2119, 260, 107, 20, 20,
                 CANVAS_STYLE_IMG, 0, 0, 0, 0, 0, g_pui8LightOff, 0),
    CanvasStruct(g_psRadioContainers + 1, 0, 0,
                 &g_sKentec320x240x16_SSD2119, 260, 152, 20, 20,
                 CANVAS_STYLE_IMG, 0, 0, 0, 0, 0, g_pui8LightOff, 0),
};
tRadioButtonWidget g_psRadioButtons1[] =
{
    RadioButtonStruct(g_psRadioContainers, g_psRadioButtons1 + 1, 0,
                      &g_sKentec320x240x16_SSD2119, 10, 50, 80, 45,
                      RB_STYLE_TEXT, 16, 0, ClrSilver, ClrSilver, &g_sFontCm20,
                      "One", 0, OnRadioChange),
    RadioButtonStruct(g_psRadioContainers, g_psRadioButtons1 + 2, 0,
                      &g_sKentec320x240x16_SSD2119, 10, 95, 80, 45,
                      RB_STYLE_TEXT, 16, 0, ClrSilver, ClrSilver, &g_sFontCm20,
                      "Two", 0, OnRadioChange),
    RadioButtonStruct(g_psRadioContainers, g_psRadioButtonIndicators, 0,
                      &g_sKentec320x240x16_SSD2119, 10, 140, 80, 45,
                      RB_STYLE_TEXT, 24, 0, ClrSilver, ClrSilver, &g_sFontCm20,
                      "Three", 0, OnRadioChange)
};
#define NUM_RADIO1_BUTTONS      (sizeof(g_psRadioButtons1) /   \
                                 sizeof(g_psRadioButtons1[0]))
tRadioButtonWidget g_psRadioButtons2[] =
{
    RadioButtonStruct(g_psRadioContainers + 1, g_psRadioButtons2 + 1, 0,
                      &g_sKentec320x240x16_SSD2119, 175, 50, 80, 45,
                      RB_STYLE_IMG, 16, 0, ClrSilver, 0, 0, 0, g_pui8Logo,
                      OnRadioChange),
    RadioButtonStruct(g_psRadioContainers + 1, g_psRadioButtons2 + 2, 0,
                      &g_sKentec320x240x16_SSD2119, 175, 95, 80, 45,
                      RB_STYLE_IMG, 24, 0, ClrSilver, 0, 0, 0, g_pui8Logo,
                      OnRadioChange),
    RadioButtonStruct(g_psRadioContainers + 1, g_psRadioButtonIndicators + 3,
                      0, &g_sKentec320x240x16_SSD2119, 175, 140, 80, 45,
                      RB_STYLE_IMG, 24, 0, ClrSilver, 0, 0, 0, g_pui8Logo,
                      OnRadioChange)
};
#define NUM_RADIO2_BUTTONS      (sizeof(g_psRadioButtons2) /   \
                                 sizeof(g_psRadioButtons2[0]))
tContainerWidget g_psRadioContainers[] =
{
    ContainerStruct(g_psPanels + 6, g_psRadioContainers + 1, g_psRadioButtons1,
                    &g_sKentec320x240x16_SSD2119, 5, 27, 148, 160,
                    CTR_STYLE_OUTLINE | CTR_STYLE_TEXT, 0, ClrGray, ClrSilver,
                    &g_sFontCm20, "Group One"),
    ContainerStruct(g_psPanels + 6, 0, g_psRadioButtons2,
                    &g_sKentec320x240x16_SSD2119, 167, 27, 148, 160,
                    CTR_STYLE_OUTLINE | CTR_STYLE_TEXT, 0, ClrGray, ClrSilver,
                    &g_sFontCm20, "Group Two")
};

//*****************************************************************************
//
// The eighth panel, which demonstrates the slider widget.
//
//*****************************************************************************
Canvas(g_sSliderValueCanvas, g_psPanels + 7, 0, 0,
       &g_sKentec320x240x16_SSD2119, 210, 30, 60, 40,
       CANVAS_STYLE_TEXT | CANVAS_STYLE_TEXT_OPAQUE, ClrBlack, 0, ClrSilver,
       &g_sFontCm24, "50%",
       0, 0);

tSliderWidget g_psSliders[] =
{
    SliderStruct(g_psPanels + 7, g_psSliders + 1, 0,
                 &g_sKentec320x240x16_SSD2119, 5, 115, 220, 30, 0, 100, 25,
                 (SL_STYLE_FILL | SL_STYLE_BACKG_FILL | SL_STYLE_OUTLINE |
                  SL_STYLE_TEXT | SL_STYLE_BACKG_TEXT),
                 ClrGray, ClrBlack, ClrSilver, ClrWhite, ClrWhite,
                 &g_sFontCm20, "25%", 0, 0, OnSliderChange),
    SliderStruct(g_psPanels + 7, g_psSliders + 2, 0,
                 &g_sKentec320x240x16_SSD2119, 5, 155, 220, 25, 0, 100, 25,
                 (SL_STYLE_FILL | SL_STYLE_BACKG_FILL | SL_STYLE_OUTLINE |
                  SL_STYLE_TEXT),
                 ClrWhite, ClrBlueViolet, ClrSilver, ClrBlack, 0,
                 &g_sFontCm18, "Foreground Text Only", 0, 0, OnSliderChange),
    SliderStruct(g_psPanels + 7, g_psSliders + 3, 0,
                 &g_sKentec320x240x16_SSD2119, 240, 70, 26, 110, 0, 100, 50,
                 (SL_STYLE_FILL | SL_STYLE_BACKG_FILL | SL_STYLE_VERTICAL |
                  SL_STYLE_OUTLINE | SL_STYLE_LOCKED), ClrDarkGreen,
                  ClrDarkRed, ClrSilver, 0, 0, 0, 0, 0, 0, 0),
    SliderStruct(g_psPanels + 7, g_psSliders + 4, 0,
                 &g_sKentec320x240x16_SSD2119, 280, 30, 30, 150, 0, 100, 75,
                 (SL_STYLE_IMG | SL_STYLE_BACKG_IMG | SL_STYLE_VERTICAL |
                 SL_STYLE_OUTLINE), 0, ClrBlack, ClrSilver, 0, 0, 0,
                 0, g_pui8GettingHotter28x148, g_pui8GettingHotter28x148Mono,
                 OnSliderChange),
    SliderStruct(g_psPanels + 7, g_psSliders + 5, 0,
                 &g_sKentec320x240x16_SSD2119, 5, 30, 195, 37, 0, 100, 50,
                 SL_STYLE_IMG | SL_STYLE_BACKG_IMG, 0, 0, 0, 0, 0, 0,
                 0, g_pui8GreenSlider195x37, g_pui8RedSlider195x37,
                 OnSliderChange),
    SliderStruct(g_psPanels + 7, &g_sSliderValueCanvas, 0,
                 &g_sKentec320x240x16_SSD2119, 5, 80, 220, 25, 0, 100, 50,
                 (SL_STYLE_FILL | SL_STYLE_BACKG_FILL | SL_STYLE_TEXT |
                  SL_STYLE_BACKG_TEXT | SL_STYLE_TEXT_OPAQUE |
                  SL_STYLE_BACKG_TEXT_OPAQUE),
                 ClrBlue, ClrYellow, ClrSilver, ClrYellow, ClrBlue,
                 &g_sFontCm18, "Text in both areas", 0, 0,
                 OnSliderChange),
};

#define SLIDER_TEXT_VAL_INDEX   0
#define SLIDER_LOCKED_INDEX     2
#define SLIDER_CANVAS_VAL_INDEX 4

#define NUM_SLIDERS (sizeof(g_psSliders) / sizeof(g_psSliders[0]))

//*****************************************************************************
//
// An array of canvas widgets, one per panel.  Each canvas is filled with
// black, overwriting the contents of the previous panel.
//
//*****************************************************************************
tCanvasWidget g_psPanels[] =
{
    CanvasStruct(0, 0, &g_sIntroduction, &g_sKentec320x240x16_SSD2119, 0, 24,
                 320, 166, CANVAS_STYLE_FILL, ClrBlack, 0, 0, 0, 0, 0, 0),
    CanvasStruct(0, 0, &g_sPrimitives, &g_sKentec320x240x16_SSD2119, 0, 24,
                 320, 166, CANVAS_STYLE_FILL, ClrBlack, 0, 0, 0, 0, 0, 0),
    CanvasStruct(0, 0, &g_sCanvas1, &g_sKentec320x240x16_SSD2119, 0, 24, 320,
                 166, CANVAS_STYLE_FILL, ClrBlack, 0, 0, 0, 0, 0, 0),
    CanvasStruct(0, 0, g_psCheckBoxes, &g_sKentec320x240x16_SSD2119, 0, 24,
                 320, 166, CANVAS_STYLE_FILL, ClrBlack, 0, 0, 0, 0, 0, 0),
    CanvasStruct(0, 0, &g_sContainer1, &g_sKentec320x240x16_SSD2119, 0, 24,
                 320, 166, CANVAS_STYLE_FILL, ClrBlack, 0, 0, 0, 0, 0, 0),
    CanvasStruct(0, 0, &sidertestC, &g_sKentec320x240x16_SSD2119, 0, 24,
                 320, 166, CANVAS_STYLE_FILL, ClrBlack, 0, 0, 0, 0, 0, 0),
    CanvasStruct(0, 0, g_psRadioContainers, &g_sKentec320x240x16_SSD2119, 0,
                 24, 320, 166, CANVAS_STYLE_FILL, ClrBlack, 0, 0, 0, 0, 0, 0),
    CanvasStruct(0, 0, g_psSliders, &g_sKentec320x240x16_SSD2119, 0,
                 24, 320, 166, CANVAS_STYLE_FILL, ClrBlack, 0, 0, 0, 0, 0, 0),
};

//*****************************************************************************
//
// The number of panels.
//
//*****************************************************************************
#define NUM_PANELS              (sizeof(g_psPanels) / sizeof(g_psPanels[0]))

//*****************************************************************************
//
// The names for each of the panels, which is displayed at the bottom of the
// screen.
//
//*****************************************************************************
char *g_pcPanei32Names[] =
{
    "     Introduction     ",
    "     Primitives     ",
    "     Canvas     ",
    "     Checkbox     ",
    "     Container     ",
    "     Push Buttons     ",
    "     Radio Buttons     ",
    "     Sliders     ",
    "     S/W Update    "
};

//*****************************************************************************
//
// The buttons and text across the bottom of the screen.
//
//*****************************************************************************
RectangularButton(g_sPrevious, 0, 0, 0, &g_sKentec320x240x16_SSD2119, 0, 190,
                  50, 50, PB_STYLE_FILL, ClrBlack, ClrBlack, 0, ClrSilver,
                  &g_sFontCm20, "-", g_pui8Blue50x50, g_pui8Blue50x50Press, 0, 0,
                  OnPrevious);

/*Canvas(g_sTitle, 0, 0, 0, &g_sKentec320x240x16_SSD2119, 50, 190, 220, 50,
       CANVAS_STYLE_TEXT | CANVAS_STYLE_TEXT_OPAQUE, 0, 0, ClrSilver,
       &g_sFontCm20, 0, 0, 0);*/

RectangularButton(g_sNext, 0, 0, 0, &g_sKentec320x240x16_SSD2119, 270, 190,
                  50, 50, PB_STYLE_IMG | PB_STYLE_TEXT, ClrBlack, ClrBlack, 0,
                  ClrSilver, &g_sFontCm20, "+", g_pui8Blue50x50,
                  g_pui8Blue50x50Press, 0, 0, OnNext);

//*****************************************************************************
//
// The panel that is currently being displayed.
//
//*****************************************************************************
uint32_t g_ui32Panel;

//*****************************************************************************
//
// Handles presses of the previous panel button.
//
//*****************************************************************************
void
OnPrevious(tWidget *psWidget)
{
    //
    // There is nothing to be done if the first panel is already being
    // displayed.
    //
    if(g_ui32Panel == 0)
    {
        return;
    }

    //
    // Remove the current panel.
    //
    WidgetRemove((tWidget *)(g_psPanels + g_ui32Panel));

    //
    // Decrement the panel index.
    //
    g_ui32Panel--;

    //
    // Add and draw the new panel.
    //
    WidgetAdd(WIDGET_ROOT, (tWidget *)(g_psPanels + g_ui32Panel));
    WidgetPaint((tWidget *)(g_psPanels + g_ui32Panel));

    //
    // Set the title of this panel.
    //
    //CanvasTextSet(&g_sTitle, g_pcPanei32Names[g_ui32Panel]);
    //WidgetPaint((tWidget *)&g_sTitle);

    //
    // See if this is the first panel.
    //
    if(g_ui32Panel == 0)
    {
        //
        // Clear the previous button from the display since the first panel is
        // being displayed.
        //
        PushButtonImageOff(&g_sPrevious);
        PushButtonTextOff(&g_sPrevious);
        PushButtonFillOn(&g_sPrevious);
        WidgetPaint((tWidget *)&g_sPrevious);
    }

    //
    // See if the previous panel was the last panel.
    //
    if(g_ui32Panel == (NUM_PANELS - 2))
    {
        //
        // Display the next button.
        //
        PushButtonImageOn(&g_sNext);
        PushButtonTextOn(&g_sNext);
        PushButtonFillOff(&g_sNext);
        WidgetPaint((tWidget *)&g_sNext);
    }

}

//*****************************************************************************
//
// Handles presses of the next panel button.
//
//*****************************************************************************
void
OnNext(tWidget *psWidget)
{
    //
    // There is nothing to be done if the last panel is already being
    // displayed.
    //
    if(g_ui32Panel == (NUM_PANELS - 1))
    {
        return;
    }

    //
    // Remove the current panel.
    //
    WidgetRemove((tWidget *)(g_psPanels + g_ui32Panel));

    //
    // Increment the panel index.
    //
    g_ui32Panel++;

    //
    // Add and draw the new panel.
    //
    WidgetAdd(WIDGET_ROOT, (tWidget *)(g_psPanels + g_ui32Panel));
    WidgetPaint((tWidget *)(g_psPanels + g_ui32Panel));

    //
    // Set the title of this panel.
    //
    //CanvasTextSet(&g_sTitle, g_pcPanei32Names[g_ui32Panel]);
    //WidgetPaint((tWidget *)&g_sTitle);

    //
    // See if the previous panel was the first panel.
    //
    if(g_ui32Panel == 1)
    {
        //
        // Display the previous button.
        //
        PushButtonImageOn(&g_sPrevious);
        PushButtonTextOn(&g_sPrevious);
        PushButtonFillOff(&g_sPrevious);
        WidgetPaint((tWidget *)&g_sPrevious);
    }

    //
    // See if this is the last panel.
    //
    if(g_ui32Panel == (NUM_PANELS - 1))
    {
        //
        // Clear the next button from the display since the last panel is being
        // displayed.
        //
        PushButtonImageOff(&g_sNext);
        PushButtonTextOff(&g_sNext);
        PushButtonFillOn(&g_sNext);
        WidgetPaint((tWidget *)&g_sNext);
    }

}

//*****************************************************************************
//
// Handles paint requests for the introduction canvas widget.
//
//*****************************************************************************
void
OnIntroPaint(tWidget *psWidget, tContext *psContext)
{
    //
    // Display the introduction text in the canvas.
    //
    GrContextFontSet(psContext, &g_sFontCm18);
    GrContextForegroundSet(psContext, ClrSilver);
    GrStringDraw(psContext, "This application demonstrates the", -1,
                 0, 32, 0);
    GrStringDraw(psContext, "TivaWare Graphics Library.", -1, 0, 50, 0);
    GrStringDraw(psContext, "Each panel shows a different feature of", -1, 0,
                 74, 0);
    GrStringDraw(psContext, "the graphics library. Widgets on the panels", -1, 0,
                 92, 0);
    GrStringDraw(psContext, "are fully operational; pressing them will", -1, 0,
                 110, 0);
    GrStringDraw(psContext, "result in visible feedback of some kind.", -1, 0,
                 128, 0);
    GrStringDraw(psContext, "Press the + and - buttons at the bottom", -1, 0,
                 146, 0);
    GrStringDraw(psContext, "of the screen to move between the panels.", -1, 0,
                 164, 0);
}

//*****************************************************************************
//
// Handles paint requests for the primitives canvas widget.
//
//*****************************************************************************
void
OnPrimitivePaint(tWidget *psWidget, tContext *psContext)
{
    uint32_t ui32Idx;
    tRectangle sRect;

    //
    // Draw a vertical sweep of lines from red to green.
    //
    for(ui32Idx = 0; ui32Idx <= 8; ui32Idx++)
    {
        GrContextForegroundSet(psContext,
                               (((((10 - ui32Idx) * 255) / 10) << ClrRedShift) |
                                (((ui32Idx * 255) / 10) << ClrGreenShift)));
        GrLineDraw(psContext, 115, 120, 5, 120 - (11 * ui32Idx));
    }

    //
    // Draw a horizontal sweep of lines from green to blue.
    //
    for(ui32Idx = 1; ui32Idx <= 10; ui32Idx++)
    {
        GrContextForegroundSet(psContext,
                               (((((10 - ui32Idx) * 255) / 10) <<
                                 ClrGreenShift) |
                                (((ui32Idx * 255) / 10) << ClrBlueShift)));
        GrLineDraw(psContext, 115, 120, 5 + (ui32Idx * 11), 29);
    }

    //
    // Draw a filled circle with an overlapping circle.
    //
    GrContextForegroundSet(psContext, ClrBrown);
    GrCircleFill(psContext, 185, 69, 40);
    GrContextForegroundSet(psContext, ClrSkyBlue);
    GrCircleDraw(psContext, 205, 99, 30);

    //
    // Draw a filled rectangle with an overlapping rectangle.
    //
    GrContextForegroundSet(psContext, ClrSlateGray);
    sRect.i16XMin = 20;
    sRect.i16YMin = 100;
    sRect.i16XMax = 75;
    sRect.i16YMax = 160;
    GrRectFill(psContext, &sRect);
    GrContextForegroundSet(psContext, ClrSlateBlue);
    sRect.i16XMin += 40;
    sRect.i16YMin += 40;
    sRect.i16XMax += 30;
    sRect.i16YMax += 28;
    GrRectDraw(psContext, &sRect);

    //
    // Draw a piece of text in fonts of increasing size.
    //
    GrContextForegroundSet(psContext, ClrSilver);
    GrContextFontSet(psContext, &g_sFontCm14);
    GrStringDraw(psContext, "Strings", -1, 125, 110, 0);
    GrContextFontSet(psContext, &g_sFontCm18);
    GrStringDraw(psContext, "Strings", -1, 145, 124, 0);
    GrContextFontSet(psContext, &g_sFontCm22);
    GrStringDraw(psContext, "Strings", -1, 165, 142, 0);
    GrContextFontSet(psContext, &g_sFontCm24);
    GrStringDraw(psContext, "Strings", -1, 185, 162, 0);

    //
    // Draw an image.
    //
    GrImageDraw(psContext, g_pui8Logo, 270, 80);
}

//*****************************************************************************
//
// Handles paint requests for the canvas demonstration widget.
//
//*****************************************************************************
void
OnCanvasPaint(tWidget *psWidget, tContext *psContext)
{
    uint32_t ui32Idx;

    //
    // Draw a set of radiating lines.
    //
    GrContextForegroundSet(psContext, ClrGoldenrod);
    for(ui32Idx = 50; ui32Idx <= 180; ui32Idx += 10)
    {
        GrLineDraw(psContext, 210, ui32Idx, 310, 230 - ui32Idx);
    }

    //
    // Indicate that the contents of this canvas were drawn by the application.
    //
    GrContextFontSet(psContext, &g_sFontCm12);
    GrStringDrawCentered(psContext, "App Drawn", -1, 260, 50, 1);
}

//*****************************************************************************
//
// Handles change notifications for the check box widgets.
//
//*****************************************************************************
void
OnCheckChange(tWidget *psWidget, uint32_t bSelected)
{
    uint32_t ui32Idx;

    //
    // Find the index of this check box.
    //
    for(ui32Idx = 0; ui32Idx < NUM_CHECK_BOXES; ui32Idx++)
    {
        if(psWidget == (tWidget *)(g_psCheckBoxes + ui32Idx))
        {
            break;
        }
    }

    //
    // Return if the check box could not be found.
    //
    if(ui32Idx == NUM_CHECK_BOXES)
    {
        return;
    }

    //
    // Set the matching indicator based on the selected state of the check box.
    //
    CanvasImageSet(g_psCheckBoxIndicators + ui32Idx,
                   bSelected ? g_pui8LightOn : g_pui8LightOff);
    WidgetPaint((tWidget *)(g_psCheckBoxIndicators + ui32Idx));

}

//*****************************************************************************
//
// Handles press notifications for the push button widgets.
//
//*****************************************************************************
void
OnButtonPress(tWidget *psWidget)
{
    uint32_t ui32Idx;

    //
    // Find the index of this push button.
    //
    for(ui32Idx = 0; ui32Idx < NUM_PUSH_BUTTONS; ui32Idx++)
    {
        if(psWidget == (tWidget *)(g_psPushButtons + ui32Idx))
        {
            break;
        }
    }

    //
    // Return if the push button could not be found.
    //
    if(ui32Idx == NUM_PUSH_BUTTONS)
    {
        return;
    }

    //
    // Toggle the state of this push button indicator.
    //
    g_ui32ButtonState ^= 1 << ui32Idx;

    //
    // Set the matching indicator based on the selected state of the check box.
    //
    CanvasImageSet(g_psPushButtonIndicators + ui32Idx,
                   (g_ui32ButtonState & (1 << ui32Idx)) ? g_pui8LightOn :
                   g_pui8LightOff);
    WidgetPaint((tWidget *)(g_psPushButtonIndicators + ui32Idx));

}

//*****************************************************************************
//
// Handles notifications from the slider controls.
//
//*****************************************************************************
void
OnSliderChange(tWidget *psWidget, int32_t i32Value)
{
    static char pcCanvasText[5];
    static char pcSliderText[5];

    //
    // Is this the widget whose value we mirror in the canvas widget and the
    // locked slider?
    //
    if(psWidget == (tWidget *)&g_psSliders[SLIDER_CANVAS_VAL_INDEX])
    {
        //
        // Yes - update the canvas to show the slider value.
        //
        usprintf(pcCanvasText, "%3d%%", i32Value);
        CanvasTextSet(&g_sSliderValueCanvas, pcCanvasText);
        WidgetPaint((tWidget *)&g_sSliderValueCanvas);

        //
        // Also update the value of the locked slider to reflect this one.
        //
        SliderValueSet(&g_psSliders[SLIDER_LOCKED_INDEX], i32Value);
        WidgetPaint((tWidget *)&g_psSliders[SLIDER_LOCKED_INDEX]);
    }

    if(psWidget == (tWidget *)&g_psSliders[SLIDER_TEXT_VAL_INDEX])
    {
        //
        // Yes - update the canvas to show the slider value.
        //
        usprintf(pcSliderText, "%3d%%", i32Value);
        SliderTextSet(&g_psSliders[SLIDER_TEXT_VAL_INDEX], pcSliderText);
        WidgetPaint((tWidget *)&g_psSliders[SLIDER_TEXT_VAL_INDEX]);
    }
}

//*****************************************************************************
//
// Handles change notifications for the radio button widgets.
//
//*****************************************************************************
void
OnRadioChange(tWidget *psWidget, uint32_t bSelected)
{
    uint32_t ui32Idx;

    //
    // Find the index of this radio button in the first group.
    //
    for(ui32Idx = 0; ui32Idx < NUM_RADIO1_BUTTONS; ui32Idx++)
    {
        if(psWidget == (tWidget *)(g_psRadioButtons1 + ui32Idx))
        {
            break;
        }
    }

    //
    // See if the radio button was found.
    //
    if(ui32Idx == NUM_RADIO1_BUTTONS)
    {
        //
        // Find the index of this radio button in the second group.
        //
        for(ui32Idx = 0; ui32Idx < NUM_RADIO2_BUTTONS; ui32Idx++)
        {
            if(psWidget == (tWidget *)(g_psRadioButtons2 + ui32Idx))
            {
                break;
            }
        }

        //
        // Return if the radio button could not be found.
        //
        if(ui32Idx == NUM_RADIO2_BUTTONS)
        {
            return;
        }

        //
        // Sind the radio button is in the second group, offset the index to
        // the indicators associated with the second group.
        //
        ui32Idx += NUM_RADIO1_BUTTONS;
    }

    //
    // Set the matching indicator based on the selected state of the radio
    // button.
    //
    CanvasImageSet(g_psRadioButtonIndicators + ui32Idx,
                   bSelected ? g_pui8LightOn : g_pui8LightOff);
    WidgetPaint((tWidget *)(g_psRadioButtonIndicators + ui32Idx));

}


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
    enableMotor();
    initMotorLib(20, NULL);
    updateMotor(0, 0,1);
    setDuty(100);
}
void MotorTest(){

    updateMotor(0, 1, 0);
    Task_sleep(100);


}
extern void MotorPage();
extern void BackMainMotorPage();
Canvas(g_clear, 0, 0, 0, &g_sKentec320x240x16_SSD2119, 0, 31,
       320, 209, CANVAS_STYLE_FILL, ClrBlack, 0, 0, 0, 0, 0, 0);
tPushButtonWidget MotorPgBtn = RectangularButtonStruct(0, 0, 0,
                                                  &g_sKentec320x240x16_SSD2119, 89, 58, 150, 40,
                                                  PB_STYLE_IMG | PB_STYLE_TEXT, 0, 0, 0, ClrSilver,
                                                  &g_sFontCm16, "Motor Control", g_pui8btn140x40,
                                                  g_pui8btn150x40Press, 0, 0, MotorPage);
tPushButtonWidget SensorPgBtn = RectangularButtonStruct(0, 0, 0,
                                                  &g_sKentec320x240x16_SSD2119, 89, 117, 150, 40,
                                                  PB_STYLE_IMG | PB_STYLE_TEXT, 0, 0, 0, ClrSilver,
                                                  &g_sFontCm16, "Sensor Control", g_pui8btn140x40,
                                                  g_pui8btn150x40Press, 0, 0, btntest);
tPushButtonWidget GraphPgBtn = RectangularButtonStruct(0, 0, 0,
                                                  &g_sKentec320x240x16_SSD2119, 89, 172, 150, 40,
                                                  PB_STYLE_IMG | PB_STYLE_TEXT, 0, 0, 0, ClrSilver,
                                                  &g_sFontCm16, "Data Graphing", g_pui8btn140x40,
                                                  g_pui8btn150x40Press, 0, 0, btntest);

tPushButtonWidget CurrentUpBtn = RectangularButtonStruct(0, 0, 0,
                                                  &g_sKentec320x240x16_SSD2119, 130, 111, 36, 36,
                                                  PB_STYLE_IMG | PB_STYLE_TEXT, 0, 0, 0, ClrSilver,
                                                  &g_sFontCm16, "", g_pui8arrowLeft,
                                                  g_pui8arrowLeft, 0, 0, btntest);
tPushButtonWidget CurrentDownBtn = RectangularButtonStruct(0, 0, 0,
                                                  &g_sKentec320x240x16_SSD2119, 266, 111, 36, 36,
                                                  PB_STYLE_IMG | PB_STYLE_TEXT, 0, 0, 0, ClrSilver,
                                                  &g_sFontCm16, "", g_pui8arrowRight,
                                                  g_pui8arrowRight, 0, 0, btntest);
tPushButtonWidget AccUpBtn = RectangularButtonStruct(0, 0, 0,
                                                  &g_sKentec320x240x16_SSD2119, 130, 155, 36, 36,
                                                  PB_STYLE_IMG | PB_STYLE_TEXT, 0, 0, 0, ClrSilver,
                                                  &g_sFontCm16, "", g_pui8arrowLeft,
                                                  g_pui8arrowLeft, 0, 0, btntest);
tPushButtonWidget AccDownBtn = RectangularButtonStruct(0, 0, 0,
                                                  &g_sKentec320x240x16_SSD2119, 266, 155, 36, 36,
                                                  PB_STYLE_IMG | PB_STYLE_TEXT, 0, 0, 0, ClrSilver,
                                                  &g_sFontCm16, "", g_pui8arrowRight,
                                                  g_pui8arrowRight, 0, 0, btntest);



tPushButtonWidget backMotorBtn = RectangularButtonStruct(0, 0, 0,
                                                  &g_sKentec320x240x16_SSD2119, 1, 209, 30, 30,
                                                  PB_STYLE_IMG | PB_STYLE_TEXT, 0, 0, 0, ClrSilver,
                                                  &g_sFontCm16, "", g_pui8backBtn,
                                                  g_pui8backBtn, 0, 0, BackMainMotorPage);

tPushButtonWidget MotorMode = RectangularButtonStruct(0, 0, 0,
                                                  &g_sKentec320x240x16_SSD2119, 102, 199, 150, 40,
                                                  PB_STYLE_IMG | PB_STYLE_TEXT, 0, 0, 0, ClrSilver,
                                                  &g_sFontCm16, "Motor Start", g_pui8btn140x40,
                                                  g_pui8btn150x40Press, 0, 0, btntest);

tSliderWidget MotorSpeedSlider = SliderStruct(g_psPanels+5, 0, 0,
             &g_sKentec320x240x16_SSD2119, 129, 69, 174, 38, 0, 100, 25,
             (SL_STYLE_FILL | SL_STYLE_BACKG_FILL | SL_STYLE_OUTLINE |
              SL_STYLE_TEXT | SL_STYLE_BACKG_TEXT),
             ClrGray, ClrBlack, ClrSilver, ClrWhite, ClrWhite,
             &g_sFontCm20, "25%", 0, 0, OnSliderChange);
tRectangle clearBox;
tRectangle CurrBox;
tRectangle AccBox;

void MainPage(){
    clearBox.i16XMin = 0;
    clearBox.i16YMin = 31;
    clearBox.i16XMax = 319;
    clearBox.i16YMax = 239;
  WidgetAdd(WIDGET_ROOT,&MotorPgBtn);
  WidgetAdd(WIDGET_ROOT,&SensorPgBtn);
  WidgetAdd(WIDGET_ROOT,&GraphPgBtn);
  GrContextForegroundSet(&sContext, ClrBlack);
  GrContextForegroundSet(&sContext, ClrBlack);
  GrRectFill(&sContext, &clearBox);
  WidgetPaint(&MotorPgBtn);
  WidgetPaint(&SensorPgBtn);
  WidgetPaint(&GraphPgBtn);
}
void RemoveMainPage(){
    WidgetRemove(&MotorPgBtn);
    WidgetRemove(&SensorPgBtn);
    WidgetRemove(&GraphPgBtn);
}
void BackMainMotorPage(){
    WidgetRemove((tWidget *)&MotorSpeedSlider);
    WidgetRemove((tWidget *)&CurrentUpBtn);
    WidgetRemove((tWidget *)&CurrentDownBtn);
    WidgetRemove((tWidget *)&backMotorBtn);
    WidgetRemove((tWidget *)&AccDownBtn);
    WidgetRemove((tWidget *)&AccUpBtn);
    WidgetRemove((tWidget *)&MotorMode);
    MainPage();
}
void MotorPage(){
    //WidgetPaint(&g_clear);
    RemoveMainPage();
    CurrBox.i16XMin = 161;
    CurrBox.i16YMin = 111;
    CurrBox.i16XMax = 109+161;
    CurrBox.i16YMax = 36+111-1;
    AccBox.i16XMin = 161;
    AccBox.i16YMin = 155;
    AccBox.i16XMax = 109+161;
    AccBox.i16YMax = 36+155-1;
    GrContextForegroundSet(&sContext, ClrBlack);
    GrRectFill(&sContext, &clearBox);
    GrContextForegroundSet(&sContext, ClrWhite);
    GrContextFontSet(&sContext, g_psFontCmss18b);
    GrStringDrawCentered(&sContext, "Motor Speed", -1,
                     71, 85, 0);
    GrStringDrawCentered(&sContext, "Current Speed: ", -1,
                     73, 46, 0);
    GrStringDrawCentered(&sContext, "100 rad/s", -1,
                     165, 46, 0);
    GrStringDrawCentered(&sContext, "Max Current", -1,
                     70, 131, 0);
    GrStringDrawCentered(&sContext, "Max Acceleration", -1,
                     83, 176, 0);
    GrContextForegroundSet(&sContext, 0x404040);
    GrRectFill(&sContext, &CurrBox);
    GrRectFill(&sContext, &AccBox);
    GrContextForegroundSet(&sContext, ClrWhite);
    GrRectDraw(&sContext, &CurrBox);
    GrRectDraw(&sContext, &AccBox);
    WidgetAdd(WIDGET_ROOT,(tWidget *)&MotorSpeedSlider);
    WidgetAdd(WIDGET_ROOT,(tWidget *)&CurrentUpBtn);
    WidgetAdd(WIDGET_ROOT,(tWidget *)&CurrentDownBtn);
    WidgetAdd(WIDGET_ROOT,(tWidget *)&backMotorBtn);
    WidgetAdd(WIDGET_ROOT,(tWidget *)&AccDownBtn);
    WidgetAdd(WIDGET_ROOT,(tWidget *)&AccUpBtn);
    WidgetAdd(WIDGET_ROOT,(tWidget *)&MotorMode);
    WidgetPaint((tWidget *)&MotorSpeedSlider);
    WidgetPaint((tWidget *)&CurrentUpBtn);
    WidgetPaint((tWidget *)&CurrentDownBtn);
    WidgetPaint((tWidget *)&backMotorBtn);
    WidgetPaint((tWidget *)&AccDownBtn);
    WidgetPaint((tWidget *)&AccUpBtn);
    WidgetPaint((tWidget *)&MotorMode);


}
Void heartBeatFxn(UArg arg0, UArg arg1)
{
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

    while (1) {
        //Task_sleep(100);
        GPIO_toggle(Board_LED0);
        //MotorTest();
        WidgetMessageQueueProcess();


        //TouchScreenIntHandler;
        Task_sleep(1000);
        sprintf(tempc,"%d:%d:%d %d/%d/%d \r\n",datetime.hours,datetime.minutes,datetime.seconds,datetime.days,datetime.months,datetime.year);
        //UART_write(uart, tempc, sizeof(tempc));
        TopBarDraw(&sContext, &g_sCanvas2, tempc, LowLight);
        LowLight=!LowLight;



    }
}

/*
 *  ======== main ========
 */
void HAF(unsigned int index){
    HA=!HA;
}
void HBF(unsigned int index){
    HB=!HB;
}
void HCF(unsigned int index){
    HC=!HC;
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
    PinoutSet(false, false);
    //DrawFrame()

    /* Construct heartBeat Task  thread */
    Task_Params_init(&taskParams);
    taskParams.arg0 = 1000;
    taskParams.stackSize = TASKSTACKSIZE;
    taskParams.stack = &task0Stack;
    Task_construct(&task0Struct, (Task_FuncPtr)heartBeatFxn, &taskParams, NULL);
    // Turn on user LED
    GPIO_write(Board_LED0, Board_LED_ON);
    //tContext sContext;
    Kentec320x240x16_SSD2119Init(120000000);
    GrContextInit(&sContext, &g_sKentec320x240x16_SSD2119);
    TouchScreenInit(120000000);
    //TouchScreenCallbackSet(WidgetPointerMessage);

    //GrContextForegroundSet(&sContext, ClrDarkBlue);
    //GrRectFill(&sContext, &sRect);
    //
    // Put a white box around the banner.
    //
    //GrContextForegroundSet(&sContext, ClrWhite);
    //GrRectDraw(&sContext, &sRect);
    //
    // Put the application name in the middle of the banner.
    //
    //GrContextFontSet(&sContext, &g_sFontCm20);

    //
    // Configure and enable uDMA
    //
    BannerInit(&sContext);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UDMA);
    SysCtlDelay(10);
    uDMAControlBaseSet(&psDMAControlTable[0]);
    uDMAEnable();
    int ww=GrContextDpyWidthGet(&sContext);
    int hh=GrContextDpyHeightGet(&sContext);

    //
    // Initialize the touch screen driver and have it route its messages to the
    // widget tree.
    //
    //TouchScreenInit(g_ui32SysClock);
    //TouchScreenInit(120000000);
    TouchScreenCallbackSet(WidgetPointerMessage);

    //
    // Add the title block and the previous and next buttons to the widget
    // tree.
    //


    if (!initUART(&uart)){
        System_printf("EROR\n");
    }

    //FrameDraw(&sContext, tempc);
    GPIO_setCallback(HALL_A, HAF);
    GPIO_setCallback(HALL_B, HBF);
    GPIO_setCallback(HALL_C, HCF);

    System_printf("Starting the example\nSystem provider is set to SysMin. "
                  "Halt the target to view any SysMin contents in ROV.\n");
    /* SysMin will only print to the console when you call flush or exit */
    System_flush();

    /* Start BIOS */
    timer1sec(&timerclock,&datetime);
    BIOS_start();

    return (0);
}

