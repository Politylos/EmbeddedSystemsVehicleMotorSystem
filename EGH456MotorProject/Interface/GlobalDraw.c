/*
 * GlobalDraw.c
 *
 *  Created on: 7 May 2022
 *      Author: polit
 */
#include <time.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <xdc/std.h>
#include <xdc/runtime/System.h>
#include <driverlib/sysctl.h>

#include <xdc/std.h>
#include <xdc/runtime/System.h>

#include "grlib/grlib.h"
#include "grlib/widget.h"
#include "grlib/canvas.h"
#include "grlib/checkbox.h"
#include "grlib/container.h"
#include "grlib/pushbutton.h"
#include "grlib/radiobutton.h"
#include "grlib/slider.h"
#include "Clock.h"
#include <ti/sysbios/gates/GateMutex.h>
#include <ti/sysbios/gates/GateMutexPri.h>
//#include "Images/GUIImg.h"
#include "Images/Images.h"
#include "Images/GUIImg.h"
#include "GlobalDraw.h"
#include "driverlib/udma.h"
#include "drivers/Kentec320x240x16_ssd2119_spi.h"

#include "GraphFunctions.h"
#include "drivers/touch.h"
#include "empty.h"

#ifdef ewarm
#pragma data_alignment=1024
tDMAControlTable psDMAControlTable[64];
#elif defined(ccs)
#pragma DATA_ALIGN(psDMAControlTable, 1024)
tDMAControlTable psDMAControlTable[64];
#else
tDMAControlTable psDMAControlTable[64] __attribute__ ((aligned(1024)));
#endif

extern void MotorPage();
extern void GraphPage();
extern void BackMainMotorPage();
extern void graphSelectPage();
extern void BackGraphSelect();
extern void BackMainGraph();
extern void backMainSensor();
extern void SensorPage();
extern void btntest();

extern void GraphPrimitive(double *dataPoints, double tstart, double tend,
                           double mesStart, double mesEnd, int size);

tContext sContext;

Canvas(g_clear, 0, 0, 0, &g_sKentec320x240x16_SSD2119, 0, 31, 320, 209,
       CANVAS_STYLE_FILL, ClrBlack, 0, 0, 0, 0, 0, 0);
tPushButtonWidget MotorPgBtn = RectangularButtonStruct(0, 0, 0,
        &g_sKentec320x240x16_SSD2119, 89, 58, 150, 40,
        PB_STYLE_IMG | PB_STYLE_TEXT, 0, 0, 0, ClrSilver,
        &g_sFontCm16, "Motor Control", g_pui8btn140x40,
        g_pui8btn150x40Press, 0, 0, MotorPage)
;
tPushButtonWidget SensorPgBtn = RectangularButtonStruct(0, 0, 0,
        &g_sKentec320x240x16_SSD2119, 89, 117, 150, 40,
        PB_STYLE_IMG | PB_STYLE_TEXT, 0, 0, 0, ClrSilver,
        &g_sFontCm16, "Sensor Control", g_pui8btn140x40,
        g_pui8btn150x40Press, 0, 0, SensorPage)
;
tPushButtonWidget GraphPgBtn = RectangularButtonStruct(0, 0, 0,
        &g_sKentec320x240x16_SSD2119, 89, 172, 150, 40,
        PB_STYLE_IMG | PB_STYLE_TEXT, 0, 0, 0, ClrSilver,
        &g_sFontCm16, "Data Graphing", g_pui8btn140x40,
        g_pui8btn150x40Press, 0, 0, graphSelectPage)
;

tPushButtonWidget CurrentUpBtn = RectangularButtonStruct(0, 0, 0,
        &g_sKentec320x240x16_SSD2119, 130, 111, 36, 36,
        PB_STYLE_IMG | PB_STYLE_TEXT, 0, 0, 0, ClrSilver,
        &g_sFontCm16, "", g_pui8arrowLeft,
        g_pui8arrowLeft, 0, 0, btntest)
;
tPushButtonWidget CurrentDownBtn = RectangularButtonStruct(0, 0, 0,
        &g_sKentec320x240x16_SSD2119, 266, 111, 36, 36,
        PB_STYLE_IMG | PB_STYLE_TEXT, 0, 0, 0, ClrSilver,
        &g_sFontCm16, "", g_pui8arrowRight,
        g_pui8arrowRight, 0, 0, btntest)
