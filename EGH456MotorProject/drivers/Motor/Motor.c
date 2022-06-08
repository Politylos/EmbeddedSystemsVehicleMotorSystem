/*
 * Motor.c
 *
 *  Created on: 1 Jun. 2022
 *      Author: akank
 */

/*
 * Copyright (c) 2015, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*
 *  ======== empty_min.c ========
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
#include <ti/sysbios/gates/GateSwi.h>
 #include <ti/drivers/I2C.h>
#include <xdc/runtime/Error.h>
#include <xdc/runtime/Log.h>
#include <xdc/runtime/System.h>
#include <driverlib/sysctl.h>
#include <driverlib/pin_map.h>
#include <ti/sysbios/hal/Timer.h>
#include <math.h>

/* Board Header file */
#include "Board.h"
#include <drivers/motorlib.h>
#include "Interface/GraphFunctions.h"
#include "Interface/GraphData.h"
#include "Estop.h"

#define PERIOD  0.01 //ms
#define PWM_START  25
#define CHANGES_PER_REVOLUTION  26
#define MAX_ACCEL  500 //rpm/ms
#define MAX_DECEL  -500 //rpm/period
#define ESTOP_DECEL  -1*(1000) //rpm/period
#define STOP_DECEL  -1*(500) //rpm/period

Clock_Struct clk0Struct, clk1Struct;
Clock_Handle clk2Handle;

GateHwi_Handle gateHwi;
GateHwi_Params gHwiprms;

GateSwi_Handle SwiGate;
GateSwi_Params SwiPrams;

double VelocityBuffer[20];
double VelocitySecond=0;
int bufferpos = 0;
Swi_Handle SwiHandle;
double current_Des = 0;
int Hall_changed = 0;
double desired_speed = 1500; //rpm
bool estop = 0;
double pwm_current = PWM_START;
double error_sum = 0;
double current_speed = 0;
double current_speed_estop = 0;
double previous_speed = 0;
double previous_speed_estop = 0;
int noMove = 1;
bool Stop=1;
/*
 *  ======== clk0Fxn =======
 */
Void clk0Fxn(UArg arg0)
{
    UInt32 time;
    time = Clock_getTicks();
    /*if(time == 6800){
        estop = true;
    }*/
    Swi_post(SwiHandle); //every period
}
int freq2ticks(int freq){
    double time = ((double)1/(double)freq)*(double)1000000;
    int ticks = time/Clock_tickPeriod;
    return ticks;
}
/*
 *  ======== Initialise Motor ========
 */
void MotorInit(){
    Error_Block eb;
    Error_init(&eb);

    initMotorLib(100, &eb);
    setDuty(PWM_START);
    enableMotor();

    //trigger motor hall effect sensors
    updateMotor(0,0,0);

    //System_printf("Motor initialisation complete\n");
   // System_flush();
}

/*
 *  ======== eStop Motor ========
 */
void eStop(){ //input sensor values
    System_printf("Estop triggered\n");

    //if estop condition flag is true or event has occured
//    if(estop && current_speed != 0){
//                    desired_speed = 0;
//                    acceleration = ESTOP_DECEL;
//                    previous_speed = current_speed - ((acceleration*PERIOD)); //this is the problem should decrease by 200
//                    current_speed = (acceleration*PERIOD) + previous_speed;
//                    System_printf("estop current_speed RPM = %d, prev speed = %d\n", (int)current_speed, (int)previous_speed);
//            }
    desired_speed = 0;
    stopMotor(0); //1
    //disableMotor();

    System_flush();
}

/*
 *  ======== Hall Effect Sensor Hwi Functions ========
 */

void HAllF(unsigned int index){
    UInt gateKey;

    gateKey = GateHwi_enter(gateHwi);

    //read hall sensor GPIO pins
    bool HA = GPIO_read(EK_TM4C1294XL_HA);
    bool HB = GPIO_read(EK_TM4C1294XL_HB);
    bool HC = GPIO_read(EK_TM4C1294XL_HC);

    Hall_changed++; //increment counter

    updateMotor(HA,HB,HC);
    GateHwi_leave(gateHwi, gateKey);
}

double GetCurrentSpeed(){
    double revp = ((double)Hall_changed/(double)CHANGES_PER_REVOLUTION); //revolutions in period
    Hall_changed = 0; //reset
    return (revp*((double)1/((double)1/(double)100)))*(double)60; //speed in RPM
}

double GetDesiredSpeed(double user_desired_speed){ //from UI

    if(desired_speed <= 0){
        return 0;
    }
    else{
        return user_desired_speed;
    }
}

