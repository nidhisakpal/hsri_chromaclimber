/*//////////////////////////////////////////////////////////////
// Author: Petras Swissler 
// Description: Example code to start from for the breakout-
                based robot using the Adafruit ESP32-S3 Feather
*///////////////////////////////////////////////////////////////
// Include Libraries
#include "def_includes.h"
//#include <Adafruit_NeoPixel.h>


// Global Variables
Adafruit_MAX17048  lipo_monitor;
Adafruit_NeoPixel  rgb_led(1, PIN_NEOPIXEL, NEO_GRB + NEO_KHZ800);
Adafruit_MPU6050   imu;
Adafruit_TCS34725  color_sensor(TCS34725_INTEGRATIONTIME_60MS, TCS34725_GAIN_1X);
QMC5883LCompass    compass;

robot controller;

///////////////////////////////////////////////////////////////
// Setup
void setup() 
{
    boardSetup(&rgb_led, &lipo_monitor, &imu, &color_sensor, &compass);
    controller.behavior =  BEHAVIOR_SHADOW_INSTRUCTION;
}

///////////////////////////////////////////////////////////////
// Interrupts here

///////////////////////////////////////////////////////////////
// Main Loop
void loop() 
{

    //serialInterjection(&controller);
    //commHandlerIn();
    //serverHandler();
    inputs(&controller, &lipo_monitor, &imu, &color_sensor, &compass);
    controller.runLogic();
    outputs(&controller, &rgb_led);
    //commHandlerOut();
}