;
tPushButtonWidget AccUpBtn = RectangularButtonStruct(0, 0, 0,
        &g_sKentec320x240x16_SSD2119, 130, 155, 36, 36,
        PB_STYLE_IMG | PB_STYLE_TEXT, 0, 0, 0, ClrSilver,
        &g_sFontCm16, "", g_pui8arrowLeft,
        g_pui8arrowLeft, 0, 0, btntest)
;
tPushButtonWidget AccDownBtn = RectangularButtonStruct(0, 0, 0,
        &g_sKentec320x240x16_SSD2119, 266, 155, 36, 36,
        PB_STYLE_IMG | PB_STYLE_TEXT, 0, 0, 0, ClrSilver,
        &g_sFontCm16, "", g_pui8arrowRight,
        g_pui8arrowRight, 0, 0, btntest)
;

tPushButtonWidget DistUpBtn = RectangularButtonStruct(0, 0, 0,
        &g_sKentec320x240x16_SSD2119, 274, 101, 36, 36,
        PB_STYLE_IMG | PB_STYLE_TEXT, 0, 0, 0, ClrSilver,
        &g_sFontCm16, "", g_pui8arrowRight,
        g_pui8arrowRight, 0, 0, btntest)
;
tPushButtonWidget DistDownBtn = RectangularButtonStruct(0, 0, 0,
        &g_sKentec320x240x16_SSD2119, 138, 101, 36, 36,
        PB_STYLE_IMG | PB_STYLE_TEXT, 0, 0, 0, ClrSilver,
        &g_sFontCm16, "", g_pui8arrowLeft,
        g_pui8arrowLeft, 0, 0, btntest)
;

tPushButtonWidget backMotorBtn = RectangularButtonStruct(0, 0, 0,
        &g_sKentec320x240x16_SSD2119, 1, 209, 30, 30,
        PB_STYLE_IMG | PB_STYLE_TEXT, 0, 0, 0, ClrSilver,
        &g_sFontCm16, "", g_pui8backBtn,
        g_pui8backBtn, 0, 0, BackMainMotorPage)
;

tPushButtonWidget backGraphBtn = RectangularButtonStruct(0, 0, 0,
        &g_sKentec320x240x16_SSD2119, 1, 209, 30, 30,
        PB_STYLE_IMG | PB_STYLE_TEXT, 0, 0, 0, ClrSilver,
        &g_sFontCm16, "", g_pui8backBtn,
        g_pui8backBtn, 0, 0, BackGraphSelect)
;
tPushButtonWidget backSensorBtn = RectangularButtonStruct(0, 0, 0,
        &g_sKentec320x240x16_SSD2119, 1, 209, 30, 30,
        PB_STYLE_IMG | PB_STYLE_TEXT, 0, 0, 0, ClrSilver,
        &g_sFontCm16, "", g_pui8backBtn,
        g_pui8backBtn, 0, 0, backMainSensor)
;
tPushButtonWidget backGraphSelectBtn = RectangularButtonStruct(0, 0, 0,
        &g_sKentec320x240x16_SSD2119, 1, 209, 30, 30,
        PB_STYLE_IMG | PB_STYLE_TEXT, 0, 0, 0, ClrSilver,
        &g_sFontCm16, "", g_pui8backBtn,
        g_pui8backBtn, 0, 0, BackMainGraph)
;

tPushButtonWidget MotorMode = RectangularButtonStruct(0, 0, 0,
        &g_sKentec320x240x16_SSD2119, 102, 199, 150, 40,
        PB_STYLE_IMG | PB_STYLE_TEXT, 0, 0, 0, ClrSilver,
        &g_sFontCm16, "Motor Start", g_pui8btn140x40,
        g_pui8btn150x40Press, 0, 0, btntest)
;

tPushButtonWidget PowerGraphBtn = RectangularButtonStruct(0, 0, 0,
        &g_sKentec320x240x16_SSD2119, 83, 43, 150, 40,
        PB_STYLE_IMG | PB_STYLE_TEXT, 0, 0, 0, ClrSilver,
        &g_sFontCm16, "Power Graph", g_pui8btn140x40,
        g_pui8btn150x40Press, 0, 0, MotorPage)
;
tPushButtonWidget MotorGraphBtn = RectangularButtonStruct(0, 0, 0,
        &g_sKentec320x240x16_SSD2119, 83, 90, 150, 40,
        PB_STYLE_IMG | PB_STYLE_TEXT, 0, 0, 0, ClrSilver,
        &g_sFontCm16, "Motor Speed Graph", g_pui8btn140x40,
        g_pui8btn150x40Press, 0, 0, MotorPage)
