/*////////////////////////////////////////////////////
 Name:		def_pins.h
 Created:	3/25/2019
 Author:	Petras Swissler

 Purpose:	Define simple helper functions
*/////////////////////////////////////////////////////

#include "simple_helper_functions.h"
#include <math.h>
//////////////////////////////////////////////////////
// Bound an input between two limits
float bound(float input, float minimum, float maximum)
{
	return max(min(input, maximum), minimum);
}

//////////////////////////////////////////////////////
// Adjust max and min values based on observed measurement
void	adjustMaxMin(float* measurement, float* max_meas, float* min_meas)
{
	*max_meas = max(*measurement, *max_meas);
	*min_meas = min(*measurement, *min_meas);
	return;
}

//////////////////////////////////////////////////////
// Convert number to normalized range between max and min. Adjust max and min if needed
void	normalizedNumber0to1(float* x, float* x_max, float* x_min)
{
	*x_max = max(*x, *x_max);
	*x_min = min(*x, *x_min);
	*x = (*x - *x_min) / (*x_max - *x_min);
}

void normalizedNumberNeg1to1(float* x, float* x_max, float* x_min)
{
	normalizedNumber0to1(x, x_max, x_min);
	*x = 2 * (*x) - 1;
}


//////////////////////////////////////////////////////
// Geth the nth prime
int getNthPrime(int nth)
{
	int count = 0;
	int check = 1;
	while (count < nth)
	{
		check += 1;
		char found_factor = 0;
		for (int ii = 2; ii < ((check + 2) / 2); ii++)
		{
			if (!(check % ii))
			{
				found_factor = 1;
				break;
			}
		}
		if (!found_factor)
		{
			count += 1;
		}
	}
	return check;
}

//////////////////////////////////////////////////////
// Get the sign of a number
float getFloatSign(float x)
{
	return (x > 0) - (x < 0);
}

int getIntSign(int x)
{
	return (x > 0) - (x < 0);
}

//////////////////////////////////////////////////////
// Vector and Matrix Math Functions
void	vectorAdd_f(float* target_vec3, float vx, float vy, float vz, float scale)
{
	target_vec3[0] = target_vec3[0] + vx * scale;
	target_vec3[1] = target_vec3[1] + vy * scale;
	target_vec3[2] = target_vec3[2] + vz * scale;
}

//////////////////////////////////////////////////////
// Get the average reading of an analog pin over a 
// certain number of measurements
float getAvgReading(int pin, uint8_t numMeas) {
	float avg = 0;
	for (uint8_t ii = 0; ii < numMeas; ii++) 
	{
		avg = avg + analogRead(pin);
	}
	return (avg / (float)numMeas);
}

int getAvgReading_int(int pin, uint8_t numMeas) {
	int avg = 0;
	for (uint8_t ii = 0; ii < numMeas; ii++)
	{
		avg = avg + analogRead(pin);
	}
	return (avg / numMeas);
}

int		getAvgReading_int_notHiOrLo(int pin, uint8_t numMeas) {
	int avg = 0;
	int hi = 0;
	int lo = 1023;
	int reading = 0;
	for (uint8_t ii = 0; ii < numMeas; ii++)
	{
		reading = analogRead(pin);
		avg = avg + reading;
		hi = max(hi, reading);
		lo = min(lo, reading);
	}
	avg = avg - hi - lo;
	return (avg / (numMeas-2));
}

//////////////////////////////////////////////////////
// Convert a float into an array of chars
void floatToFourChars(float inFloat, uint8_t* outChar)
{
	for (uint8_t ii = 0; ii < BYTES_PER_FLOAT; ii++)
	{	
		outChar[ii] = (uint8_t)((*(uint32_t*) &inFloat) >> (BITS_PER_BYTE * ii)) & 0xFF;	
	}
}

float fourCharsToFloat(uint8_t* inChar)
{
	return  *(float*)&inChar[0];
}

void int16ToTwoChars(uint16_t inInt, uint8_t* outChar)
{
	for (uint8_t ii = 0; ii < 2; ii++)
	{
		outChar[ii] = (uint8_t)((*(uint16_t*)&inInt) >> (BITS_PER_BYTE * ii)) & 0xFF;
	}
}

