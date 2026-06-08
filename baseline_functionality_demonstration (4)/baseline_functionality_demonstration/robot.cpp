#pragma once
/*/////////////////////////////////////////////////////////////
// Author: Petras Swissler
// Description: Basically a container for robot variables and
                other logic.
*//////////////////////////////////////////////////////////////
// Include Libraries
#include <Arduino.h>
#include "robot.h"
// Methods -------------------------------------------
// Instantiates class
robot::robot(void) {}
robot::~robot(void) {}

// Logic methods
void robot::runLogic(void)
{
    resetOutputsToNull();

    // This is the main logic function
    switch(behavior)
    {
        case BEHAVIOR_IDLE:
            // Do Nothing
            break;

        case BEHAVIOR_HEARTBEAT:
            behaviorReport();
            break;

        case BEHAVIOR_EXAMPLE:
            behaviorExample();
            break;

        case COLOR_SENSE_TEST:
            whats_the_color();
            break;

        case WHERE_TO_GO:
            move_test();
            break;

        case BEHAVIOR_SPATIAL_PROGRAM:
            behavior_spatial_programming();
            break;

        case BEHAVIOR_SHADOW_INSTRUCTION:
            shadow_instructions();
            break;
        
        case BEHAVIOR_FRACTIAL_PLAID_GPS:
            f_p_gps();
            break;

        default:
            // Do Nothing
            break;
    }
}

////////////////////////////////////////////////////////////////
// Behavior methods -------------------------------------------
///////////////////////////////////////////////////////////////
void robot::resetOutputsToNull(void)
{
    drive_forward = 0;
    drive_spin = 0;

    rgb_r = 0;
    rgb_g = 0;
    rgb_b = 0;
    color_sensor_white_led = 0;
}

void robot::behaviorReport(void)
{
    // Print all sensor information then delay for some duration
    drawHoriz(20, '#');
    Serial.print(F("Time, dt:           "));
    Serial.print(this_time);
    Serial.print(F(", "));
    Serial.print(dt);
    Serial.println();

    Serial.print(F("Accelerometer xyz:  "));
    Serial.print(ax);
    Serial.print(F(", "));
    Serial.print(ay);
    Serial.print(F(", "));
    Serial.print(az);
    Serial.println();

    Serial.print(F("Theta:  "));
    Serial.print(_theta);
    Serial.println();

    Serial.print(F("Gyroscope xyz:      "));
    Serial.print(gx);
    Serial.print(F(", "));
    Serial.print(gy);
    Serial.print(F(", "));
    Serial.print(gz);
    Serial.println();

    Serial.print(F("Magnetometer xyz:   "));
    Serial.print(mx);
    Serial.print(F(", "));
    Serial.print(my);
    Serial.print(F(", "));
    Serial.print(mz);
    Serial.println();

    Serial.print(F("Temperature:        "));
    Serial.print(temperature);
    Serial.println();

    Serial.print(F("Battery V, pct:     "));
    Serial.print(battery_voltage);
    Serial.print(F(", "));
    Serial.print(battery_percent);
    Serial.println();

    Serial.print(F("Color rgb sense:    "));
    Serial.print(sense_red);
    Serial.print(F(", "));
    Serial.print(sense_green);
    Serial.print(F(", "));
    Serial.print(sense_blue);
    Serial.print(F(", "));
    Serial.print(_brightness);
    Serial.println();

    Serial.print(F("Hue:                "));
    Serial.print(_hue);
    Serial.println();

    Serial.print(F("Color index:        "));
    Serial.print(_color);
    Serial.println();
}

void robot::whats_the_color(void)
{
    Serial.print(F("Hue:    "));
    Serial.print(_hue);
    Serial.println();
    Serial.print(F("Birghtness:    "));
    Serial.print(_brightness);
    Serial.println();
    Serial.print(F("Color Index:   "));
    Serial.print(_color);
    Serial.println();
}

void robot::move_test(void)
{
    correction(de_theta, _theta, drive_forward, drive_spin);
    
    Serial.print(F("Current Theta:  "));
    Serial.print(_theta);
    Serial.println();

    Serial.print(F("Desired Theta:  "));
    Serial.print(de_theta);
    Serial.println();

    Serial.print(F("Forward:        "));
    Serial.print(drive_forward);
    Serial.println();

    Serial.print(F("Spin:           "));
    Serial.print(drive_spin);
    Serial.println();
}