;
tPushButtonWidget LightGraphBtn = RectangularButtonStruct(0, 0, 0,
        &g_sKentec320x240x16_SSD2119, 83, 138, 150, 40,
        PB_STYLE_IMG | PB_STYLE_TEXT, 0, 0, 0, ClrSilver,
        &g_sFontCm16, "Light Graph", g_pui8btn140x40,
        g_pui8btn150x40Press, 0, 0, MotorPage)
;
tPushButtonWidget TestGraphBtn = RectangularButtonStruct(0, 0, 0,
        &g_sKentec320x240x16_SSD2119, 83, 187, 150, 40,
        PB_STYLE_IMG | PB_STYLE_TEXT, 0, 0, 0, ClrSilver,
        &g_sFontCm16, "Test Graph", g_pui8btn140x40,
        g_pui8btn150x40Press, 0, 0, GraphPage)
;

tSliderWidget MotorSpeedSlider = SliderStruct(0, 0, 0,
        &g_sKentec320x240x16_SSD2119, 129, 69, 174, 38, 0, 100, 25,
        (SL_STYLE_FILL | SL_STYLE_BACKG_FILL | SL_STYLE_OUTLINE |
                SL_STYLE_TEXT | SL_STYLE_BACKG_TEXT),
        ClrGray, ClrBlack, ClrSilver, ClrWhite, ClrWhite,
        &g_sFontCm20, "25%", 0, 0, btntest)
;

tRectangle clearBox;
tRectangle CurrBox;
tRectangle AccBox;
tRectangle Distbox;

Canvas(g_sCanvas3, 0, 0, 0, &g_sKentec320x240x16_SSD2119, 205, 27, 110, 158,
       CANVAS_STYLE_OUTLINE | CANVAS_STYLE_APP_DRAWN, 0, ClrGray, 0, 0, 0, 0,
       0);
Canvas(g_sCanvas2, 0, &g_sCanvas3, 0, &g_sKentec320x240x16_SSD2119, 292, 0, 25,
       25, CANVAS_STYLE_IMG, 0, 0, 0, 0, 0, g_pui8Logo, 0);
Canvas(g_scompass, 0, 0, 0, &g_sKentec320x240x16_SSD2119, 260, 0, 25, 25,
       CANVAS_STYLE_IMG, 0, 0, 0, 0, 0, g_pui8CompN, 0);
Canvas(g_MotorStat, 0, 0, 0, &g_sKentec320x240x16_SSD2119, 233, 0, 25, 25,
       CANVAS_STYLE_IMG, 0, 0, 0, 0, 0, g_pui8idle, 0);
Canvas(g_Acceleration, 0, 0, 0, &g_sKentec320x240x16_SSD2119, 206, 0, 25, 25,
       CANVAS_STYLE_IMG, 0, 0, 0, 0, 0, g_pui8accSlow, 0);

Canvas(g_sCanvas1, 0, &g_sCanvas2, 0, &g_sKentec320x240x16_SSD2119, 5, 27, 195,
       76, CANVAS_STYLE_FILL | CANVAS_STYLE_OUTLINE | CANVAS_STYLE_TEXT,
       ClrMidnightBlue, ClrGray, ClrSilver, &g_sFontCm22, "Text", 0, 0);

tRectangle Banner;
tRectangle BannerB;

GateMutexPri_Handle ScreenGate;


void btntest()
{
    int ahhhhhh = 1;
}

void InitScreen()
{
    GateMutexPri_Params Gprams;
    GateMutexPri_Params_init(&Gprams);
    ScreenGate = GateMutexPri_create(&Gprams, NULL);
    Kentec320x240x16_SSD2119Init(120000000);
    GrContextInit(&sContext, &g_sKentec320x240x16_SSD2119);
    TouchScreenInit(120000000);
    BannerInit(&sContext);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UDMA);
    SysCtlDelay(10);
    uDMAControlBaseSet(&psDMAControlTable[0]);
    uDMAEnable();
    TouchScreenCallbackSet(WidgetPointerMessage);
    MainPage();
}
void BannerInit(tContext *sContext)
{
    Banner.i16XMin = 0;
    Banner.i16YMin = 0;
    Banner.i16XMax = GrContextDpyWidthGet(sContext) - 1;
    Banner.i16YMax = 29;
    BannerB.i16XMin = 0;
    BannerB.i16YMin = 0;
    BannerB.i16XMax = GrContextDpyWidthGet(sContext) - 1;
    BannerB.i16YMax = 30;
    GrContextForegroundSet(sContext, ClrWhite);
    GrRectDraw(sContext, &BannerB);
    GrContextForegroundSet(sContext, 0x00001A);
    GrRectFill(sContext, &Banner);
    GrContextForegroundSet(sContext, ClrWhite);
    GrContextFontSet(sContext, g_psFontCmss18b);
    GrStringDrawCentered(sContext, "Loadinng...", -1,
                         GrContextDpyWidthGet(sContext) / 2-100, 8, 0);
}

