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

#include "grlib/grlib.h"
#include "grlib/widget.h"
#include "grlib/canvas.h"
#include "grlib/checkbox.h"
#include "grlib/container.h"
#include "grlib/pushbutton.h"
#include "grlib/radiobutton.h"
#include "grlib/slider.h"
#include "Clock.h"

//#include "Images/GUIImg.h"
#include "Images/Images.h"
#include "GlobalDraw.h"
tRectangle Banner;
tRectangle BannerB;
void BannerInit(tContext *sContext){
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
void TopBarDraw(tContext *sContext, tCanvasWidget *imgcan, tCanvasWidget *comp,tCanvasWidget *motor,tCanvasWidget *acc, FT time, bool Lightlvl,int Direction,int MotorStats, int accStat){
    GrContextForegroundSet(sContext, 0x00001A);
    GrRectFill(sContext, &Banner);
    GrContextForegroundSet(sContext, ClrWhite);
    GrContextFontSet(sContext, g_psFontCmss20);
    char tempc[8];
    char tempd[10];
    sprintf(tempc,"%d:%d:%d",time.hours,time.minutes,time.seconds);
    sprintf(tempd,"%d/%d/%d",time.days,time.months,time.year);
    GrStringDrawCentered(sContext, tempc, -1,
                         GrContextDpyWidthGet(sContext) / 2-125, 20, 0);
    GrContextFontSet(sContext, g_psFontCmss12b);
    GrStringDrawCentered(sContext, tempd, -1,
                         GrContextDpyWidthGet(sContext) / 2-60, 20, 0);
    if(Lightlvl){
    CanvasImageSet(imgcan,g_pui8Night);

    } else{
    CanvasImageSet(imgcan,g_pui8Day);
    }
    if(Direction==0){
        CanvasImageSet(comp,g_pui8CompN);
    } else if (Direction==1){
        CanvasImageSet(comp,g_pui8CompE);
    }else if (Direction==2){
        CanvasImageSet(comp,g_pui8CompS);
    } else{
        CanvasImageSet(comp,g_pui8CompW);
    }
    if(MotorStats==0){
        CanvasImageSet(motor,g_pui8idle);

        } else if(MotorStats==1){
        CanvasImageSet(motor,g_pui8Go);
        } else{
            CanvasImageSet(motor,g_pui8Stop);
        }
    if(accStat==0){
        CanvasImageSet(acc,g_pui8accSlow);

        } else if(accStat==1){
        CanvasImageSet(acc,g_pui8accmid);
        } else{
            CanvasImageSet(acc,g_pui8accfast);
        }
    WidgetPaint(imgcan);
    WidgetPaint(comp);
    WidgetPaint(motor);
    WidgetPaint(acc);
}