void MotorStart(){
    enableMotor();
    bool HA = GPIO_read(EK_TM4C1294XL_HA);
    bool HB = GPIO_read(EK_TM4C1294XL_HB);
    bool HC = GPIO_read(EK_TM4C1294XL_HC);
    updateMotor(HA,HB,HC);
    UInt gateKey;
    gateKey = GateHwi_enter(gateHwi);
    Hall_changed=0;
    Stop=0;
    GateHwi_leave(gateHwi, gateKey);
}

void PIControl(double current_speed, double desired_speed){

    double error = 0;
    double kp = 0.005;
    double ki = 0.000000005;
    double pwm = 0;
    int pwm_max = 95;
    int pwm_int = 0;

    if(1){
        error = desired_speed - current_speed;
        error_sum += error;

        pwm = (kp*error) + (ki*error_sum);
        pwm_int = (int)pwm;
       // System_printf("pwm = %d, error = %d, error_sum = %d\n", pwm_int, (int)error, (int)error_sum);
        pwm_current += pwm_int;
        if(pwm_current<0){
            pwm_current=0;
        }else if (pwm_current>95){
            pwm_current=95;
        }
        /*if(pwm_int > 5 && pwm_int < (pwm_max - pwm_current)){
            pwm_current += pwm_int;
          //  System_printf("1, current pwm = %d\n", (int)pwm_current);
        }
        else if(pwm_int == 0){
           // System_printf("2, current pwm = %d\n", (int)pwm_current);
        }
        else if(pwm_int > pwm_max){
            pwm_current += 1;
           // System_printf("3, current pwm = %d\n", (int)pwm_current);
        }
        else if(pwm_int <= 5){ //!stop &&
            pwm_current -= 1;
           // System_printf("4, current pwm = %d\n", (int)pwm_current);
        }
        else if(estop && pwm_int <= 0){
            if(current_speed <= 0){
                pwm_current = 0;
            }
            else{
                pwm_current -= pwm_int;
            }*/
           // System_printf("7, stop pwm = %d\n", (int)pwm_current);
            //setDuty(pwm_current);
        //}
        setDuty(pwm_current);
/*        //if less than 5 for too long the motor stops to spin - get out of there
        if((pwm_current <= pwm_max) && (pwm_current > 5)){
        //    System_printf("6, set current pwm = %d\n", (int)pwm_current);
            setDuty(pwm_current);
        }
        else if(estop){
        //    System_printf("6, set current pwm = %d\n", (int)pwm_current);
            setDuty(pwm_current);
        }*/
    }
}

/*
 *  ======== Speed Regulation Swi Function ========
 */
