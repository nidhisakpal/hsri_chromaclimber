#pragma once
/*/////////////////////////////////////////////////////////////
// Author: Petras Swissler
// Description: Setup functions
*//////////////////////////////////////////////////////////////
// Includes
#include "def_pins.h"
#include "def_config.h"
#include <Arduino.h>
//#include <esp32-hal-ledc.h> 
#include <Wire.h>
#include <Adafruit_MAX1704X.h>  
#include <Adafruit_TCS34725.h>
#include <Adafruit_MPU6050.h>
#include <QMC5883LCompass.h>
// #include <Adafruit_NeoPXL8.h>
#include <Adafruit_NeoPixel.h>

/////////////////////////////////////////////////////////////////////////
// Functions
void pinConfig(void);
void serialSetup(void);

void boardSetup(Adafruit_NeoPixel*  rgb_led, Adafruit_MAX17048*  lipo_monitor, 
                Adafruit_MPU6050*   imu, Adafruit_TCS34725*  color_sensor, 
                QMC5883LCompass*    compass);
/////////////////////////////////////////////////////////////////////////