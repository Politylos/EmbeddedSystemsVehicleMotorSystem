/*
 * GraphData.c
 *
 *  Created on: 30 May 2022
 *      Author: polit
 */
#include "GraphData.h"
bool LightGraph = false;
bool AccelGraph = false;
double LightData[GRAPHBUFFER];
double AccelData[GRAPHBUFFER];
void InitsingleBuffer(double *buffer,int size){
    int i;
    for (i=0;i<size;i++){
        buffer[i]=0;
    }
}
void initGraphBuffers(){
    InitsingleBuffer(LightData,GRAPHBUFFER);
    InitsingleBuffer(AccelData,GRAPHBUFFER);
}