uint16_t twoCharsToInt16(uint8_t* inChar)
{
	uint16_t return_num= 0x0000;
	return_num = return_num | inChar[0];
	return_num = return_num << 8;
	return_num = return_num | inChar[1];
	return return_num;
}



//////////////////////////////////////////////////////
// Get the middle value of three floats
float getMidof3(float a, float b, float c)
{
	if (a > b)
	{
		if (b > c)
		{
			return b;
		}
		else
		{
			if (c > a)
			{
				return a;
			}
			else {
				return c;
			}
		}
	}
	else
	{
		if (c > b)
		{
			return b;
		}
		else
		{
			if (c > a)
			{
				return c;
			}
			else {
				return a;
			}
		}
	}

	// Should never get here
	return b;
}

//////////////////////////////////////////////////////
// Check whether a certain duration has elapsed
int getMaxofN(int pin, uint8_t numMeas)
{
	int maxMeas = 0;
	for (uint8_t ii = 0; ii < numMeas; ii++)
	{
		int newMeas = analogRead(pin);
		if (newMeas > maxMeas)
		{
			maxMeas = newMeas;
		}
	}
	return maxMeas;
}

//////////////////////////////////////////////////////
/// <summary>
/// Check whether a certain duration has elapsed
/// </summary>
/// <param name="startTime"> time (ms) that the timer started</param>
/// <param name="nowTime"> time (ms) that it currently is</param>
/// <param name="targetDuration_seconds"> time that we wish to pass before returning true</param>
/// <returns> True if target duration has elapsed, false otherwise </returns>
bool checkTime(unsigned long startTime, unsigned long nowTime, float targetDuration_seconds)
{
	if ((nowTime - startTime) >= (targetDuration_seconds*MS_PER_S)) 
	{	return true;	}
	else 
	{	return false;	}
}

//////////////////////////////////////////////////////
// integer square root
int isqrt(int N)
{
	int a = 1;
	int b = N;

	while (abs(a - b) > 1)
	{
		b = N / a;
		a = (a + b) / 2;
	}

	return a;
}

//////////////////////////////////////////////////////
// Wrap cir
float wrapToPi(float val)
{
	// This should really be a while() but I don't want to
	// risk putting a while loop in the code. 
	// This implementation should be ok for this purpose
	if (fabs(val) > PI)
	{
		val = val - (2 * PI) * getFloatSign(val);
	}
	return val;
}

//////////////////////////////////////////////////////
// Get whatever byte is missing from a list of three options
uint8_t	getAbsentByte(uint8_t A, uint8_t B, uint8_t option1, uint8_t option2, uint8_t option3)
{
	// Take in two unique values and three options. Return the unchosen option.
	// If both unique values noth within the options, then return 0
	if (!((A == option1 || A == option2 || A == option3) && (B == option1 || B == option2 || B == option3)))
	{
		return 0;
	}
	else
	{
		return ((uint8_t)(!(A == option1 || B == option1)) * option1 + (uint8_t)(!(A == option2 || B == option2)) * option2 + (uint8_t)(!(A == option3 || B == option3)) * option3);
	}
}

//////////////////////////////////////////////////////
// Encode / Decode floats using an identical conversion factor
uint8_t floatToEncodedByte(float inValue, float conversionFactor)
{
	// Special behavior to prevent overflow
	return ((uint8_t)(bound(inValue * conversionFactor,0,255)));
}

float encodedByteToFloat(uint8_t inValue, float conversionFactor)
{
	return ((float)inValue / conversionFactor);
}

uint8_t s2u_int8(int8_t inbyte)
{
	return (*(uint8_t*)&inbyte);
}

int8_t u2s_int8(uint8_t inbyte)
{
	return (*(int8_t*)&inbyte);
}

