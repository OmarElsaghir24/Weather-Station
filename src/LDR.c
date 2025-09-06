/*
 * LDR.c
 *
 *  Created on: Aug 25, 2025
 *      Author: Omar Elsaghir
 */
#include <inttypes.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "gpio.h"
#include "adc1.h"
#include "uart0.h"

#define LDR PORTE, 3
char str[100];

void initLDR(void)
{

    enablePort(PORTE);

    // Set the Light Dependent Resistor as an input to AIN0
    selectPinAnalogInput(LDR);

    // Initializes analog to digital converter
    initAdc1Ss3();
    // Use AIN0 input with N=4 hardware sampling
    setAdc1Ss3Mux(0);
    setAdc1Ss3Log2AverageCount(2);
}

uint8_t time_of_day(void) {

    uint16_t ldr_raw = readAdc1Ss3();
    //uint8_t index;
    //static const char * time[] = {"Daytime", "Night time"};
    snprintf(str, sizeof(str), " LDR: %d\n", ldr_raw);
    putsUart0(str);

    uint8_t brightness = ((4095 - ldr_raw) * 100) / 4095;
    snprintf(str, sizeof(str), " Brightness: %d %%\n", brightness);
    putsUart0(str);

/*        if(ldr_raw < 3300)
        {
            index = 0;
        }
        else if(ldr_raw >= 3400)
        {
            index = 1;
        }

        return time[index];*/
    return brightness;
}
