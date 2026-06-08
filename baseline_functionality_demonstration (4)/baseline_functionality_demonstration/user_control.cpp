#pragma once
/*/////////////////////////////////////////////////////////////
// Author: Petras Swissler
// Description: Functions related to robot measurement
*//////////////////////////////////////////////////////////////
// Includes
#include "user_control.h"
/////////////////////////////////////////////////////////////////////////
// Functions
void serialInterjection(robot* controller)
{
    // Use the serial port to control the robot. Below is a simple example
    if(Serial.available())
    {
        char user_input = Serial.read();
        clearSerialBuffer();
        if(user_input == 'x')
        {
            Serial.println(F("User has input an X"));
            delay(1000);
        }
    }
}
/////////////////////////////////////////////////////////////////////////