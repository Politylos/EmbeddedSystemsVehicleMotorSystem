/*
 * GlobalDraw.h
 *
 *  Created on: 7 May 2022
 *      Author: polit
 */

#ifndef INTERFACE_GLOBALDRAW_H_
#define INTERFACE_GLOBALDRAW_H_
#include "Clock.h"
extern tContext sContext;
extern void TopBarDraw(tContext *sContext,  FT time, bool Lightlvl,int Direction,int MotorStats, int accStat);
extern void BannerInit(tContext *sContext);
extern void InitScreen();
extern void MainPage();
extern void UpdateGraphPlot(double *dataPoints,double tstart,double tend,double mesStart, double mesEnd, int size);

#endif /* INTERFACE_GLOBALDRAW_H_ */
