#pragma once
/*/////////////////////////////////////////////////////////////
// Author: Petras Swissler
// Description: To keep the main file clean, throw all the 
                includes into this file
*//////////////////////////////////////////////////////////////
#define DEFAULT_I2C_PORT &Wire
// Some boards have TWO I2C ports, how nifty. We should scan both
#if defined(ARDUINO_ARCH_RP2040) \
    || defined(ARDUINO_ADAFRUIT_QTPY_ESP32S2) \
    || defined(ARDUINO_ADAFRUIT_QTPY_ESP32S3_NOPSRAM) \
    || defined(ARDUINO_ADAFRUIT_QTPY_ESP32S3) \
    || defined(ARDUINO_ADAFRUIT_QTPY_ESP32_PICO) \
    || defined(ARDUINO_SAM_DUE)
  #define SECONDARY_I2C_PORT &Wire1
#endif

// Arduino and Standard Libraries -----------
#include <Arduino.h>
#include <Wire.h>
#include <stdint.h>

// External Libraries -----------------------
//Battery Monitor
#include <Adafruit_MAX1704X.h>  
// RGB Color Sensor
#include <Adafruit_TCS34725.h>
// 6DOF IMU
#include <Adafruit_MPU6050.h>
// 3DOF Magnetometer
#include <QMC5883LCompass.h>
// RGB LED
// #include <Adafruit_NeoPXL8.h>
#include <Adafruit_NeoPixel.h>
// Adafruit Stuff
#include <Adafruit_TestBed.h>

// Basic Definitions ------------------------
#include "def_friendlynames.h"

// Bespoke Libraries ------------------------
#include "robot.h"
#include "user_control.h"
#include "outputs.h"
#include "setup.h"
#include "inputs.h"
