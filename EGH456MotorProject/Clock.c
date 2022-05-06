/*
 * Clock.c
 *
 *  Created on: 6 May 2022
 *      Author: polit
 */

#include <time.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <math.h>

#include <ti/sysbios/hal/Seconds.h>
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
#include "Clock.h"
#include "drivers/pinout.h"

FT timeConvert(Seconds_Time time){
    FT ft;
    time.secs=time.secs+10*60*60;
    ft.year = floor(1970+time.secs/31556926);//10%5;//((int)time.secs/31556926 - time%31556926);
    int leap = floor((ft.year/4)-(ft.year/100)+(ft.year/400)-((1970/4)-(1970/100)+(1970/400)));
    int normyear = floor((ft.year-1970)-leap);
    //ft.months = ((time.secs%31556926)/2629743);// -time%31556926%2629743;
    int left = floor((((time.secs-(31536000*normyear+leap*31622400)))));
    ft.days = floor((left/86400));

    int l = 0;
    if(ft.year%400+ ft.year%400+ ft.year%4 +ft.year%100 == 0){
        l++;
    }
    ft.months=0;
    int sday = ft.days*24*60*60;
    if ((ft.days - 31) > 0){
        ft.days=ft.days- 31;
        ft.months=1;

        if (((ft.days - 27+l)) > 0){
                ft.days=ft.days- (27+l);
                ft.months=2;

                if (((ft.days - 31)) > 0){
                             ft.days=ft.days- 31;
                             ft.months=3;
                             if ((ft.days - 30) > 0){
                                 ft.days=ft.days- 30;
                                 ft.months=4;
                                 if (((ft.days - 31)) > 0){
                                             ft.days=ft.days- 31;
                                             ft.months=5;

                                             if (((ft.days - 30)) > 0){
                                                         ft.days=ft.days- 30;
                                                         ft.months=6;

                                                         if (((ft.days - 31)) > 0){
                                                                     ft.days=ft.days- 31;
                                                                     ft.months=7;

                                                                     if (((ft.days - 31)) > 0){
                                                                                 ft.days=ft.days- 31;
                                                                                 ft.months=8;

                                                                                 if (((ft.days - 30)) > 0){
                                                                                             ft.days=ft.days- 30;
                                                                                             ft.months=9;

                                                                                             if (((ft.days - 31)) > 0){
                                                                                                         ft.days=ft.days- 31;
                                                                                                         ft.months=10;

                                                                                                         if (((ft.days - 30)) > 0){
                                                                                                                     ft.days=ft.days- 30;
                                                                                                                     ft.months=11;
                                                                                                                     if (((ft.days - 31)) > 0){
                                                                                                                                 ft.days=ft.days- 31;
                                                                                                                                 ft.months=12;
                                                                                                                     }
                                                                                                         }

                                                                                             }

                                                                                 }

                                                                     }

                                                         }

                                             }

                                 }

                             }

                 }

        }

    }
    ft.hours = (((floor(((left%86400))/3600))));//((((time.secs%31556926)%2629743)%86400)/3600);//(time.secs%31556926%2629743%86400/3600) -time%31556926%2629743%86400%3600;
    ft.minutes = floor(((left%86400)%3600)/60);//(((((time.secs%31556926)%2629743)%86400)%3600)/60);// (time.secs%31556926%2629743%86400%3600/60) -time%31556926%2629743%86400%3600%60;
    ft.seconds = floor(((left%86400)%3600)%60);
    return ft;

}
void updatetime(FT *date){
    Seconds_Time secs;
    UInt32 test3 =Seconds_getTime(&secs);
    *date = timeConvert(secs);
}
bool timer1sec(Timer_Handle *timer,FT *data){
    Seconds_set(1651752452);
    Seconds_Module_startup();
    Timer_Params params;
    Timer_Params_init(&params);
    params.period = Clock_tickPeriod*1000;
    // Period of a tick
    params.periodType = ti_sysbios_interfaces_ITimer_PeriodType_MICROSECS;

    // Period type
    params.runMode = ti_sysbios_interfaces_ITimer_RunMode_CONTINUOUS;
    // Timer run mode
    params.startMode = ti_sysbios_interfaces_ITimer_StartMode_AUTO;
    params.arg = data;
    *timer=Timer_create(5,updatetime,&params,NULL);
    if (timer == NULL){
        return false;
    }
    return true;
}



