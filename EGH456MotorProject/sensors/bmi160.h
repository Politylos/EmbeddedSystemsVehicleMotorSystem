/*
 * bmi160.h
 *
 *  Created on: 13 May 2022
 *      Author: Michael Baker
 */

#ifndef DRIVERS_BMI160_H_
#define DRIVERS_BMI160_H_

#include <stdbool.h>
#include <stdint.h>

typedef struct
{
    double acc_x;
    double acc_y;
    double acc_z;
} bmi160_accel_values_t;
extern int MaxAccel;
bool bmi160_init(void);
bool bmi160_readAccel(int16_t *rawData);
double bmi160_readConvert(int16_t rawValue);
bool bmi160_getAccel(bmi160_accel_values_t *values);



#endif /* DRIVERS_BMI160_H_ */
