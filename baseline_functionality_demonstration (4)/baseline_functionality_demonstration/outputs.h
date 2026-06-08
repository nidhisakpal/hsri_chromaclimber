#pragma once
/*/////////////////////////////////////////////////////////////
// Author: Petras Swissler
// Description: Functions related to robot measurement
*//////////////////////////////////////////////////////////////
// Includes
#include "def_pins.h"
#include "def_config.h"
#include "robot.h"
#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
/////////////////////////////////////////////////////////////////////////
// Functions
void setLEDs(robot* controller, Adafruit_NeoPixel*  rgb_led);
void runMotors(float forward, float spin);
void safetyControl(robot* controller);

void outputs(robot* controller, Adafruit_NeoPixel*  rgb_led);
/////////////////////////////////////////////////////////////////////////