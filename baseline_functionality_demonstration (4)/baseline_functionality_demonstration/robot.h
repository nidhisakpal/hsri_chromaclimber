#pragma once
/*/////////////////////////////////////////////////////////////
// Author: Petras Swissler
// Description: Basically a container for robot variables and
                other logic.
*//////////////////////////////////////////////////////////////
// Include libraries -------------
#include "def_states_and_phases.h"
#include "serial_helper_programs.h"
#include "simple_helper_functions.h"
#include <Arduino.h>

// Define the class --------------
/* For the most part, we're just going to use the robot class as a container for variables and for logic functions. 
Everything like loading in measurements and handling outputs will be taken care of by other parts of the code */
class robot
{
    // Everything is public since in embedded systems like this we're more or less in full control. This also makes stuff like debugging a lot easier
public:
    // constructor / destructor
    robot();
    ~robot();
    // Self values ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	uint8_t selfID = 0;
	uint8_t role = 0;
	uint32_t program_tick = 0;

	// Logic ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	uint8_t behavior	= 0;
	uint8_t action		= 0;
	uint8_t phase		= 0;	

    // Sensed values ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	uint32_t this_time = 0;
	uint32_t last_time = 0;
	float dt = 1;

    // Measured accelerations / gyroscope / magnetometer
    float ax = 0, ay = 0, az = 0;
    float gx = 0, gy = 0, gz = 0;
    float mx = 0, my = 0, mz = 0;

    // Temperature sensors
    float temperature = 0;

    // Battery Values
    float battery_voltage = 0;
    float battery_percent = 0;

    float sense_red = 0, sense_blue = 0, sense_green = 0 , sense_clear = 0;

    // Stuff for Color Function
    float _hue;
    float _color;
    float _clear;
    float _brightness;
    
    // GPS variables
    float gps_x = 0.5f;
    float gps_y = 0.5f;
    float step_size = 0.25f;
    uint8_t gps_iter = 0;
    uint8_t gps_color = 0;



    // Stuff for Movement Function
    float _theta;
    float de_theta = 0;
    float move_x, move_y;
    // Other
    uint8_t user_button_pushed = false;

    // Output values ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // LEDs
    uint8_t rgb_r = 0, rgb_g = 0, rgb_b = 0;
    uint8_t color_sensor_white_led = 0;

    // Motor Control
    float drive_forward = 0;
    float drive_spin = 0;

    float safe_drive_forward = 0;
    float safe_drive_spin = 0;

    // Color Count
    uint8_t previous_color = 0;
    uint32_t c_c = 0;
    uint32_t h_ms = 1000;
    uint32_t h1_ms = 100;
    uint32_t h2_ms = 180;

    // Networking stuff ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    //uint8_t network_data_out[128] = 0;
   //uint8_t network_data_in[128] = 0;
    
    /////////////////////////////////////////////////////////////////////////
    // Methods
    void setInputTime(uint32_t time);
    void inputIMU(float ax, float ay, float az, float gx, float gy, float gz);
    void inputMag(float mx, float my, float mz);
    void inputTemperature(float temperature);
    void inputBattery(float voltage, float percent);
    
    void cali_thresh(float red, float green, float blue, float clear);
    void inputColor(float red, float green, float blue, float clear);
    void resetOutputsToNull(void);

    void behaviorReport(void);
    void behaviorExample(void);
    void whats_the_color(void);
    void runLogic(void);
    void move_test(void);
    void behavior_spatial_programming(void);
    void shadow_instructions(void);
    void f_p_gps(void);
};