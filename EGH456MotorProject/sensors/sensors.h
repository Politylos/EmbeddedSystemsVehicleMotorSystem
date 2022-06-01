/*
 * sensors.h
 *
 *  Created on: 23 May 2022
 *      Author: Baker
 */
#include <ti/sysbios/knl/swi.h>
#include <xdc/std.h>
#include <xdc/runtime/System.h>
#ifndef SENSORS_SENSORS_H_
#define SENSORS_SENSORS_H_
extern double AccelSecond;
extern double LightSecond;
extern void readAccFxn(UArg arg0, UArg arg1);
extern void lightSensorFilterFxn(void);
extern void readOptFxn(UArg arg0, UArg arg1);
extern void readCurrentFxn(UArg arg0, UArg arg1);
extern void i2c_Open(void);
extern void accelSensorFilterFxn(void);
extern void currentSensorFilterFxn(void);
extern Swi_Handle swiAccelSensorFilterHandle;
extern Swi_Handle swiLightSensorFilterHandle;
extern Swi_Handle swiCurrentSensorFilterHandle;
extern double MaxCurrent;
#endif /* SENSORS_SENSORS_H_ */