void robot::behaviorExample(void)
{
    /*inputColor(sense_red, sense_green, sense_blue, sense_clear);*/
    switch(action)
    {
        case ACTION_INIT:
            // No initialization actions, transition to action_forward
            action = ACTION_EXAMPLE_FORWARD;
            break;
        // ------------------------------------------------
        case ACTION_EXAMPLE_FORWARD:
            // Drive directly forward
            drive_forward = 1;
            drive_spin = .5;
            // Condition: if the level of red light falls below 200 (of 255), transition to spin action
            if(_color == 2 )
            {
                action = ACTION_EXAMPLE_SPIN;
            }
            break;
        // ------------------------------------------------
        case ACTION_EXAMPLE_SPIN:

            drive_forward = 0;
            drive_spin = 1;
            // Condition: if level of red light goes above threshold, go forward again
            if(_color == 6)
            {
                action = ACTION_EXAMPLE_FORWARD;
            }
            break;
        // ------------------------------------------------
        default:
            // nothing
            drive_forward = 0;
            drive_spin = 0;
            break;
    }
}

void robot::behavior_spatial_programming(void)
{
    previous_color = _color;
    c_c = millis();
	switch(action)
	{

        case ACTION_INIT:
            // No initialization actions
            action = GREEN_ACTION;
            break;
        // ------------------------------------------------
		case GREEN_ACTION:

            Serial.println();
	        Serial.print("Pass ==================================================== ");
            Serial.println();

            Serial.print(F("Time:            "));
            Serial.print(millis());
            Serial.println();

			move_x = -1;
			move_y = 0;


            Serial.println();
	        Serial.print("Pass ==================================================== ");
            Serial.println();

            de_theta = orientation(move_y, move_x);

            Serial.print(F("Desired Theta:   "));
            Serial.print(de_theta);
            Serial.println();

            correction(de_theta, _theta, drive_forward, drive_spin);

            if(color_count(_color, previous_color, c_c, h_ms) && _color == 6)
            {
            Serial.print("I see blue");
            Serial.println();

                action = BLUE_ACTION;
            }
			break;
        // ------------------------------------------------
		case BLUE_ACTION:

            Serial.println();
	        Serial.print("Pass ==================================================== ");
            Serial.println();

            Serial.print(F("Time:            "));
            Serial.print(millis());
            Serial.println();

			move_x = 0;
			move_y = 1;

            Serial.println();
	        Serial.print("Pass ==================================================== ");
            Serial.println();

            de_theta = orientation(move_y, move_x);

            Serial.print(F("Desired Theta:   "));
            Serial.print(de_theta);
            Serial.println();

            correction(de_theta, _theta, drive_forward, drive_spin);

            if(color_count(_color, previous_color, c_c, h_ms) && _color == 4)
            {
            Serial.print("I see green");
            Serial.println();
            
            
                action = GREEN_ACTION;
            }
			break;
        // ------------------------------------------------
		default:
        
            drive_forward = 1;
            drive_spin = 0;

            break;
	}
	
}

void robot::shadow_instructions(void)
{
    previous_color = _color;
    c_c = millis();

  	switch(action)
	{
        case ACTION_INIT:
            // No initialization actions
            action = WHITE_ACTION;
            break;
        // ------------------------------------------------
		case WHITE_ACTION:

            Serial.println();
            Serial.println("Lightness");
            Serial.println();
            Serial.println(_brightness);
            Serial.println();
            Serial.print(F("Color Index:   "));
            Serial.print(_color);
            Serial.println();

            drive_forward = 1;
            drive_spin = 0;

            if( color_count(_color, previous_color, c_c, h1_ms) && _color == 0)
            {
                action = BLACK_ACTION;
            }
			break;
        // ------------------------------------------------
		case BLACK_ACTION:

            Serial.println();
            Serial.print(F("Color Index:   "));
            Serial.print(_color);
            Serial.println();

            forward_spin(_color == 0, drive_forward, drive_spin);

            if(color_count(_color, previous_color, c_c, h_ms) && _color == 1)
            {      
                action = WHITE_ACTION;
            }
			break;
        // ------------------------------------------------
		default:

            drive_forward = 1;
            drive_spin = 0;

            break;
	}
}