//////////////////////////////////////////////////////
// Reporting / Printing Functions
void printArray(uint8_t* msg_array, uint8_t numBytes, bool use_print)
{
	Serial.print(F("["));
	for (uint8_t ii = 0; ii < numBytes; ii++)
	{
		if (use_print)
		{
			Serial.print(msg_array[ii]);
		}
		else
		{
			Serial.write(msg_array[ii]);
		}
		Serial.print(F(", "));
	}
	Serial.println(F("]"));
}

// Get free memory
/*#ifdef __arm__
// should use uinstd.h to define sbrk but Due causes a conflict
extern "C" char* sbrk(int incr);
#else  // __ARM__
extern char* __brkval;
#endif  // __arm__*/

int getFreeMemory() {
	char top;
	extern char* __brkval;
	return &top - __brkval;
/*#ifdef __arm__
	return &top - reinterpret_cast<char*>(sbrk(0));
#elif defined(CORE_TEENSY) || (ARDUINO > 103 && ARDUINO != 151)
	return &top - __brkval;
#else  // __arm__
	return __brkval ? &top - __brkval : &top - __malloc_heap_start;
#endif  // __arm__*/
}

char waitForKeypress(void) {
	char symbolIn = 0;

	// Wait until have serial data available
	while (Serial.available() == 0) {
		// Do nothing
	}

	symbolIn = Serial.read();

	// Clear the serial buffer
	delay(5);
	while (Serial.available()) {
		Serial.read();
	}

	return symbolIn;
}

// Linear Regression Functions
float getSlope(float x1, float y1, float x2, float y2)
{
	return ((y2 - y1) / (x2 - x1));
}

float getYIntercept(float x1, float y1, float slope)
{
	return (y1 - slope * x1);
}

// Clear Array
void clearArray(uint8_t* msgArray, uint8_t len)
{
	for (uint8_t ii = 0; ii < len; ii++)
	{		
		msgArray[ii] = 0;	
	}
	return;
}

// Normalize vector
void normalizeVector(float* x, float* y, float* z)
{
	float vec_mag = getVectorMagnitude(*x, *y, *z);
	*x = *x / vec_mag;
	*y = *y / vec_mag;
	*z = *z / vec_mag;
}

// Get vector magnitude
float getVectorMagnitude(float x, float y, float z)
{
	return sqrt(x * x + y * y + z * z);
}

// Get dot product
float vectorDotProduct(float* x0, float* y0, float* z0, float* x1, float* y1, float* z1)
{
	return (*x0 * *x1 + *y0 * *y1 + *z0 * *z1);
}

// Get angle between vectors
float getAngleBetweenVectors(float* x0, float* y0, float* z0, float* x1, float* y1, float* z1)
{
	// Just do the calculation all at once
	return acosf(vectorDotProduct(x0, y0, z0, x1, y1, z1) / (getVectorMagnitude(*x0, *y0, *z0) * getVectorMagnitude(*x1, *y1, *z1)));
}


////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
// Filtering functions	--------------------------------------------
////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////

float	susdFilter(float meas, float last, float stepsize)
{
	return min(max(meas, last - stepsize), last + stepsize);
}


//////////////////////////////////////////////////////
// Calculate the output value of an exponential filter
float expFilter(float newVal, float oldVal, float alpha)
{
	// DANGER: 
	// Here we assume that the user is smart enough to keep alpha between 0 and 1
	// Obviously, user error could make things blow up but not performing this step will make things faster
	//alpha = bound(alpha, 0.0, 1.0);
	return (newVal*alpha + (1-alpha)*oldVal);
}

////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
// Convert RGB values to color indexes  ----------------------------
////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////

float calchue(float R, float G, float B)
{
  float hue;

  // Intemediate Values
  float c_max = max(max(R, G), B);
  float c_min = min(min(R, G), B);
  float delta = c_max - c_min;

  // Convert RGB values to hue values
  if(delta != 0)
  {
    if(R == c_max)
    {
      hue = (G - B) / delta;
    }
    if(G == c_max)
    {
      hue = 2.0 + (B - R) / delta;
    }
    if(B == c_max)
    {
      hue = 4.0 + (R - G) / delta;
    }

    hue = hue / 6.0;
    hue = fmodf(hue, 1.0);
  }
  else
  {
    hue = 0.0;
  }

  return hue;
}

