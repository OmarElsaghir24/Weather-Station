# Weather-Station
Implemented a weather station using the TM4C123GH6PM microcontroller to measure and log environmental conditions such as temperature, humidity, pressure, wind speed, wind direction, and light level.

![Weather Station](https://github.com/user-attachments/assets/8407a511-e005-464e-952c-06a0320b69a9)

# Hardware Components
|                                             |
|---------------------------------------------|
| TM4C123GH6PM Tiva Board |
| BME280 Sensor |
| Adafruit Anemometer |
| AS5600 Magnetic Encoder Sensor |
| 6x2 mm Diametric Magnet (Comes with AS5600) |
| Light-Dependent Resistor (LDR) |
| SSD1306 OLED Display |
| DIY Designed Wind Vane (in progress) |

# Peripherals Used
|                         |
|-------------------------|
| ADC0 (Sample Sequencer) |
| ADC1 (Sample Sequencer) |
| I2C0 (BME280 Sensor) |
| I2C1 (OLED Display) |
| I2C2 (AS5600 Sensor) |
| GPIO |
| UART |
| Timers |
| Hibernation |

# OLED Display Output
* Curent time:
  
![Display 1](https://github.com/user-attachments/assets/658e65d1-285d-466a-9c85-1b2447a8b160)

* Temperature:

![Display 2](https://github.com/user-attachments/assets/e3162fe9-a007-42d9-853c-c74070093996)

* Humidity:
  
![Display 3](https://github.com/user-attachments/assets/9ed5d35b-9ca6-4912-a03f-e78451e7ce87)

* Pressure:
  
![Display 4](https://github.com/user-attachments/assets/9d8e27dd-bf80-402a-8286-a4b4ae04e6c1)

* Wind Speed and Direction:
  
![Display 5](https://github.com/user-attachments/assets/664000f6-2117-49a0-ad9d-2e101a4605d9)

* Brightness:
  
![Display 6](https://github.com/user-attachments/assets/12ccc807-b53e-4e26-8580-4eaa9af351e3)