void TopBarDraw(tContext *sContext, FT time, bool Lightlvl, int Direction,
                int MotorStats, int accStat)
{
    IArg ScreenKey;
    ScreenKey = GateMutexPri_enter(ScreenGate);
    GrContextForegroundSet(sContext, 0x00001A);
    GrRectFill(sContext, &Banner);
    GrContextForegroundSet(sContext, ClrWhite);
    GrContextFontSet(sContext, g_psFontCmss20);
    char tempc[8];
    char tempd[10];
    sprintf(tempc, "%02d:%02d:%02d", time.hours, time.minutes, time.seconds);
    sprintf(tempd, "%02d/%02d/%02d", time.days, time.months, time.year);
    GrStringDraw(sContext, tempc, 8,
    GrContextDpyWidthGet(sContext) / 2 - 155,
                 10, 0);
    GrContextFontSet(sContext, g_psFontCmss12b);
    GrStringDrawCentered(sContext, tempd, 8,
                         GrContextDpyWidthGet(sContext) / 2-50, 17, 0);
    if (Lightlvl)
    {
        CanvasImageSet(&g_sCanvas2, g_pui8Night);

    }
    else
    {
        CanvasImageSet(&g_sCanvas2, g_pui8Day);
    }
    if (Direction == 0)
    {
        CanvasImageSet(&g_scompass, g_pui8CompN);
    }
    else if (Direction == 1)
    {
        CanvasImageSet(&g_scompass, g_pui8CompE);
    }
    else if (Direction == 2)
    {
        CanvasImageSet(&g_scompass, g_pui8CompS);
    }
    else
    {
        CanvasImageSet(&g_scompass, g_pui8CompW);
    }
    if (MotorStats == 0)
    {
        CanvasImageSet(&g_MotorStat, g_pui8idle);

    }
    else if (MotorStats == 1)
    {
        CanvasImageSet(&g_MotorStat, g_pui8Go);
    }
    else
    {
        CanvasImageSet(&g_MotorStat, g_pui8Stop);
    }
    if (accStat == 0)
    {
        CanvasImageSet(&g_Acceleration, g_pui8accSlow);

    }
    else if (accStat == 1)
    {
        CanvasImageSet(&g_Acceleration, g_pui8accmid);
    }
    else
    {
        CanvasImageSet(&g_Acceleration, g_pui8accfast);
    }
    WidgetPaint(&g_sCanvas2);
    WidgetPaint(&g_scompass);
    WidgetPaint(&g_MotorStat);
    WidgetPaint(&g_Acceleration);
    GateMutexPri_leave(ScreenGate, ScreenKey);
}

void GraphPage()
{
    float x1 = 0;
    float x2 = 10;
    float y1 = 0;
    float y2 = 100;
    //ongraph=1;
    double dd[50] = { 0, 0.0416493127863390, 0.166597251145356,
                      0.374843815077051, 0.666389004581425, 1.04123281965848,
                      1.49937526030821, 2.04081632653061, 2.66555601832570,
                      3.37359433569346, 4.16493127863390, 5.03956684714702,
                      5.99750104123282, 7.03873386089130, 8.16326530612245,
                      9.37109537692628, 10.6622240733028, 12.0366513952520,
                      13.4943773427738, 15.0354019158684, 16.6597251145356,
                      18.3673469387755, 20.1582673885881, 22.0324864639734,
                      23.9900041649313, 26.0308204914619, 28.1549354435652,
                      30.3623490212412, 32.6530612244898, 35.0270720533111,
                      37.4843815077051, 40.0249895876718, 42.6488962932112,
                      45.3561016243232, 48.1466055810079, 51.0204081632653,
                      53.9775093710954, 57.0179092044981, 60.1416076634736,
                      63.3486047480217, 66.6389004581424, 70.0124947938359,
                      73.4693877551020, 77.0095793419409, 80.6330695543524,
                      84.3398583923365, 88.1299458558934, 92.0033319450229,
                      95.9600166597251, 100 };
    GraphPrimitive(dd, x1, x2, y1, y2, 50);
}

