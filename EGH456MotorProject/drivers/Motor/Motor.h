/*
 * Motor.h
 *
 *  Created on: 1 Jun. 2022
 *      Author: akank
 */

#include <time.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <math.h>
/* XDCtools Header files */
#include <xdc/std.h>

/* BIOS Header files */
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Task.h>

/* TI-RTOS Header files */
#include <ti/drivers/GPIO.h>
#include <ti/drivers/UART.h>
#include "driverlib/uart.h"
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
#include <xdc/runtime/Log.h>
#include <xdc/runtime/System.h>
#include <driverlib/sysctl.h>
#include <driverlib/pin_map.h>
#include <ti/sysbios/hal/Timer.h>
#include <math.h>

#ifndef DRIVERS_MOTOR_MOTOR_H_
#define DRIVERS_MOTOR_MOTOR_H_
extern void MotorMain(void);
extern double GetDesiredSpeed(double user_desired_speed);
extern volatile double desired_speed;
extern double VelocitySecond;
extern bool Stop;
extern bool estop;
extern void MotorStart();

extern GateHwi_Handle gateHwi;

#endif /* DRIVERS_MOTOR_MOTOR_H_ */