float intensity(float R, float G, float B)
{
	float brightness = (max(R, max(G, B)) +  min(R, min(G,B)))/2;
	return brightness;
}

// Determine color index
uint8_t sense_color(float hue, float brightness, uint16_t thresh_white, uint16_t thresh_black) 
{
  if (brightness > thresh_white) return COLOR_WHITE;
  if (brightness < thresh_black) return COLOR_BLACK;

  if (hue > RED_MIN_1) return COLOR_RED;
  if (hue > MAGENTA_MIN) return COLOR_MAGENTA;
  if (hue > BLUE_MIN) return COLOR_BLUE;
  if (hue > CYAN_MIN) return COLOR_CYAN;
  if (hue > GREEN_MIN) return COLOR_GREEN;
  if (hue > YELLOW_MIN) return COLOR_YELLOW;
  return COLOR_RED;  // fallback
}

////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
// Determine orientation  ------------------------------------------
////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////

float orientation(float x, float y)
{
	// converting ax and ay to theta
	 float rad = atan2(x, -y);
	 float theta = rad * (180.0/M_PI);

	 if (theta < 0.0)
	{
		theta += 360.0;
	}
	 return theta;
}


void correction(float d_theta, float c_theta, float &drive_forward, float &drive_spin)
{

	float error = d_theta - c_theta;

  if (error < 0)
	{
		error += 360.0;
	}
	else
	{
		error;
	}

	float dead_zone = 30.0;
	float max_angle = 180.0;

	if (error > dead_zone && error < 360 - dead_zone) 
	{
    drive_forward = 0.0;

    if (error <= max_angle) 

		{
      float ratio = (error / max_angle);
			if (ratio < 0.35)
			{
				ratio = 0.35;
			}
      drive_spin = - ratio;
    } 
		else
		{
      float back_error = 360 - error;        
      float ratio = (back_error / max_angle);
			if (ratio < 0.35)
			{
				ratio = 0.35;
			}
      drive_spin = ratio;
    }
	} 
	else 
	{    
    drive_forward = 1.0;
    drive_spin    = 0.0;
	}

  /*if (error > 5.0 && error < 355.0) 
	{
		drive_forward = 0;

		if (error <= 180)
		{
			drive_forward = 0;
			drive_spin = -1;
		}
		else
		{
			drive_forward = 0;
			drive_spin = 1;
		}
	}
	else
	{
		drive_forward = 1;
		drive_spin = 0;
	}*/

	Serial.print(F("Error:           "));
	Serial.print(error);
	Serial.println();

	Serial.print(F("Drive Forward:   "));
	Serial.print(drive_forward);
	Serial.println();

	Serial.print(F("Drive Spin:      "));
	Serial.print(drive_spin);
	Serial.println();

}
////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
// Delay color recognition  ----------------------------------------
////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////

bool color_count(uint8_t measured_color, uint8_t &color_for_logic, uint32_t &count_of_color, uint32_t hold_ms)

{
	uint32_t now = millis();

	if(measured_color == color_for_logic)
	{
		if (now - count_of_color >= hold_ms)
		{
			return true;
		}
		return false;
	}
	else
	{
		color_for_logic = measured_color; // start trackking new color
		count_of_color = now; 						// store the start time
		return false;
	}
}


void forward_spin(bool what_color, float& forward, float& spin)
{
	static bool active = false;
	static uint32_t t0 = 0;
	const float RAMP_MS = 6000;
	const float F0 = 0.20;
	const float S0 = 0.35;

	if (what_color)
	{
		if(!active)
		{
			active = true;
			t0 = millis();
		}

		float t = (millis()- t0) / RAMP_MS;
		if (t > 1.0)
		{
			t = 1.0;
		}
    forward = F0 + (1.0f - F0) * t;
    spin    = S0 + (1.0f - S0) * t;

		forward = bound(forward, 0, 1.0);
		spin = bound(spin, 0, 1.0);
	}

	else 
	{
		active = false;
	}

}