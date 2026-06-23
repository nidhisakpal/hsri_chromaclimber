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

        case COLOR_SENSE_TEST:
            whats_the_color();
            break;

        case BEHAVIOR_SPATIAL_PROGRAM:
            behavior_spatial_programming();
            break;

        case BEHAVIOR_GUIDE_TO_GOAL:
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
void robot::behaviorReport(void)
{
    Serial.print(dt);           Serial.print(F(", "));
    Serial.print(input_ax);     Serial.print(F(", "));
    Serial.print(input_ay);     Serial.print(F(", "));
    Serial.print(input_az);     Serial.print(F(", "));

    Serial.print(input_gx);     Serial.print(F(", "));
    Serial.print(input_gy);     Serial.print(F(", "));
    Serial.print(input_gz);     Serial.print(F(", "));

    Serial.print(input_mx);     Serial.print(F(", "));
    Serial.print(input_my);     Serial.print(F(", "));
    Serial.print(input_mz);     Serial.print(F(", "));

    Serial.print(calc_theta);   Serial.print(F(", "));

    Serial.print(input_battery_voltage);    Serial.print(F(", "));
    Serial.print(calc_battery_percent);     Serial.print(F(", "));

    Serial.print(input_sense_red);   Serial.print(F(", "));
    Serial.print(input_sense_green); Serial.print(F(", "));
    Serial.print(input_sense_blue);  Serial.print(F(", "));
    Serial.print(input_sense_clear); Serial.print(F(", "));
    Serial.print(calc_hue);          Serial.print(F(", "));
    Serial.print(calc_brightness);   Serial.print(F(", "));
    Serial.print(calc_color);        Serial.print(F(", "));
}

void robot::whats_the_color(void)
{
    Serial.print(F("Hue:    "));
    Serial.print(calc_hue);
    Serial.println();
    Serial.print(F("Birghtness:    "));
    Serial.print(calc_brightness);
    Serial.println();
    Serial.print(F("Color Index:   "));
    Serial.print(calc_color);
    Serial.println();
}

void robot::behavior_spatial_programming(void)
{
    // Defaults
    play_mode = SILENT;
    move_mode = DRIVEMODE_VECTOR;

    // The robot will move in a direction based on the color it sees.
    switch(calc_color)
    {
        case BLUE:
            setMoveVectors(NORTH);
            break;
        case GREEN:
            setMoveVectors(EAST);
            break;
        case RED:
            setMoveVectors(WEST);
            break;
        case TEAL:
            setMoveVectors(SOUTH);
            break;
        case YELLOW:
            setMoveVectors(STOP_MOVING);
            play_mode = PLAY_TONE;
            speaker_frequency = 440;
            speaker_volume = 100;
            speaker_duration = 500;
            led_g = 1;
            break;
        case WHITE:
            // Reverse direction
            flag_0 = 0xFF;
            logic_timer_0 = this_time  + 1000;
            break;
        default:
            setMoveVectors(STOP_MOVING);
            break;
    }

    // If the reverse flag is set, check if it's time to reverse back
    if(flag_0)
    {
        if(this_time < logic_timer_0)
        {
            move_mode = DRIVEMODE_ARC;
            drive_forward = -1;
            drive_spin = 0;
        }
        else
        {
            // Clear the flag after reversing for 500ms
            flag_0 = 0;
        }
    }
}
    
