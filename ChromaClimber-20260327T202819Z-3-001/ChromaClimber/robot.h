#pragma once
/*/////////////////////////////////////////////////////////////
// Author: Petras Swissler
// Description: Basically a container for robot variables and
                other logic.
*//////////////////////////////////////////////////////////////
// Include libraries -------------
#include "def_states_and_phases.h"
#include "serial_helper_programs.h"
#include <Arduino.h>
#include "def_robot_properties.h"
#include "def_friendlynames.h"
#include "def_playtones.h"
#include <Pololu3piPlus32U4.h>
#include "simple_helper_functions.h"

using namespace Pololu3piPlus32U4;

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
    
    uint8_t flag_0 = 0;
    uint8_t flag_1 = 0; 

    // Timing variables
	uint32_t this_time = 0;
	uint32_t last_time = 0;
	float dt = 1;

    uint32_t logic_timer_0 = 0;

    // Global variables ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    uint8_t move_mode = DRIVEMODE_VECTOR;
    float drive_forward = 0; //  0...1
    float drive_spin = 0;    //  0...1
    float move_vector_x = 0; // -1...1
    float move_vector_y = 0; // -1...1

    
    OLED* display;
    
    // Sensed values ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // Measured accelerations / gyroscope / magnetometer
    float input_ax = 0, input_ay = 0, input_az = 0;
    float input_gx = 0, input_gy = 0, input_gz = 0;
    float input_mx = 0, input_my = 0, input_mz = 0;

    // Battery Value
    float input_battery_voltage = 0;

    // Color sensor values
    float input_sense_red = 0, input_sense_blue = 0, input_sense_green = 0 , input_sense_clear = 0;

    // Buttons and bumpers
    uint8_t input_button_a = 0;
    uint8_t input_button_b = 0;
    uint8_t input_button_c = 0;

    uint8_t input_bumper_left = 0;
    uint8_t input_bumper_right = 0;

    int16_t encoder_count_left = 0;
    int16_t encoder_count_right = 0;
    int16_t previous_encoder_count_left = 0;
    int16_t previous_encoder_count_right = 0;

    // Line Sensors
    uint16_t input_line_sensor_values[5] = {0};

    // Calculated Values ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    float calc_battery_percent = 0;

    float calc_hue;
    float calc_clear;
    float calc_brightness;
    uint8_t calc_color;

    float encoder_speed_left = 0;
    float encoder_speed_right = 0;
    
    // GPS variables
    float gps_x = 0.5f;
    float gps_y = 0.5f;
    float step_size = 0.25f;
    uint8_t gps_iter = 0;
    uint8_t gps_color = 0;

    // Stuff for Movement Function
    float calc_theta;
    float de_theta = 0;
    float move_x;
    float move_y;

    // Output values ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // LEDs and speaker
    uint8_t led_r = 0, led_g = 0, led_b = 0;
    //uint8_t color_sensor_white_led = 0;
    unsigned int  speaker_frequency = 0;
    unsigned char speaker_volume = 0;
    unsigned int speaker_duration = 0;
    uint8_t play_mode = SILENT;

    // Motor Control
    float motorspeed_left = 0;
    float motorspeed_right = 0;

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
    /*void setInputTime(uint32_t time);
    void inputIMU(float ax, float ay, float az, float gx, float gy, float gz);
    void inputMag(float mx, float my, float mz);
    void inputTemperature(float temperature);
    void inputBattery(float voltage, float percent);
    
    void cali_thresh(float red, float green, float blue, float clear);
    void inputColor(float red, float green, float blue, float clear);*/
    void resetOutputsToNull(void);
    void processSensorData(void);
    void processOutputs(void);

    void setMoveVectors(uint8_t);
    void behaviorReport(void);
    void whats_the_color(void);
    void runLogic(void);
    void behavior_spatial_programming(void);
    void shadow_instructions(void);
    void f_p_gps(void);
};