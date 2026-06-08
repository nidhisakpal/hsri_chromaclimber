#pragma once
/*/////////////////////////////////////////////////////////////
// Author: Petras Swissler
// Description: Functions related to robot measurement
*//////////////////////////////////////////////////////////////
// Includes
#include "outputs.h"

/////////////////////////////////////////////////////////////////////////
void outputs(robot* controller, Adafruit_NeoPixel*  rgb_led)
{
    safetyControl(controller);
    setLEDs(controller, rgb_led);
    runMotors(controller->drive_forward, controller->drive_spin);
}

/////////////////////////////////////////////////////////////////////////
void setLEDs(robot* controller, Adafruit_NeoPixel*  rgb_led)
{
    rgb_led->begin();
    rgb_led->setPixelColor(controller->rgb_r, controller->rgb_g, controller->rgb_b, 0);
    rgb_led->show();

    digitalWrite(DO_COLOR_SENSOR_LED, controller->color_sensor_white_led);
}

void runMotors(float forward, float spin)
{

    // Assume: forward and spin are -1...1 
    // Assume: PWM is using 8-bit resolution
    float a_speed = max(min(255 * forward + 255 * spin, (float)255), (float)-255); 
    float b_speed = max(min(255 * forward - 255 * spin, (float)255), (float)-255);
   
    if((a_speed == 0)&&(b_speed == 0))
    {
        // Serial.print("Disabled");
        // Serial.println();
        // NOTE: Change to HIGH
        digitalWrite(DO_HB_SLP, LOW);
    }
    else
    {
        // NOTE: Change to LOW
        digitalWrite(DO_HB_SLP, HIGH);
        // Serial.print("Enabled");
        // Serial.println();
        if(a_speed > 0)
        {
            Serial.print(F("Case_A1:         "));
            Serial.print(a_speed);
            Serial.println();
            analogWrite(DO_HB_AIN1, fabs(a_speed));
            analogWrite(DO_HB_AIN2, 0);
            //ledcWrite(PWM_CH_A1, fabs(a_speed));
            //ledcWrite(PWM_CH_A2, 0);
        }
        else
        {
            Serial.print(F("Case_A2:         "));
            Serial.print(a_speed);
            Serial.println();
            analogWrite(DO_HB_AIN1, 0);
            analogWrite(DO_HB_AIN2, fabs(a_speed));
            
            //ledcWrite(PWM_CH_A1, 0);
            //ledcWrite(PWM_CH_A2, fabs(a_speed));
        }

        if(b_speed > 0)
        {
            Serial.print(F("Case_B1:         "));
            Serial.print(b_speed);
             Serial.println();
            analogWrite(DO_HB_BIN1, fabs(b_speed));
            analogWrite(DO_HB_BIN2 , 0);
        }
        else
        {
            Serial.print(F("Case_B2:         "));
            Serial.print(b_speed);
            Serial.println();
            analogWrite(DO_HB_BIN1, 0);
            analogWrite(DO_HB_BIN2 , fabs(b_speed));
        }
    }    
}

void safetyControl(robot* controller)
{
    // TODO: Add some sort of logic to prevent rapid changes in motor directions
}