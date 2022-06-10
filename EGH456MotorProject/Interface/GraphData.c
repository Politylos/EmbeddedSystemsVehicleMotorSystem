/*
 * GraphData.c
 *
 *  Created on: 30 May 2022
 *      Author: polit
 */
#include "GraphData.h"
bool LightGraph = false;
bool AccelGraph = false;
bool CurrentGraph = false;
bool VelocityGraph = false;
double LightData[GRAPHBUFFER];
double AccelData[GRAPHBUFFER];
double VelocityData[GRAPHBUFFER];
double CurrentData[GRAPHBUFFER];
//Used to set a given buffer to all 0 so all data is known
void InitsingleBuffer(double *buffer,int size){
    int i;
    for (i=0;i<size;i++){
        buffer[i]=0;
    }
}
//used to init all buffer to 0 so that the data is known
void initGraphBuffers(){
    InitsingleBuffer(LightData,GRAPHBUFFER);
    InitsingleBuffer(AccelData,GRAPHBUFFER);
    InitsingleBuffer(VelocityData,GRAPHBUFFER);
    InitsingleBuffer(CurrentData,GRAPHBUFFER);
}
