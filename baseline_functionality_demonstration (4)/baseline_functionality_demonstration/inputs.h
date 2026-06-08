#pragma once
/*/////////////////////////////////////////////////////////////
// Author: Petras Swissler
// Description: Functions related to robot measurement
*//////////////////////////////////////////////////////////////
// Includes
#include "def_pins.h"
#include "def_config.h"
#include <Arduino.h>
#include <Wire.h>
#include "robot.h"
#include <Adafruit_MAX1704X.h>  
#include <Adafruit_TCS34725.h>
#include <Adafruit_MPU6050.h>
#include <QMC5883LCompass.h>
// #include <Adafruit_NeoPXL8.h>
#include <Adafruit_NeoPixel.h>
#include <Adafruit_TestBed.h>
/////////////////////////////////////////////////////////////////////////
// Functions
void inputsFromDevices(robot* controller, Adafruit_MAX17048*  lipo_monitor, Adafruit_MPU6050*   imu, Adafruit_TCS34725*  color_sensor, QMC5883LCompass*    compass);;
void inputsFromHardwarePins(robot* controller);
void inputs(robot* controller, Adafruit_MAX17048*  lipo_monitor, Adafruit_MPU6050*   imu, Adafruit_TCS34725*  color_sensor, QMC5883LCompass*    compass);
/////////////////////////////////////////////////////////////////////////