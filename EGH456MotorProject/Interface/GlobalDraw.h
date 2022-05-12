/*
 * GlobalDraw.h
 *
 *  Created on: 7 May 2022
 *      Author: polit
 */

#ifndef INTERFACE_GLOBALDRAW_H_
#define INTERFACE_GLOBALDRAW_H_
#include "Clock.h"
extern void TopBarDraw(tContext *sContext, tCanvasWidget *imgcan, tCanvasWidget *comp,tCanvasWidget *motor,tCanvasWidget *acc, FT time, bool Lightlvl,int Direction,int MotorStats, int accStat);
extern void BannerInit(tContext *sContext);


#endif /* INTERFACE_GLOBALDRAW_H_ */