void robot::shadow_instructions(void)
{
    // Different behaviors based on percieved color. 
    // Robots will move forward if they see white
    // Robots spin in place if they see black.
    // If the robots bump into another robot, they will stop and turn for a random duration.

    move_mode = DRIVEMODE_ARC;

  	switch(action)
	{
        case ACTION_GUIDE_INIT:
            // No initialization actions
            action = ACTION_GUIDE_MOVE;
            break;
        case ACTION_GUIDE_MOVE:
            if (input_bumper_left || input_bumper_right)
            {
                action = ACTION_GUIDE_HIT_OBJECT_REVERSE;
                logic_timer_0 = this_time + 1000;
                drive_forward = -1;
                drive_spin = 0;
            }
            else if (calc_color == COLOR_WHITE)
            {
                drive_forward = 1;
                drive_spin = 0;
            }
            else if (calc_color == COLOR_BLACK)
            {
                drive_forward = 0;
                drive_spin = 1;
            }
            else if(calc_color == COLOR_GREEN)
            {
                action = ACTION_GUIDE_AT_GOAL;
            }
            else
            {
                drive_forward = 0;
                drive_spin = 0;
            }
            break;
        case ACTION_GUIDE_HIT_OBJECT_REVERSE:
            drive_forward = -1;
            drive_spin = 0;
            if(this_time >= logic_timer_0)
            {
                action = ACTION_GUIDE_HIT_OBJECT_SPIN;
                logic_timer_0 = this_time + (rand() % 2000 + 1000); // Spin for random duration between 1-3 seconds
            }
            break;
        case ACTION_GUIDE_HIT_OBJECT_SPIN:
            drive_forward = 0;
            drive_spin = 1;
            if(this_time >= logic_timer_0)
            {
                action = ACTION_GUIDE_MOVE;
            }
            break;
        case ACTION_GUIDE_AT_GOAL:
            drive_forward = 0;
            drive_spin = 0;
            led_r = 1;
            led_g = 1;
            led_b = 1;
            speaker_frequency = 250;
            speaker_volume = 100;
            speaker_duration = 1000;
            if(calc_color == BLACK)
            {
                action = ACTION_GUIDE_MOVE; // If we see black again, start moving again
            }
            break;
        default:
            action = ACTION_GUIDE_MOVE;
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
            Serial.println(calc_color);
            // DO NOT START immediately. First, require stable white.
            gps_iter = 0;
            previous_color = calc_color;
            c_c = millis();

            Serial.println(F("GPS Sequence has Begun"));

            action = ACTION_GPS_WAIT_WHITE_START; // NEXT sub-state
            break;
        }
        // ------------------------------------------------
        case ACTION_GPS_WAIT_WHITE_START:
        {
            Serial.println(calc_color);
            // Wait until WHITE is stable, then initialize GPS run
            if (calc_color != COLOR_WHITE)
            {
                // keep tracking until it becomes white
                color_count(calc_color, previous_color, c_c, h2_ms);
                break;
            }

            if (color_count(calc_color, previous_color, c_c, h2_ms))
            {
                // clean tracker for next phase
                previous_color = calc_color;
                c_c = millis();

                Serial.println(F("GPS Sequence has Begun (WHITE confirmed)"));
                action = ACTION_GPS_WAIT_NONWHITE;
            }
            break;
        }                
        // ------------------------------------------------
        case ACTION_GPS_WAIT_NONWHITE:
        {
            Serial.println(calc_color);
            // Wait until we see a stable NON-WHITE color
            if (calc_color == COLOR_WHITE)
            {
                color_count(calc_color, previous_color,c_c,h2_ms);
                break;
            }

            // Non-white candidate: require stability
            if(color_count(calc_color,previous_color,c_c, h2_ms))
            {
                // Confirmed stable direction color
                gps_color = calc_color;             // stable color
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
            previous_color = calc_color;
            c_c    = millis();

            action = ACTION_GPS_WAIT_WHITE;
            break;
        }

        case ACTION_GPS_WAIT_WHITE:
        {
            // Wait until white is stable again before next iteration
            if (calc_color != COLOR_WHITE) 
            {
                color_count(calc_color, previous_color, c_c, h2_ms);
                break;
            }

            if (color_count(calc_color, previous_color, c_c, h2_ms))
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
                    previous_color = calc_color;
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
void robot::processSensorData(void)
{
    // Color
    calc_hue = calchue(input_sense_red, input_sense_green, input_sense_blue);
    calc_brightness = intensity(input_sense_red, input_sense_green, input_sense_blue);
    calc_color = sense_color(calc_hue, calc_brightness, THRESHOLD_WHITE, THRESHOLD_BLACK);
    
    // Orientation
    calc_theta = orientation(input_ax, input_ay);

    // Battery Percent
    calc_battery_percent = ((input_battery_voltage - BATTERY_VOLTAGE_MIN) / (BATTERY_VOLTAGE_MAX - BATTERY_VOLTAGE_MIN)) * 100.0f;

    // Motor speed
    encoder_speed_left = (encoder_count_left - previous_encoder_count_left) / dt;
    encoder_speed_right = (encoder_count_right - previous_encoder_count_right) / dt;
    previous_encoder_count_left = encoder_count_left;
    previous_encoder_count_right = encoder_count_right;
}

///////////////////////////////////////////////////////////////
// Output methods ---------------------------------------
///////////////////////////////////////////////////////////////
void robot::resetOutputsToNull(void)
{
    motorspeed_left = 0;
    motorspeed_right = 0;

    led_r = 0;
    led_g = 0;
    led_b = 0;
}

void robot::processOutputs(void)
{
    float max_speed = 0;
    switch(move_mode)
    {
        case DRIVEMODE_VECTOR:
            // Convert move_x and move_y into drive_forward and drive_spin
            de_theta = orientation(move_y, move_x);
            float error = de_theta - calc_theta;
            // Normalize error to range [-180, 180]
            if (error > 180) error -= 360;
            if (error < -180) error += 360;
            // Proportional control for spin based on error
            if(fabs(error) >= FACING_ONLY_TURN_DEG)
            {
                drive_spin = getFloatSign(error) * 1.0f; // Full spin in direction of error
                drive_forward = 0;
            }
            else if (fabs(error) >= FACING_DEADZONE_DEG)
            {
                drive_spin = (error / FACING_ONLY_TURN_DEG) * 0.5 + 0.5; // Scaled spin within deadzone
                drive_forward = 1-(fabs(error)/FACING_ONLY_TURN_DEG); // Full forward when within deadzone
            }
            else
            {
                drive_spin = 0;
                drive_forward = 1.0f; // Full forward when within deadzone
            }

            // Convert spin and forward into differential drive speeds
            max_speed = fabs(drive_forward) + fabs(drive_spin);
            if (max_speed > 1.0f) {
                drive_forward /= max_speed;
                drive_spin /= max_speed;
            }
            motorspeed_left = drive_forward + drive_spin;
            motorspeed_right = drive_forward - drive_spin;

            break;

        case DRIVEMODE_ARC:
            // Convert spin and forward into differential drive speeds
            max_speed = fabs(drive_forward) + fabs(drive_spin);
            if (max_speed > 1.0f) {
                drive_forward /= max_speed;
                drive_spin /= max_speed;
            }
            motorspeed_left = drive_forward + drive_spin;
            motorspeed_right = drive_forward - drive_spin;

            break;

        case DRIVEMODE_MANUAL:
            // Assume drive_forward and drive_spin are set directly by the logic function
            break;

        default:
            // Default to manual if invalid mode
            break;
    }

    // Safety: If bumping, don't go forward
    if (input_bumper_left || input_bumper_right)
    {
        drive_forward = 0;
        motorspeed_left = drive_spin;
        motorspeed_right = -drive_spin;
    }
}

void robot::setMoveVectors(uint8_t direction)
{
    switch(direction)
    {
        case NORTH:
            move_x = NORTH_X;
            move_y = NORTH_Y;
            break;
        case NORTHEAST:
            move_x = NORTHEAST_X;
            move_y = NORTHEAST_Y;
            break;
        case EAST:
            move_x = EAST_X;
            move_y = EAST_Y;
            break;
        case SOUTHEAST:
            move_x = SOUTHEAST_X;
            move_y = SOUTHEAST_Y;
            break;
        case SOUTH:
            move_x = SOUTH_X;
            move_y = SOUTH_Y;
            break;
        case SOUTHWEST:
            move_x = SOUTHWEST_X;
            move_y = SOUTHWEST_Y;
            break;
        case WEST:
            move_x = WEST_X;
            move_y = WEST_Y;
            break;
        case NORTHWEST:
            move_x = NORTHWEST_X;
            move_y = NORTHWEST_Y;
            break;
        case STOP_MOVING:
            move_x = 0;
            move_y = 0;
            break;
    }
}