/*
 * Estop.h
 *
 *  Created on: 31 May 2022
 *      Author: polit
 */

#ifndef DRIVERS_MOTOR_ESTOP_H_
#define DRIVERS_MOTOR_ESTOP_H_


extern void initEstop();
extern void EstopAccel();
extern void EstopPower();
extern bool checkEstop();
extern bool EStop;


#endif /* DRIVERS_MOTOR_ESTOP_H_ */
