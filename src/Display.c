/*
 * Display.c
 *
 *  Created on: Sep 2, 2025
 *      Author: Omar Elsaghir
 */
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include "Display.h"
#include "tm4c123gh6pm.h"
#include "OLED.h"
#include "Wind_Measurements.h"

// State Defines
#define TIME            0
#define TEMPERATURE     1
#define HUMIDITY        2
#define PRESSURE        3
#define WIND            4
#define BRIGHTNESS      5

// Variables
uint8_t state;
uint8_t count = 0;

// 16x16 thermometer icon
const uint8_t thermometer_icon[32] = {
       0x01, 0xc0, 0x03, 0x60, 0x02, 0x30, 0x02, 0x30, 0x02, 0x30, 0x02, 0x30, 0x03, 0xf0, 0x03, 0xf0,
       0x03, 0xf0, 0x06, 0xf0, 0x07, 0xd8, 0x0d, 0xf8, 0x0d, 0xf8, 0x07, 0xd8, 0x07, 0x70, 0x01, 0xe0
};

// 16x16 humidity icon
const uint8_t humidity_icon[32] = {
       0x00, 0x00, 0x01, 0x80, 0x03, 0xc0, 0x03, 0xc0, 0x07, 0xe0, 0x0f, 0xf0, 0x0f, 0xf0, 0x1f, 0xf8,
       0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xd8, 0x1f, 0x98, 0x1f, 0x38, 0x0f, 0xf0, 0x07, 0xe0, 0x00, 0x00
};

// 16x16 down arrow icon
const uint8_t pressure_icon[32] = {
       0x07, 0xe0, 0x07, 0xe0, 0x07, 0xe0, 0x07, 0xe0, 0x07, 0xe0, 0x07, 0xe0, 0x07, 0xe0, 0x07, 0xe0,
       0x3f, 0xfc, 0x1f, 0xf8, 0x0f, 0xf0, 0x0f, 0xf0, 0x07, 0xe0, 0x03, 0xc0, 0x01, 0x80, 0x01, 0x80
};

// 16x16 sun icon
const uint8_t sun_icon[32] = {
       0x01, 0x80, 0x01, 0x80, 0x31, 0x8c, 0x38, 0x1c, 0x13, 0xc8, 0x07, 0xe0, 0x0f, 0xf0, 0xef, 0xf7,
       0xef, 0xf7, 0x0f, 0xf0, 0x07, 0xe0, 0x03, 0xc0, 0x38, 0x1c, 0x31, 0x8c, 0x01, 0x80, 0x01, 0x80
};

// 16x16 wind icon
const uint8_t wind_icon[32] = {
       0x00, 0x00, 0x00, 0x00, 0x03, 0xc0, 0x00, 0x0e, 0x78, 0x01, 0xff, 0x01, 0x00, 0x03, 0x2f, 0xfc,
       0x00, 0x00, 0xff, 0x9c, 0x00, 0xc2, 0x00, 0x4e, 0x02, 0x80, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00
};

// 32x32 sun icon
const uint8_t sun_32x32_bitmap[128] = {
    0x00, 0x00, 0x00, 0x00, 0x60, 0xF0, 0xF0, 0xE0, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1E, 0x3F,
    0x3F, 0x1E, 0x00, 0x00, 0x00, 0x00, 0x00, 0xC0, 0xE0, 0xF0, 0xF0, 0x60, 0x00, 0x00, 0x00, 0x00,
    0x80, 0xC0, 0xC0, 0xC0, 0xC0, 0x80, 0x01, 0x01, 0xE1, 0xF8, 0xFC, 0xFE, 0xFE, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFE, 0xFE, 0xFC, 0xF8, 0xE1, 0x01, 0x01, 0x80, 0xC0, 0xC0, 0xC0, 0xC0, 0x80,
    0x01, 0x03, 0x03, 0x03, 0x01, 0x01, 0x80, 0x80, 0x87, 0x1F, 0x3F, 0x7F, 0x7F, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0x7F, 0x7F, 0x3F, 0x1F, 0x87, 0x80, 0x80, 0x01, 0x01, 0x03, 0x03, 0x03, 0x01,
    0x00, 0x00, 0x00, 0x00, 0x06, 0x0F, 0x0F, 0x07, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x78, 0xFC,
    0xFC, 0x78, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x07, 0x0F, 0x0F, 0x06, 0x00, 0x00, 0x00, 0x00,
};

