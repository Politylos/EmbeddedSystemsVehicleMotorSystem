/*
 * EStop.c
 *
 *  Created on: 31 May 2022
 *      Author: polit
 */
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>

#include <ti/sysbios/knl/Event.h>
#include "Estop.h"

#define TIMEOUT         1      /* Timeout value */

Event_Struct evtStruct;
Event_Handle EstopEvent;
uint16_t AccelStop = Event_Id_00;
uint16_t PowerEstop = Event_Id_01;
UInt postedStop;
UInt posted;
bool EStop=0;
void initEstop(){
    Event_construct(&evtStruct, NULL);
    EstopEvent = Event_handle(&evtStruct);
}
void EstopAccel(){
    Event_post(EstopEvent, AccelStop);
}
void EstopPower(){
    Event_post(EstopEvent, PowerEstop);
}
bool checkEstop(){
    postedStop = Event_pend(EstopEvent,
                        0, AccelStop + PowerEstop,/* orMask */
                        TIMEOUT);
    return postedStop;


}

