/*
 * Clock.h
 *
 *  Created on: 6 May 2022
 *      Author: polit
 */

#ifndef CLOCK_H_
#define CLOCK_H_


#include <stdbool.h>

#include <ti/sysbios/hal/Timer.h>
#include <ti/sysbios/hal/Seconds.h>

typedef struct FullTime{
    int year;
    int months;
    int days;
    int hours;
    int minutes;
    int seconds;
}FT;

extern  FT timeConvert(Seconds_Time time);

extern bool timer1sec();
extern void updatetime(FT *date);

#endif /* CLOCK_H_ */
