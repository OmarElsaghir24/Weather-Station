

/**
 * main.c
 */
#include <inttypes.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "tm4c123gh6pm.h"
#include "clock.h"
#include "gpio.h"
#include "uart0.h"
#include "wait.h"
#include "i2c0.h"
#include "i2c1.h"
#include "adc0.h"
#include "adc1.h"
#include "BME280.h"
#include "LDR.h"
#include "OLED.h"
#include "Wind_Measurements.h"
#include "Display.h"

// Variables
BME280_CalibData calibration;
char str[100];

// Bitband Aliases
#define ANEMOMETER           (*((volatile uint32_t *)(0x42000000 + (0x400073FC-0x40000000)*32 + 3*4)))
#define SW1                  (*((volatile uint32_t *)(0x42000000 + (0x400253FC-0x40000000)*32 + 4*4)))

#define ANEMOMETER_MASK 8
#define SW1_MASK 16

// Timer 0 ISR
void timer0Isr(void)
{

    wind_speed = getWindSpeed();

    wind_deg_raw = as5600_read_raw_angle();
    angle = as5600_raw_to_degrees(wind_deg_raw);
    //angle = as5600_get_heading_deg(0);

    while(!(HIB_CTL_R & HIB_CTL_WRC));
    realTime = HIB_RTCC_R % 86400;
    hours = realTime / 3600;
    remaining_seconds = realTime % 3600;
    minutes = remaining_seconds / 60;
    seconds = remaining_seconds % 60;

    brightness = time_of_day();

    display_sequence();

    TIMER0_ICR_R = TIMER_ICR_TATOCINT;  // clear timer interrupt
}

// Timer 1 ISR
void timer1Isr(void)
{
    float pressure_Pa;

    readBME280Calibration();
    readBME280Raw(&temp, &pres, &hum);

    temperature = compensateTemperature(temp);
    pressure_Pa = compensatePressure(pres);
    humidity = compensateHumidity(hum);

    pressure = pressure_Pa / 100.0f;          // Divide by 100 to convert from Pa to hPa

    TIMER1_ICR_R = TIMER_ICR_TATOCINT;       // clear interrupt flag
}

// Initialize hardware (system clock and timers)
void initHw(void)
{

    // Initialize system clock to 40 MHz
    initSystemClockTo40Mhz();

    // Enable clocks
    enablePort(PORTF);
    SYSCTL_RCGCTIMER_R |= SYSCTL_RCGCTIMER_R0 | SYSCTL_RCGCTIMER_R1;
    SYSCTL_RCGCHIB_R |= SYSCTL_RCGCHIB_R0;
    _delay_cycles(3);

    // Configure PF4 (SW1) as input with pull-up resistor
    GPIO_PORTF_DIR_R &= ~SW1_MASK;       // Set PF4 as input
    GPIO_PORTF_PUR_R |= SW1_MASK;        // Enable pull-up resistor on PF4
    GPIO_PORTF_DEN_R |= SW1_MASK;        // Enable digital function on PF4

    // Configure Timer 0 as the time base for wind measurements
    TIMER0_CTL_R &= ~TIMER_CTL_TAEN;                 // turn-off timer before reconfiguring
    TIMER0_CFG_R = TIMER_CFG_32_BIT_TIMER;           // configure as 32-bit timer (A+B)
    TIMER0_TAMR_R = TIMER_TAMR_TAMR_PERIOD;          // configure for periodic mode (count down)
    TIMER0_TAILR_R = 40000000;                       // set load value to 80e6 for 2 Hz interrupt rate
    TIMER0_IMR_R = TIMER_IMR_TATOIM;                 // turn-on interrupts
    TIMER0_CTL_R |= TIMER_CTL_TAEN;                  // turn-on timer
    NVIC_EN0_R = 1 << (INT_TIMER0A-16);              // turn-on interrupt 35 (TIMER0A)

    // Configure Timer 1 as the time base for BME280 sensor
    TIMER1_CTL_R &= ~TIMER_CTL_TAEN;                 // turn-off timer before reconfiguring
    TIMER1_CFG_R = TIMER_CFG_32_BIT_TIMER;           // configure as 32-bit timer (A+B)
    TIMER1_TAMR_R = TIMER_TAMR_TAMR_PERIOD;          // configure for periodic mode (count down)
    TIMER1_TAILR_R = 80000000;                       // set load value to 80e6 for 2 Hz interrupt rate
    TIMER1_IMR_R = TIMER_IMR_TATOIM;                 // turn-on interrupts
    TIMER1_CTL_R |= TIMER_CTL_TAEN;                  // turn-on timer
    NVIC_EN0_R = 1 << (INT_TIMER1A-16);              // turn-on interrupt 37 (TIMER1A)

    // Configure RTC
    while(!(HIB_CTL_R & HIB_CTL_WRC));
    HIB_CTL_R |= HIB_CTL_CLK32EN;            // Enables Hibernation mode clock source
    while(!(HIB_CTL_R & HIB_CTL_WRC));
    HIB_CTL_R |= HIB_CTL_RTCEN;
    //while(!(HIB_CTL_R & HIB_CTL_WRC));
    //HIB_IM_R |= HIB_IM_RTCALT0;
    //while(!(HIB_CTL_R & HIB_CTL_WRC));
    //NVIC_EN1_R = 1 << (INT_HIBERNATE-16-32);
}

int main(void)
{
    initHw();
	initUart0();
	initI2c1();
	initBME280();
	initLDR();
	initWindSensor();
	initWindVane();

	init_ssd1306();
	ssd1306_clearDisplay();

	setUart0BaudRate(115200, 40e6);

	while(!(HIB_CTL_R & HIB_CTL_WRC));
	HIB_RTCLD_R = 10800;

    while(true);
}