Void RegulateSpeed(UArg a0, UArg a1)
{
    UInt gateKey;
    UInt SwiKey;
    UInt32 time;



    /*get current speed*/
    time = Clock_getTicks();
    previous_speed = current_speed;
    gateKey = GateHwi_enter(gateHwi);
    current_speed = GetCurrentSpeed();
    GateHwi_leave(gateHwi, gateKey);

    //current_speed_estop = current_speed;
    //desired_speed = GetDesiredSpeed(user_desired_speed);
    //System_printf("time %lu, hall changed %d, current_speed RPM = %d, prev speed = %d\n",time, Hall_changed, (int)current_speed, (int)previous_speed);

    /*check for estop conditions*/

    /*calculate acceleration*/
    //if(time > 0 && time % 1000 == 0){//every second

        double acceleration = 0;//(speed_rate/(double)PERIOD)*100;
        SwiKey = GateSwi_enter(SwiGate);
        if((current_speed==0)&&(((!estop)&&(!Stop)))){
            MotorStart();

        }
        if (((estop)||(Stop))||((current_speed > (desired_speed+500))||(current_speed < (desired_speed-500)))){

            if(estop && current_speed != 0){
                acceleration = ESTOP_DECEL;
                current_Des = current_Des+((double)acceleration*(double)PERIOD);
            }else if (Stop && current_speed != 0){
                acceleration = STOP_DECEL;
                //previous_speed = previous_speed + ((acceleration*PERIOD)); //this is the problem should decrease by 200
                current_Des = current_Des+((double)acceleration*(double)PERIOD);
            }else if (current_speed < (desired_speed-100)){

                acceleration = MAX_ACCEL;
                current_Des = current_Des+((double)acceleration*(double)PERIOD);
            }else{
                acceleration = MAX_DECEL;
                current_Des = current_Des+((double)acceleration*(double)PERIOD);
            }
        }else{
            current_Des = desired_speed;
        }

        if(!estop){estop =checkEstop();}
        /*if(estop && current_speed != 0){
                acceleration = ESTOP_DECEL;
                //previous_speed = previous_speed + ((acceleration*PERIOD)); //this is the problem should decrease by 200
                current_speed = (acceleration*PERIOD) + previous_speed;
              //  System_printf("estop current_speed RPM = %d, prev speed = %d\n", (int)current_speed, (int)previous_speed);
        }else if (Stop && current_speed != 0){
            acceleration = STOP_DECEL;
            //previous_speed = previous_speed + ((acceleration*PERIOD)); //this is the problem should decrease by 200
            current_speed = (acceleration*PERIOD) + previous_speed;
        }
        else if(acceleration > (MAX_ACCEL*100)){
            acceleration = MAX_ACCEL;
            current_speed = (acceleration*PERIOD) + previous_speed;
            //System_printf("time %d, max acceleration -> speed changed %d\n",time, (int)current_speed);
        }
        else if(acceleration < (MAX_DECEL*100)){

            acceleration = MAX_DECEL;
            current_speed = (acceleration*PERIOD) + previous_speed;
          //  System_printf("time %d, min acceleration -> speed changed%d\n",time, (int)current_speed);
        }
        else{
        //    System_printf("time %d, normal acceleration %d, %d, %d\n",time, (int)acceleration, (int)(MAX_ACCEL*100), (int)(MAX_DECEL*100) );
        }*/



    /*estop control*/
    if((estop && current_speed <= 1000)){
      //  System_printf("motor stop\n");
        stopMotor(0); //1
        //disableMotor();
        estop=0;
        Stop=1;
        noMove=1;
        current_Des = 0;
        //estop =checkEstop();
    }
    else if((Stop) && (current_speed <= 500)){
        stopMotor(0);
        current_Des = 0;
        //PIControl(current_speed, current_Des); //desired speed is zero
    }
    else{
        PIControl(current_speed, current_Des);
    }
    GateSwi_leave(SwiGate, SwiKey);

    VelocityBuffer[bufferpos] = current_speed;
        if (bufferpos==19){
            VelocitySecond=CalcAvg(VelocityBuffer,5);
            if (VelocitySecond < 0){VelocitySecond=0;}
            ArrayUpdate(VelocityData,VelocitySecond);
        }
        bufferpos = (bufferpos+1)%20;


    //System_flush();
}
/*
 *  ======== main ========
 */
void MotorMain(void)
 {
    //Task_Params taskParams;
    Clock_Params clkParams;

    /* Call board init functions */

    int ticks = freq2ticks(100);
    /*enable Hall Effect Sensor GPIO pins */
    GPIO_enableInt(EK_TM4C1294XL_HA);
    GPIO_enableInt(EK_TM4C1294XL_HB);
    GPIO_enableInt(EK_TM4C1294XL_HC);

    /* Construct a periodic Clock Instance with period system time units */
    Clock_Params_init(&clkParams);
    clkParams.period = ticks;
    clkParams.startFlag = TRUE;

    Clock_construct(&clk0Struct, (Clock_FuncPtr)clk0Fxn, ticks/2, &clkParams);

    /*Create the Swi thread*/
    Swi_Params swiParams;
    Swi_Params_init(&swiParams);
    swiParams.priority = 13;
    SwiHandle = Swi_create(RegulateSpeed,&swiParams, NULL);

    /*Create Hwi Gate Mutex*/
    GateHwi_Params_init(&gHwiprms);
    gateHwi = GateHwi_create(&gHwiprms, NULL);
    if (gateHwi == NULL) {
        System_abort("Gate Hwi create failed");
    }

    GateSwi_Params_init(&SwiPrams);
    SwiGate = GateSwi_create(&SwiPrams, NULL);
        if (SwiGate == NULL) {
            System_abort("Gate Swi create failed");
        }


    if (SwiHandle == NULL) {
        System_abort("Swi create failed");
    }

    /*Initialise Motor*/
    MotorInit();

    /*Hall Effect Sensor HWI through GPIO*/
    GPIO_setCallback(EK_TM4C1294XL_HA, HAllF);
    GPIO_setCallback(EK_TM4C1294XL_HB, HAllF);
    GPIO_setCallback(EK_TM4C1294XL_HC, HAllF);

    //System_printf("main complete\n");
    //System_flush();

    /* Start BIOS */

}



