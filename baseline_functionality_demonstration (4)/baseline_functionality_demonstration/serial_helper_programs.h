/*////////////////////////////////////////////////////
 Name:		serial_helper_programs
 Created:	5/17/2019
 Author:	Petras Swissler

 Purpose:	Programs that abstract serial communication
*/////////////////////////////////////////////////////
// Boilerplate ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#pragma once
// Include Simple Definitions ~~~~~~~~~~~~~~~~~~~~~~~~
// Include Necessary Libraries ~~~~~~~~~~~~~~~~~~~~~~~
#include <stdint.h>
#include <Arduino.h>
// None: All functions intended to stand alone
// Low-Level Functions ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void	clearSerialBuffer(void);
void	waitForSerialIn(void);
void	drawHoriz(uint8_t num, char sym);
// Main Functions ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
bool	checkYN(uint32_t timeout);
bool	getAffirmation(float, uint32_t timeout);
char	getSerialKeypress(void);
int		getSerialInt(uint32_t timeout, bool echo);
float	getSerialFloat_check(uint32_t timeout, bool echo);
float	getSerialFloat(uint32_t timeout, bool echo);
void	printCompileInfo(void);
char	getSerialKeypressEcho(void);
