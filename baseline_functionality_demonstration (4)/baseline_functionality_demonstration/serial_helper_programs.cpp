/*////////////////////////////////////////////////////
 Name:		serial_helper_programs
 Created:	5/17/2019
 Author:	Petras Swissler

 Purpose:	Programs that abstract serial communication
*/////////////////////////////////////////////////////
#include "serial_helper_programs.h"
// Main Functions ////////////////////////////////////

/// <summary>
/// Simple function that waits until a keypress is recieved then returns the associated character
/// </summary>
/// <returns>the pressed key</returns>
char getSerialKeypress()
{
	char symbolIn = 0;

	do	{
		waitForSerialIn();
		symbolIn = Serial.read();
		clearSerialBuffer();
	} 	while ((symbolIn == '\r') || (symbolIn == '\n'));

	return symbolIn;
}

/// <summary>
/// Simple function that waits until a keypress is recieved then returns the associated character and also prints it out
/// </summary>
/// <param name=""></param>
/// <returns>The pressed key</returns>
char getSerialKeypressEcho(void)
{
	char sym = getSerialKeypress();
	Serial.println(sym);
	return sym;
}

/// <summary>
/// Waits for user to input an integer via serial. Returns the integer
/// </summary>
/// <param name="timeout_s">How long to wait before timing out</param>
/// <param name="echo">whether or not to echo the input integer</param>
/// <returns></returns>
int getSerialInt(uint32_t timeout_s = 100, bool echo = false)
{
	int intIn = 0;

	Serial.setTimeout(timeout_s * 1000);
	waitForSerialIn();
	intIn = Serial.parseInt();
	clearSerialBuffer();

	if (echo)
	{
		Serial.println(intIn);
	}

	return intIn;
}

/// <summary>
/// Waits for user to input a floating point number via serial. Returns the input number
/// </summary>
/// <param name="timeout_s">How long to wait before timing out</param>
/// <param name="echo">whether or not to echo the input number</param>
/// <returns></returns>
float getSerialFloat(uint32_t timeout_s = 100, bool echo = false)
{
	float floatIn = 0;

	Serial.setTimeout(timeout_s * 1000);
	waitForSerialIn();
	floatIn = Serial.parseFloat();
	clearSerialBuffer();
	if (echo)
	{
		Serial.println(floatIn);
	}
	return floatIn;
}

/// <summary>
/// Check that the user agrees with a given number. Usually used after a getSerialNumber() command
/// </summary>
/// <param name="floatIn">the number to check</param>
/// <param name="timeout_s">How long (s) to wait before timing out</param>
/// <returns>Whether the user agreed</returns>
bool getAffirmation(float floatIn, uint32_t timeout_s = 100)
{
	Serial.print(F("Got: ")); Serial.println(floatIn,6);
	Serial.println(F("? y/n"));
	return checkYN(timeout_s);
}


bool checkYN(uint32_t timeout_s)
{
	clearSerialBuffer();
	char check = getSerialKeypressEcho();
	if (check == 'y') { return true; }
	else { return false; }
}

float getSerialFloat_check(uint32_t timeout_s = 100, bool echo = false)
{
	float floatIn = 0;
	bool success = false;

	while (!success)
	{
		Serial.setTimeout(timeout_s * 1000);
		waitForSerialIn();
		floatIn = Serial.parseFloat();
		clearSerialBuffer();

		success = getAffirmation(floatIn, 100);
		if (!success)
		{
			Serial.println(F("Re-In:"));
		}
	}

	if (echo)
	{
		Serial.println(floatIn);
	}

	return floatIn;
}

void printCompileInfo(void)
{
	//Serial.println();
	//Serial.println(	F(__FILE__));
	//Serial.print(	F(__DATE__));	Serial.print(F("\t"));
	//Serial.println(	F(__TIME__));	
}

void drawHoriz(uint8_t num, char sym)
{
	for (uint8_t ii = 0; ii < num; ii++)
	{
		Serial.write(sym);
	}
	Serial.println();
}

// Lower-Level Functions /////////////////////////////
void clearSerialBuffer(void)
{
	while (Serial.available())
	{
		Serial.read();
	}
}


/// <summary>
/// Do nothing until a serial command is available
/// </summary>
/// <param name=""></param>
void waitForSerialIn(void)
{
	// Wait until have serial data available
	while (Serial.available() == 0)
	{
		// Do nothing
	}
}


