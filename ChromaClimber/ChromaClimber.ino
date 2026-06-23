//////////////////////////////////////////////////////////////////////////////////
// Authors: Petras Swissler, Roberto Torres
// Description: This is the code for the ChromaClimber, a robot based on the 3pi+
// 32U4 OLED Robot.
// Robot control code adapted from the Pololu 3pi+ 32U4 OLED Robot example code
//////////////////////////////////////////////////////////////////////////////////
// -------------------------------------------------------------------------------
// Include the various libraries 
#include <Wire.h>

// Pololu 3pi+ 32U4 OLED Robot
#include <Pololu3piPlus32U4.h>
#include <Pololu3piPlus32U4IMU.h>
#include <PololuMenu.h>

// RGB Color Sensor
#include <Adafruit_TCS34725.h>

// Other helper functions and definitions
#include "menu_stuff.h"
#include "splash.h"
#include "def_states_and_phases.h"
#include "def_robot_properties.h"
#include "def_friendlynames.h"
#include "def_playtones.h"
#include "robot.h"

// -------------------------------------------------------------------------------
// Create global objects and variables
using namespace Pololu3piPlus32U4;

OLED display;
Buzzer buzzer;
ButtonA buttonA;
ButtonB buttonB;
ButtonC buttonC;
LineSensors lineSensors;
BumpSensors bumpSensors;
IMU imu;
Motors motors;
Encoders encoders;

PololuMenu<typeof(display)> mainMenu;
Adafruit_TCS34725  color_sensor(TCS34725_INTEGRATIONTIME_60MS, TCS34725_GAIN_1X);

robot controller;

//////////////////////////////////////////////////////////////////////////////////
// Function Prototypes
void config_behavior_spatial_programming(void);
void config_behavior_heartbeat(void);
void config_behavior_shadow_instructions(void);
void config_behavior_virtual_joysticks(void);
void config_behavior_fractal_gps(void);
void config_behavior_color_reporting(void);
void inputs(robot* controller);
void outputs(robot* controller);
void loadCustomCharacters(void);
void loadCustomCharactersBarGraph(void);
void loadCustomCharactersMotorDirs(void);
void displayBackArrow(void);
void displaySplash(uint8_t *graphics, uint8_t offset);
void showSplash(void);
void mainMenuWelcome(void);

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

void setup() {
  Serial.begin(115200);
  // Menu setup
  static const PololuMenuItem mainMenuItems[] = {
    { F("Spatial"),     config_behavior_spatial_programming },
    { F("Heartbeat"),   config_behavior_heartbeat },
    { F("Shadow"),      config_behavior_shadow_instructions },
    { F("Joysticks"),   config_behavior_virtual_joysticks },
    { F("Fractal GPS"), config_behavior_fractal_gps },
    { F("Color Rep"),   config_behavior_color_reporting },
  };
  mainMenu.setItems(mainMenuItems, sizeof(mainMenuItems)/sizeof(mainMenuItems[0]));
  mainMenu.setDisplay(display);
  mainMenu.setBuzzer(buzzer);
  mainMenu.setButtons(buttonA, buttonB, buttonC);
  mainMenu.setSecondLine(F("\x7f" "A \xa5" "B C\x7e"));

  // Other Initializations
  Wire.begin();
  imu.init();
  imu.enableDefault();
  loadCustomCharacters();
  controller.display = &display;
}

void loop() {
  // put your main code here, to run repeatedly:
  if(mainMenu.buttonMonitor() != 'B')
  {
    // Main loop
    inputs(&controller);
    controller.processSensorData();
    controller.runLogic();
    controller.processOutputs();
    outputs(&controller);
  }
  else
  {
    // Show the splash screen and wait for the user to press button B
    mainMenuWelcome();
    while(!mainMenu.select());
    displayBackArrow();
  }

}

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
// Definitions for each of the behaviors in the menu. These functions essentially just configure the robot logic
void config_behavior_spatial_programming(void)
{
  // Set up the robot to use the spatial programming behavior
  controller.behavior = BEHAVIOR_SPATIAL_PROGRAM;
  controller.action = ACTION_SPATIAL_INIT;
}

