/*/////////////////////////////////////////////////////////////////////
 Author:	Petras Swissler
 Purpose:	All setup functions go here
*//////////////////////////////////////////////////////////////////////
#include "setup.h"
///////////////////////////////////////////////////////////////////////
void boardSetup(Adafruit_NeoPixel*  rgb_led, Adafruit_MAX17048*  lipo_monitor, 
                Adafruit_MPU6050*   imu, Adafruit_TCS34725*  color_sensor, 
                QMC5883LCompass*    compass)
{
    // ESP32 is kinda odd in that secondary ports must be manually
    // assigned their pins with setPins()!
    //Wire.begin();
    //Wire.setClock(400000);
    Wire1.setPins(SDA, SCL);
   

    pinConfig();
    
    // ----------------------------------------------------------------------
    // Configure the various daughter boards
    lipo_monitor->begin();
    lipo_monitor->setAlertVoltages(3.0, 4.2);

    imu->begin();
    imu->setAccelerometerRange(MPU6050_RANGE_8_G);
    imu->setGyroRange(MPU6050_RANGE_500_DEG);
    imu->setFilterBandwidth(MPU6050_BAND_184_HZ);

    compass->init();
    //compass->setSmoothing(2,true);  

    // ----------------------------------------------------------------------
    rgb_led->begin();

    while(digitalRead(DI_USER_BUTTON))
    {
        if(((millis()/500)%5)>3)
        {
            rgb_led->setPixelColor(0, 100, 0, 0);
        }
        else
        {
            rgb_led->setPixelColor(0, 0, 0, 0);
        }
        rgb_led->show();
    }



   // Set light to green to show that system has successfully initialized
    serialSetup();
    rgb_led->setPixelColor(0, 0, 100, 0);
    rgb_led->show();
}

// Configure pins
void pinConfig(void)
{
    pinMode(DO_COLOR_SENSOR_LED, OUTPUT);
    //pinMode(DO_ONBOARD_LED         , OUTPUT); Redundant
    /*pinMode(DO_HB_AIN1             , OUTPUT);
    pinMode(DO_HB_AIN2             , OUTPUT);
    pinMode(DO_HB_BIN1             , OUTPUT);
    pinMode(DO_HB_BIN2             , OUTPUT);*/
    pinMode(DO_HB_SLP, OUTPUT);

    pinMode(DI_USER_BUTTON, INPUT_PULLUP);

    analogWriteResolution(DO_HB_AIN1, PWM_BIT_RESOLUTION);
    analogWriteResolution(DO_HB_AIN2, PWM_BIT_RESOLUTION);
    analogWriteResolution(DO_HB_BIN1, PWM_BIT_RESOLUTION);
    analogWriteResolution(DO_HB_BIN2, PWM_BIT_RESOLUTION);

    analogWriteFrequency(DO_HB_AIN1, PWM_FREQUENCY);
    analogWriteFrequency(DO_HB_AIN2, PWM_FREQUENCY);
    analogWriteFrequency(DO_HB_BIN1, PWM_FREQUENCY);
    analogWriteFrequency(DO_HB_BIN2, PWM_FREQUENCY);

    // Initialize PWM outputs to zero duty
    analogWrite(DO_HB_AIN1, 0);
    analogWrite(DO_HB_AIN2, 0);
    analogWrite(DO_HB_BIN1, 0);
    analogWrite(DO_HB_BIN2, 0);


    digitalWrite(DO_COLOR_SENSOR_LED    , LOW);
    digitalWrite(DO_HB_AIN1             , LOW);
    digitalWrite(DO_HB_AIN2             , LOW);
    digitalWrite(DO_HB_BIN1             , LOW);
    digitalWrite(DO_HB_BIN2             , LOW);
    digitalWrite(DO_HB_SLP              , LOW);
    digitalWrite(DO_ONBOARD_LED         , LOW);
}

// Serial communication
void serialSetup(void)
{
	Serial.begin(SERIAL_BAUD_RATE);
	Serial.println();
	Serial.println(	F(__FILE__));
	Serial.print(	F(__DATE__));	Serial.print(F("\t"));
	Serial.println(	F(__TIME__));	
}
