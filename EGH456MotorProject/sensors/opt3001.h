/*
 * opt3001.h
 *
 *  Created on: 23 May 2022
 *      Author: Baker
 */

#ifndef SENSORS_OPT3001_H_
#define SENSORS_OPT3001_H_

bool opt3001_init(void);
bool opt3001_read(uint16_t *rawData);
void opt3001_convert(uint16_t rawData, float *convertedLux);

#endif /* SENSORS_OPT3001_H_ */
