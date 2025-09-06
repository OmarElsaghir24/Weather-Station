/*
 * Wind_Measurements.h
 *
 *  Created on: Aug 29, 2025
 *      Author: Omar Elsaghir
 */

#ifndef WIND_MEASUREMENTS_H_
#define WIND_MEASUREMENTS_H_

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

void initWindSensor(void);
float getWindSpeed(void);
void initWindVane(void);
bool as5600_present(void);
uint16_t as5600_read_raw_angle(void);
float as5600_raw_to_degrees(uint16_t raw);
float as5600_calibrate_zero_deg(void);
float as5600_get_heading_deg(float zero_offset_deg);
const char* get_wind_direction(float angle);

#endif /* WIND_MEASUREMENTS_H_ */
