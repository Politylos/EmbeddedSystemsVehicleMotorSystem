/*
 * GraphData.h
 *
 *  Created on: 30 May 2022
 *      Author: polit
 */

#ifndef INTERFACE_GRAPHDATA_H_
#define INTERFACE_GRAPHDATA_H_
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#define GRAPHBUFFER 50
extern bool LightGraph;
extern double LightData[GRAPHBUFFER];
extern void initGraphBuffers();
extern bool AccelGraph;
extern double AccelData[GRAPHBUFFER];


#endif /* INTERFACE_GRAPHDATA_H_ */