void robot::f_p_gps(void)
{
    // IMPORTANT: This must be called repeatedly (every loop) while GPS behavior is active.

    switch(action)
    {
        case ACTION_GPS_INIT:
        {
            Serial.println(_color);
            // DO NOT START immediately. First, require stable white.
            gps_iter = 0;
            previous_color =_color;
            c_c = millis();

            Serial.println(F("GPS Sequence has Begun"));

            action = ACTION_GPS_WAIT_WHITE_START; // NEXT sub-state
            break;
        }
        // ------------------------------------------------
        case ACTION_GPS_WAIT_WHITE_START:
        {
            Serial.println(_color);
            // Wait until WHITE is stable, then initialize GPS run
            if (_color != COLOR_WHITE)
            {
                // keep tracking until it becomes white
                color_count(_color, previous_color, c_c, h2_ms);
                break;
            }

            if (color_count(_color, previous_color, c_c, h2_ms))
            {
                // clean tracker for next phase
                previous_color = _color;
                c_c = millis();

                Serial.println(F("GPS Sequence has Begun (WHITE confirmed)"));
                action = ACTION_GPS_WAIT_NONWHITE;
            }
            break;
        }                
        // ------------------------------------------------
        case ACTION_GPS_WAIT_NONWHITE:
        {
            Serial.println(_color);
            // Wait until we see a stable NON-WHITE color
            if (_color == COLOR_WHITE)
            {
                color_count(_color, previous_color,c_c,h2_ms);
                break;
            }

            // Non-white candidate: require stability
            if(color_count(_color,previous_color,c_c, h2_ms))
            {
                // Confirmed stable direction color
                gps_color = _color;             // stable color
                action = ACTION_GPS_REPORT; // advance

            }
            break;
        }
        case ACTION_GPS_REPORT:
        {
            // Update position based on stable direction color
            switch (gps_color)
            {
                case COLOR_RED:
                    gps_x = gps_x - step_size;  
                    gps_y = gps_y + step_size;  
                    break;
                case COLOR_BLUE:
                    gps_x = gps_x + step_size;  
                    gps_y = gps_y + step_size;  
                    break;
                case COLOR_GREEN:
                    gps_x = gps_x - step_size;  
                    gps_y = gps_y - step_size;  
                    break;
                case COLOR_MAGENTA:
                    gps_x = gps_x + step_size;  
                    gps_y = gps_y - step_size;  
                    break;
                default:
                    Serial.println(F("Invalid"));
                    break;
            }

            step_size = step_size / 2.0f;

            Serial.print(F("X: ")); Serial.print(gps_x);
            Serial.print(F(", Y: ")); Serial.println(gps_y);

            // Reset stability tracking for the "back to white" phase
            previous_color = _color;
            c_c    = millis();

            action = ACTION_GPS_WAIT_WHITE;
            break;
        }

        case ACTION_GPS_WAIT_WHITE:
        {
            // Wait until white is stable again before next iteration
            if (_color != COLOR_WHITE) 
            {
                color_count(_color, previous_color, c_c, h2_ms);
                break;
            }

            if (color_count(_color, previous_color, c_c, h2_ms))
            {
                // Completed one GPS step
                gps_iter++;

                if (gps_iter >= 5)
                {
                    action = ACTION_GPS_COMPLETE ;
                    Serial.println(F("I'm here on the whiteboard"));
                }
                else
                {
                    // Next step: wait for next stable non-white
                    action = ACTION_GPS_WAIT_NONWHITE;

                    // Reset stability tracker cleanly
                    previous_color = _color;
                    c_c    = millis();
                }
            }
            break;
        }
        case ACTION_GPS_COMPLETE:
        {
            // Do NOT halt the CPU.
            // Instead, signal your behavior/state machine that GPS is complete.
            // Example ideas (you’ll map to your system):
            // set_state(STATE_NEXT);
            // gps_started = false; // if you want to allow re-run
            break;
        }

        default:
        {
            action = ACTION_GPS_WAIT_NONWHITE;
            break;
        }
    }

}



///////////////////////////////////////////////////////////////
// Input methods ---------------------------------------
///////////////////////////////////////////////////////////////
void robot::inputIMU(float _ax, float _ay, float _az, float _gx, float _gy, float _gz)
{
    ax = _ax;
    ay = _ay;
    az = _az;
    // Todo: find unit vector

    gx = _gx;
    gy = _gy;
    gz = _gz;
    // Todo: make the calibration happen. probably not needed

    _theta = orientation(_ax, _ay);

    //Serial.print(F("Current Theta:   "));
    //Serial.print(_theta);
    //Serial.println();

    //movement(de_theta, _theta, drive_forward, drive_spin);
}

void robot::inputMag(float _mx, float _my, float _mz)
{
    mx = _mx;
    my = _my;
    mz = _mz;
    // Todo: Need to account for how these magnetometer readings actually point to north
}

void robot::inputTemperature(float _temperature)
{
    temperature = _temperature;
}

void robot::inputBattery(float _voltage, float _percent)
{
    battery_voltage = _voltage;
    battery_percent = _percent;
}

void robot::setInputTime(uint32_t _time)
{
    last_time = this_time;
    this_time = _time;
    dt = (this_time - last_time) / 1000;
}

void robot::inputColor(float _red, float _green, float _blue, float _clear)
{   
    /*sense_red   = _red;
    sense_green = _green;
    sense_blue  = _blue;
    sense_clear = _clear;*/

    // Here is where to put the color interpretation code
    _hue = calchue(_red, _green, _blue);
    _brightness = intensity(_red, _green, _blue);
    _color = sense_color(_hue, _brightness, THRESHOLD_WHITE, THRESHOLD_BLACK);
    
    // Serial.print(F("Hue:             "));
    // Serial.print(_hue);
    // Serial.println();
    
    // Serial.print(F("Brightness:      "));
    // Serial.print(_brightness);
    // Serial.println();

    // Serial.print(F("Color Index:     "));
    // Serial.print(_color);
    // Serial.println();
}