void config_behavior_heartbeat(void)
{
  // Set up the robot to use the heartbeat behavior
  controller.behavior = BEHAVIOR_HEARTBEAT;
  controller.action = ACTION_INIT;
}

void config_behavior_shadow_instructions(void)
{
  // Set up the robot to use the shadow instructions behavior
  controller.behavior = BEHAVIOR_GUIDE_TO_GOAL;
  controller.action = ACTION_GUIDE_INIT;
}

void config_behavior_virtual_joysticks(void)
{
  // Set up the robot to use the virtual joysticks behavior
  controller.behavior = BEHAVIOR_GESTURE_CONTROL;
  controller.action = ACTION_GESTURE_INIT;
}

void config_behavior_fractal_gps(void)
{
  // Set up the robot to use the fractal GPS behavior
  controller.behavior = BEHAVIOR_FRACTIAL_PLAID_GPS;
  controller.action = ACTION_GPS_INIT;
}

void config_behavior_color_reporting(void)
{
  // Set up the robot to use the color reporting behavior
  controller.behavior = COLOR_SENSE_TEST;
  controller.action = ACTION_INIT;
}

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

void inputs(robot* controller)
{
  // Sensed values ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	controller->last_time = controller->this_time;
  controller->this_time = millis();
  controller->dt = (controller->this_time - controller->last_time)/1000.0;

  // Measured accelerations / gyroscope / magnetometer
  imu.read();
  controller->input_ax = imu.a.x;
  controller->input_ay = imu.a.y;
  controller->input_az = imu.a.z;
  controller->input_gx = imu.g.x;
  controller->input_gy = imu.g.y;
  controller->input_gz = imu.g.z;
  controller->input_mx = imu.m.x;
  controller->input_my = imu.m.y;
  controller->input_mz = imu.m.z;
  
  // Battery Values
  float input_battery_voltage = readBatteryMillivolts();

  // Light sensor
  uint16_t red, green, blue, clear;
  color_sensor.getRawData(&red, &green, &blue, &clear);
  controller->input_sense_red   = red;
  controller->input_sense_blue  = blue;
  controller->input_sense_green = green;
  controller->input_sense_clear = clear;

  // Encoders
  controller->encoder_count_left = encoders.getCountsLeft();
  controller->encoder_count_right = encoders.getCountsRight();

  // Buttons
  controller->input_button_a = buttonA.isPressed();
  controller->input_button_b = buttonB.isPressed();
  controller->input_button_c = buttonC.isPressed();

  // Line sensors
  uint16_t lineSensorValues[5];
  lineSensors.read(lineSensorValues, LineSensorsReadMode::On);
  for (int i = 0; i < 5; i++)
  {
    controller->input_line_sensor_values[i] = lineSensorValues[i];
  }

  // Bumpers
  controller->input_bumper_left   = bumpSensors.leftIsPressed();
  controller->input_bumper_right  = bumpSensors.rightIsPressed();  
}

void outputs(robot* controller)
{
  // Output values ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // LEDs
  ledYellow(controller->led_b); // No blue LED on the robot, so we'll use the yellow LED to represent blue
  ledGreen(controller->led_g);
  ledRed(controller->led_r);

  // Speaker
  switch(controller->play_mode)
  {
    case SILENT:
      buzzer.stopPlaying();
      break;
    case CONTINUE_PLAYING:
      // Do nothing, just keep playing whatever tone or song is currently playing
      break;
    case PLAY_TONE:
      // Play the tone at the volume specified for the duration specified
      buzzer.playFrequency(controller->speaker_frequency, controller->speaker_duration, controller->speaker_volume);
      break;
    case SONG_FUGUE:
      buzzer.play(NOTESEQUENCE_FUGUE);
      break;
    default:
      // Nothing
      break; 
  }

  // Motors
  motors.setSpeeds(400*controller->motorspeed_left, 400*controller->motorspeed_right);
}

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