void display_sequence(void)
{
    switch(state)
    {
        case TIME:
            snprintf(str, sizeof(str), "%02d:%02d:%02d", hours, minutes, seconds);
            ssd1306_printAtSize(2, 16, str, 2);
            ssd1306_printAtSize(5, 16, "Sat, Sep 6, 2025", 1);
            if(count == 15)
            {
                state = TEMPERATURE;
                count = 0;
                ssd1306_clearDisplay();
            }
            break;
        case TEMPERATURE:
            ssd1306_drawBitmapXy(16, 16, thermometer_icon, 16, 16, BMP_ROW_MSB_FIRST);
            snprintf(buff, sizeof(buff), "%.2f", temperature);
            ssd1306_printAtSize(2, 32, buff, 2);
            ssd1306_printAtSize(2, 96, "C", 1);

            ssd1306_setCursor(5, 0);
            snprintf(buff, sizeof(buff), " Pressure:%.2f hPa", pressure);
            ssd1306_print(buff);

            ssd1306_setCursor(6, 0);
            snprintf(buff, sizeof(buff), " Humidity: %.2f %%", humidity);
            ssd1306_print(buff);
            if(count == 15)
            {
                state = HUMIDITY;
                count = 0;
                ssd1306_clearDisplay();
            }
            break;
        case HUMIDITY:
            ssd1306_drawBitmapXy(16, 16, humidity_icon, 16, 16, BMP_ROW_MSB_FIRST);
            snprintf(buff, sizeof(buff), "%.2f", humidity);
            ssd1306_printAtSize(2, 32, buff, 2);
            ssd1306_printAtSize(2, 96, "%", 1);

            ssd1306_setCursor(5, 0);
            snprintf(buff, sizeof(buff), " Temperature:%.2f C", temperature);
            ssd1306_print(buff);

            ssd1306_setCursor(6, 0);
            snprintf(buff, sizeof(buff), " Pressure:%.2f hPa", pressure);
            ssd1306_print(buff);

            if(count == 15)
            {
                state = PRESSURE;
                count = 0;
                ssd1306_clearDisplay();
            }
            break;
        case PRESSURE:
            ssd1306_drawBitmapXy(16, 16, pressure_icon, 16, 16, BMP_ROW_MSB_FIRST);
            snprintf(buff, sizeof(buff), "%.2f", pressure);
            ssd1306_printAtSize(2, 32, buff, 2);
            ssd1306_printAtSize(2, 108, "hPa", 1);

            ssd1306_setCursor(5, 0);
            snprintf(buff, sizeof(buff), " Temperature:%.2f C", temperature);
            ssd1306_print(buff);

            ssd1306_setCursor(6, 0);
            snprintf(buff, sizeof(buff), " Humidity: %.2f %%", humidity);
            ssd1306_print(buff);
            if(count == 15)
            {
                state = WIND;
                count = 0;
                ssd1306_clearDisplay();
            }
            break;
        case WIND:
            ssd1306_drawBitmapXy(24, 8, wind_icon, 16, 16, BMP_ROW_MSB_FIRST);
            ssd1306_printAtSize(1, 48, "Data", 2);
            ssd1306_setCursor(4, 0);
            snprintf(str, sizeof(str), " Wind speed:%.2f m/s\n", wind_speed);
            ssd1306_print(str);

            ssd1306_setCursor(5, 0);
            snprintf(str, sizeof(str), " Wind Direction: %.2f (%s)", angle, get_wind_direction(angle));
            ssd1306_print(str);
            if(count == 15)
            {
                state = BRIGHTNESS;
                count = 0;
                ssd1306_clearDisplay();
            }
            break;
        case BRIGHTNESS:
            ssd1306_drawBitmapXy(32, 8, sun_icon, 16, 16, BMP_ROW_MSB_FIRST);
            snprintf(str, sizeof(str), "%d %%", brightness);
            ssd1306_printAtSize(1, 56, str, 2);

            ssd1306_setCursor(5, 0);
            snprintf(buff, sizeof(buff), " Temperature: %.2f C", temperature);
            ssd1306_print(buff);

            ssd1306_setCursor(6, 0);
            snprintf(buff, sizeof(buff), " Humidity: %.2f %%", humidity);
            ssd1306_print(buff);
            if(count == 15)
            {
                state = TIME;
                count = 0;
                ssd1306_clearDisplay();
            }
            break;

    }

    count++;

}