void MainPage()
{
    IArg ScreenKey;
    ScreenKey = GateMutexPri_enter(ScreenGate);
    clearBox.i16XMin = 0;
    clearBox.i16YMin = 32;
    clearBox.i16XMax = 319;
    clearBox.i16YMax = 240;
    WidgetAdd(WIDGET_ROOT, &MotorPgBtn);
    WidgetAdd(WIDGET_ROOT, &SensorPgBtn);
    WidgetAdd(WIDGET_ROOT, &GraphPgBtn);
    GrContextForegroundSet(&sContext, ClrBlack);
    GrRectFill(&sContext, &clearBox);
    WidgetPaint(&MotorPgBtn);
    WidgetPaint(&SensorPgBtn);
    WidgetPaint(&GraphPgBtn);
    GateMutexPri_leave(ScreenGate, ScreenKey);
}

void RemoveMainPage()
{
    // ScreenKey = GateMutexPri_enter(ScreenGate);
    WidgetRemove(&MotorPgBtn);
    WidgetRemove(&SensorPgBtn);
    WidgetRemove(&GraphPgBtn);
}
void SensorPage()
{
    //double lux = 12.45;
    double lux = opt3001_filteredValue;
    double objectdist = 23;
    IArg ScreenKey;
    ScreenKey = GateMutexPri_enter(ScreenGate);
    RemoveMainPage();
    GrContextForegroundSet(&sContext, ClrBlack);
    GrRectFill(&sContext, &clearBox);
    Distbox.i16XMin = 165;
    Distbox.i16YMin = 101;
    Distbox.i16XMax = 165 + 119;
    Distbox.i16YMax = 101 + 36;
    GrContextForegroundSet(&sContext, 0x404040);
    GrRectFill(&sContext, &Distbox);
    GrContextForegroundSet(&sContext, ClrWhite);
    GrLineDraw(&sContext, 0, 64, 319, 64);
    GrRectDraw(&sContext, &Distbox);
    WidgetAdd(WIDGET_ROOT, &backSensorBtn);
    WidgetPaint((tWidget* )&backSensorBtn);
    WidgetAdd(WIDGET_ROOT, &DistDownBtn);
    WidgetPaint((tWidget* )&DistDownBtn);
    WidgetAdd(WIDGET_ROOT, &DistUpBtn);
    WidgetPaint((tWidget* )&DistUpBtn);
    GrContextFontSet(&sContext, g_psFontCmss12);
    GrStringDrawCentered(&sContext, "Light Level: ", -1, 48, 47, 0);
    GrStringDrawCentered(&sContext, "Closest Object: ", -1, 68, 80, 0);
    GrStringDrawCentered(&sContext, "Min Distance", -1, 70, 120, 0);
    char tempchar[13];
    sprintf(tempchar, "%5.2f lux", lux);
    GrStringDrawCentered(&sContext, tempchar, -1, 117, 47, 0);
    sprintf(tempchar, "%5.2f m", objectdist);
    GrStringDrawCentered(&sContext, tempchar, -1, 135, 80, 0);
    GateMutexPri_leave(ScreenGate, ScreenKey);
}
void removeSensorPage()
{
    //ScreenKey = GateMutexPri_enter(ScreenGate);
    WidgetRemove(&backSensorBtn);
    WidgetRemove(&DistDownBtn);
    WidgetRemove(&DistUpBtn);
}
void backMainSensor()
{
    IArg ScreenKey;
    ScreenKey = GateMutexPri_enter(ScreenGate);
    removeSensorPage();
    GateMutexPri_leave(ScreenGate, ScreenKey);
    MainPage();
}
void graphSelectPage()
{
    IArg ScreenKey;
    ScreenKey = GateMutexPri_enter(ScreenGate);
    RemoveMainPage();
    GrContextForegroundSet(&sContext, ClrBlack);
    GrRectFill(&sContext, &clearBox);
    WidgetAdd(WIDGET_ROOT, &PowerGraphBtn);
    WidgetAdd(WIDGET_ROOT, &MotorGraphBtn);
    WidgetAdd(WIDGET_ROOT, &LightGraphBtn);
    WidgetAdd(WIDGET_ROOT, &TestGraphBtn);
    WidgetAdd(WIDGET_ROOT, &backGraphSelectBtn);
    WidgetPaint(&PowerGraphBtn);
    WidgetPaint(&MotorGraphBtn);
    WidgetPaint(&LightGraphBtn);
    WidgetPaint(&TestGraphBtn);
    WidgetPaint(&backGraphSelectBtn);
    GateMutexPri_leave(ScreenGate, ScreenKey);
}
void RemoveGraphSelect()
{
    WidgetRemove(&PowerGraphBtn);
    WidgetRemove(&MotorGraphBtn);
    WidgetRemove(&LightGraphBtn);
    WidgetRemove(&TestGraphBtn);
    WidgetRemove(&backGraphSelectBtn);

}
void RemoveGraph()
{
    //ScreenKey = GateMutexPri_enter(ScreenGate);
    //ongraph=0;
    WidgetRemove((tWidget*) &backGraphBtn);
}
void BackGraphSelect()
{
    IArg ScreenKey;
    ScreenKey = GateMutexPri_enter(ScreenGate);
    RemoveGraph();
    GateMutexPri_leave(ScreenGate, ScreenKey);
    graphSelectPage();
}
void BackMainGraph()
{
    IArg ScreenKey;
    ScreenKey = GateMutexPri_enter(ScreenGate);
    RemoveGraphSelect();
    GateMutexPri_leave(ScreenGate, ScreenKey);
    MainPage();
}
tRectangle graphClear;
void UpdateGraphPlot(double *dataPoints, double tstart, double tend,
                     double mesStart, double mesEnd, int size)
{
    IArg ScreenKey;
    ScreenKey = GateMutexPri_enter(ScreenGate);
    graphClear.i16XMin = 54;
    graphClear.i16YMin = 56;
    graphClear.i16XMax = 303;
    graphClear.i16YMax = 207;
    GrContextForegroundSet(&sContext, ClrBlack);
    GrRectFill(&sContext, &graphClear);
    int ydif = CalcNoRemainder(150, mesEnd - mesStart);
    mesEnd = mesStart + ydif;
    int pixelH = 150 / ydif;
    int pixelW = 250 / size;
    int i;
    int ystart;
    int yend;
    int x = 54;
    //GrContextForegroundSet(&sContext, ClrBlack);
    //GrRectFill(&sContext, &clearBox);
    GrContextForegroundSet(&sContext, ClrRed);
    int qq = size - 1;

    for (i = 0; i < size - 1; i++)
    {
        ystart = -dataPoints[i] * pixelH + 207;
        yend = -dataPoints[i + 1] * pixelH + 207;
        GrLineDraw(&sContext, x, ystart, x + pixelW, yend);
        x = x + pixelW;
    }
    GateMutexPri_leave(ScreenGate, ScreenKey);
}

