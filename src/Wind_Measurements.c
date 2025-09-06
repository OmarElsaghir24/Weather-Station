/*
 * Wind_Measurements.c
 *
 *  Created on: Aug 29, 2025
 *      Author: Omar Elsaghir
 *
 *      Adafruit Anemometer Pinout:
 *      Blue wire -> PE0
 *      Brown wire -> VCC (7-24V from voltage source)
 *      Black wire -> GND (GND from voltage source connected to TM4C123, then TM4C123 GND to anemometer GND)
 *
 *      AS5600 Magnetic Encoder Pinout:
 *      SCL -> PE4
 *      SDA -> PE5
 *      DIR -> GND
 *
 */
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include "gpio.h"
#include "adc0.h"
#include "uart0.h"
#include "i2c2.h"
#include "tm4c123gh6pm.h"

// Defines
#define ANEMOMETER  PORTE, 0

#define AS5600_ADDR   0x36
#define AS5600_ANGLE_H 0x0E
#define AS5600_ANGLE_L 0x0F

// Global Variables
uint16_t sensor_value;
float sensorVoltage;
float windSpeed;
float voltageConversionConstant = 3.3f / 4096.0f; // ~0.000805
uint16_t sensorDelay = 1000;
float minVoltage = 0.4;
float minWindSpeed = 0.0;
float maxVoltage = 2.0;
float maxWindSpeed = 32;
char str[100];

// Configure 12-bit ADC for anemometer
void initWindSensor(void)
{
    enablePort(PORTE);
    selectPinAnalogInput(ANEMOMETER);
    // Initializes analog to digital converter
    initAdc0Ss3();
    // Use AIN3 input with N=4 hardware sampling
    setAdc0Ss3Mux(3);
    setAdc0Ss3Log2AverageCount(2);
}

// Configure I2C2 for wind vane (AS5600 chip)
void initWindVane(void)
{
    initI2c2();
}

// Read 12-bit ADC value, convert to voltage, then convert to wind speed based on calculated voltage
float getWindSpeed(void)
{
    sensor_value = readAdc0Ss3();

    sensorVoltage = sensor_value * (3.3f / 4096.0f);
    //snprintf(str, sizeof(str), "Raw wind data: %.2f\n", sensorVoltage);
    //putsUart0(str);

    if(sensorVoltage <= minVoltage) {
        windSpeed = 0;
    }else {
        windSpeed = (sensorVoltage - minVoltage) * maxWindSpeed / (maxVoltage - minVoltage);
    }

    return windSpeed;
}

// Optional: quick probe
bool as5600_present(void)
{
    return pollI2c2Address(AS5600_ADDR);
}

// Read 12-bit angle (0..4095)
uint16_t as5600_read_raw_angle(void)
{
    uint8_t hi = readI2c2Register(AS5600_ADDR, AS5600_ANGLE_H);
    uint8_t lo = readI2c2Register(AS5600_ADDR, AS5600_ANGLE_L);
    return ((uint16_t)hi << 8 | lo) & 0x0FFF;
}

// Convert to degrees and apply zero-offset
float as5600_raw_to_degrees(uint16_t raw)
{
    return (raw * 360.0f) / 4096.0f;
}

// One-time calibration: point vane to North and store offset
float as5600_calibrate_zero_deg(void)
{
    uint16_t raw = as5600_read_raw_angle();
    return as5600_raw_to_degrees(raw); // save as zero_offset_deg (e.g., in flash)
}

// Runtime: get corrected 0..360°
float as5600_get_heading_deg(float zero_offset_deg)
{
    uint16_t raw = as5600_read_raw_angle();
    float deg = as5600_raw_to_degrees(raw) - zero_offset_deg;
    if (deg < 0) deg += 360.0f;
    if (deg >= 360.0f) deg -= 360.0f;
    return deg;
}

// Determine direction of wind based on what angle is measured
const char* get_wind_direction(float angle)
{
    static const char* dirs[] = {"N", "NE", "E", "SE", "S", "SW", "W", "NW"};
    int sector = (int)((angle + 22.5f) / 45.0f) % 8;
    return dirs[sector];
}