void loadCustomCharacters()
{
  // The LCD supports up to 8 custom characters.  Each character
  // has a number between 0 and 7.  We assign #7 to be the back
  // arrow; other characters are loaded by individual demos as
  // needed.
  display.loadCustomCharacter(backArrow, 7);
}

// Assigns #0-6 to be bar graph characters.
void loadCustomCharactersBarGraph()
{
  static const char levels[] PROGMEM = {
    0, 0, 0, 0, 0, 0, 0, 63, 63, 63, 63, 63, 63, 63
  };
  display.loadCustomCharacter(levels + 0, 0);  // 1 bar
  display.loadCustomCharacter(levels + 1, 1);  // 2 bars
  display.loadCustomCharacter(levels + 2, 2);  // 3 bars
  display.loadCustomCharacter(levels + 3, 3);  // 4 bars
  display.loadCustomCharacter(levels + 4, 4);  // 5 bars
  display.loadCustomCharacter(levels + 5, 5);  // 6 bars
  display.loadCustomCharacter(levels + 6, 6);  // 7 bars
}

// Assigns #0-4 to be arrow symbols.
void loadCustomCharactersMotorDirs()
{
  display.loadCustomCharacter(forwardArrows, 0);
  display.loadCustomCharacter(reverseArrows, 1);
  display.loadCustomCharacter(forwardArrowsSolid, 2);
  display.loadCustomCharacter(reverseArrowsSolid, 3);
}

// Clears the LCD and puts [back_arrow]B on the second line
// to indicate to the user that the B button goes back.
void displayBackArrow()
{
  display.clear();
  display.gotoXY(0,1);
  display.print(F("\7B"));
  display.gotoXY(0,0);
}

void showSplash()
{
  // We only need the graphics array within showSplash(); it's not
  // used elsewhere in the demo program, so we can make it a local
  // variable.
  uint8_t graphics[1024];

  display.setLayout21x8WithGraphics(graphics);

  uint16_t blinkStart = millis();
  while((uint16_t)(millis() - blinkStart) < 900)
  {
    // keep setting the LEDs on for 1s
    // the Green/Red LEDs might turn off during USB communication
    ledYellow(1);
    ledGreen(1);
    ledRed(1);
  }

  // scroll quickly up
  for(uint8_t offset = 1; offset < 5; offset ++)
  {
    delay(100);
    displaySplash(graphics, offset);
  }

  display.clear();
  display.gotoXY(0, 5);
  display.print(F("ChromaClimber"));
  display.gotoXY(0, 6);
  display.print(F("Push B to start a behavior"));

  while((uint16_t)(millis() - blinkStart) < 2000)
  {
    // keep the LEDs off for 1s
    ledYellow(0);
    ledGreen(0);
    ledRed(0);
  }

  // Keep blinking the green LED while waiting for the
  // user to press button B.
  blinkStart = millis();
  while (mainMenu.buttonMonitor() != 'B')
  {
    uint16_t blinkPhase = millis() - blinkStart;
    ledGreen(blinkPhase < 1000);
    if (blinkPhase >= 2000) { blinkStart += 2000; }
  }
  ledGreen(0);

  display.setLayout11x4WithGraphics(graphics);
  display.clear();
  display.gotoXY(0,3);
  display.noAutoDisplay();
  display.print(F("Thank you!!"));
  display.display();

  buzzer.playFromProgramSpace(beepThankYou);
  delay(1000);
  display.clear();
  display.setLayout8x2();
}

void mainMenuWelcome()
{
  ledYellow(false);
  ledGreen(false);
  ledRed(false);
  display.clear();
  display.print(F("  Main"));
  display.gotoXY(0, 1);
  display.print(F("  Menu"));
  delay(1000);
}