void GraphPrimitive(double *dataPoints, double tstart, double tend,
                    double mesStart, double mesEnd, int size)
{
    IArg ScreenKey;
    ScreenKey = GateMutexPri_enter(ScreenGate);
    int ydif = CalcNoRemainder(150, mesEnd - mesStart);
    mesEnd = mesStart + ydif;
    int pixelH = 150 / ydif;
    int pixelW = 250 / size;
    int i;
    int ystart;
    int yend;
    RemoveGraphSelect();
    GrContextForegroundSet(&sContext, ClrBlack);
    GrRectFill(&sContext, &clearBox);
    GrContextForegroundSet(&sContext, ClrWhite);
    GrLineDraw(&sContext, 52, 56, 52, 209);
    GrLineDraw(&sContext, 53, 56, 53, 209);
    GrLineDraw(&sContext, 52, 209, 303, 209);
    GrLineDraw(&sContext, 52, 210, 303, 210);
    WidgetAdd(WIDGET_ROOT, (tWidget*) &backGraphBtn);
    WidgetPaint((tWidget* )&backGraphBtn);
    GrContextFontSet(&sContext, g_psFontCmss12);
    char tempchar[9];
    sprintf(tempchar, "%5.2f", tstart);
    GrStringDrawCentered(&sContext, tempchar, -1, 55, 220, 0);
    sprintf(tempchar, "%5.2f", tend);
    GrStringDrawCentered(&sContext, tempchar, -1, 296, 220, 0);
    GrStringDrawCentered(&sContext, "Time (sec)", -1, 320 / 2, 229, 0);
    sprintf(tempchar, "%5.2f", mesStart);
    GrStringDrawCentered(&sContext, tempchar, -1, 35, 207, 0);
    sprintf(tempchar, "%5.2f", mesEnd);
    GrStringDrawCentered(&sContext, tempchar, -1, 35, 57, 0);
    int x = 54;
    GrContextForegroundSet(&sContext, ClrRed);
    int qq = size - 1;
    for (i = 0; i < size - 1; i++)
    {
        ystart = -dataPoints[i] * pixelH + 207;
        yend = -dataPoints[i + 1] * pixelH + 207;
        GrLineDraw(&sContext, x, ystart, x + pixelW, yend);
        x = x + pixelW;
    }
    GateMutexPri_leave(ScreenGate, ScreenKey);
}
void BackMainMotorPage()
{
    IArg ScreenKey;
    ScreenKey = GateMutexPri_enter(ScreenGate);
    WidgetRemove((tWidget*) &MotorSpeedSlider);
    WidgetRemove((tWidget*) &CurrentUpBtn);
    WidgetRemove((tWidget*) &CurrentDownBtn);
    WidgetRemove((tWidget*) &backMotorBtn);
    WidgetRemove((tWidget*) &AccDownBtn);
    WidgetRemove((tWidget*) &AccUpBtn);
    WidgetRemove((tWidget*) &MotorMode);
    GateMutexPri_leave(ScreenGate, ScreenKey);
    MainPage();
}
void MotorPage()
{
    IArg ScreenKey;
    ScreenKey = GateMutexPri_enter(ScreenGate);
    //WidgetPaint(&g_clear);
    RemoveMainPage();
    CurrBox.i16XMin = 161;
    CurrBox.i16YMin = 111;
    CurrBox.i16XMax = 109 + 161;
    CurrBox.i16YMax = 36 + 111 - 1;
    AccBox.i16XMin = 161;
    AccBox.i16YMin = 155;
    AccBox.i16XMax = 109 + 161;
    AccBox.i16YMax = 36 + 155 - 1;
    GrContextForegroundSet(&sContext, ClrBlack);
    GrRectFill(&sContext, &clearBox);
    GrContextForegroundSet(&sContext, ClrWhite);
    GrContextFontSet(&sContext, g_psFontCmss18b);
    GrStringDrawCentered(&sContext, "Motor Speed", -1, 71, 85, 0);
    GrStringDrawCentered(&sContext, "Current Speed: ", -1, 73, 46, 0);
    GrStringDrawCentered(&sContext, "100 rad/s", -1, 165, 46, 0);
    GrStringDrawCentered(&sContext, "Max Current", -1, 70, 131, 0);
    GrStringDrawCentered(&sContext, "Max Acceleration", -1, 83, 176, 0);
    GrContextForegroundSet(&sContext, 0x404040);
    GrRectFill(&sContext, &CurrBox);
    GrRectFill(&sContext, &AccBox);
    GrContextForegroundSet(&sContext, ClrWhite);
    GrRectDraw(&sContext, &CurrBox);
    GrRectDraw(&sContext, &AccBox);
    WidgetAdd(WIDGET_ROOT, (tWidget*) &MotorSpeedSlider);
    WidgetAdd(WIDGET_ROOT, (tWidget*) &CurrentUpBtn);
    WidgetAdd(WIDGET_ROOT, (tWidget*) &CurrentDownBtn);
    WidgetAdd(WIDGET_ROOT, (tWidget*) &backMotorBtn);
    WidgetAdd(WIDGET_ROOT, (tWidget*) &AccDownBtn);
    WidgetAdd(WIDGET_ROOT, (tWidget*) &AccUpBtn);
    WidgetAdd(WIDGET_ROOT, (tWidget*) &MotorMode);
    WidgetPaint((tWidget* )&MotorSpeedSlider);
    WidgetPaint((tWidget* )&CurrentUpBtn);
    WidgetPaint((tWidget* )&CurrentDownBtn);
    WidgetPaint((tWidget* )&backMotorBtn);
    WidgetPaint((tWidget* )&AccDownBtn);
    WidgetPaint((tWidget* )&AccUpBtn);
    WidgetPaint((tWidget* )&MotorMode);
    GateMutexPri_leave(ScreenGate, ScreenKey);
}

