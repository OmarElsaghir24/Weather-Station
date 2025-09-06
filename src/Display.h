/*
 * Display.h
 *
 *  Created on: Sep 2, 2025
 *      Author: Omar Elsaghir
 */

#ifndef DISPLAY_H_
#define DISPLAY_H_

#include <inttypes.h>
#include <ctype.h>
#include "wait.h"

float angle, wind_speed;
uint16_t wind_deg_raw;
char buff[50];
int32_t temp, hum, pres;
float temperature, humidity, pressure;
uint32_t hours, minutes, seconds;
uint32_t remaining_seconds, realTime;
char str[100];
uint8_t brightness;

void display_sequence(void);

#endif /* DISPLAY_H_ */
