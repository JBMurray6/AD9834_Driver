/*
AD9834.cpp - AD9834 Direct Digital Synthesis IC Library

Copyright (C) 2012 Mark Jessop <mark.jessop@adelaide.edu.au>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

For a full copy of the GNU General Public License,
see <http://www.gnu.org/licenses/>.
*/

#if ARDUINO >= 100
#include "Arduino.h"   
#else
extern "C" {
#include "WConstants.h"
}
#endif
#include "AD9834.h"
#include <SPI.h>




//Master swtiches for pin vs software control of phase, freq, reset, and sleep functions
void AD9834::Use_Pins(int value) {
	if (value) {
		CONTROL |= 0x0100;
		PINSW = 1;
	}
	else {
		CONTROL &= ~0x0100;
		PINSW = 0;
	}
	SendWord(CONTROL);
}

void AD9834::Sign_Bit_On(int value) {
	if (value) {
		CONTROL |= 0x0028;
	}
	else {
		CONTROL &= ~0x0028;
	}
	SendWord(CONTROL);
}

void AD9834::DAC_ON(int value) {
	if (value) {
		CONTROL &= ~0x0040; // Clear the SLEEP12 bit.
	}
	else {
		CONTROL |= 0x0040; // Set the SLEEP12 bit.
	}
	SendWord(CONTROL);
}

void AD9834::SelectFREG(int value) {
	if (PINSW) {
		if (value)   digitalWrite(FreqSelPin, HIGH);
		else        digitalWrite(FreqSelPin, LOW);
	}
	else {
		if (value)   CONTROL |= 0x0800;
		else        CONTROL &= ~0x0800;
		SendWord(CONTROL);
	}
}

void AD9834::SelectPREG(int value) {
	if (PINSW) {
		if (value)   digitalWrite(PhaseSelPin, HIGH);
		else        digitalWrite(PhaseSelPin, LOW);
	}
	else {
		if (value)   CONTROL |= 0x0400;
		else        CONTROL &= ~0x0400;
		SendWord(CONTROL);
	}
}

void AD9834::Reset(int value) {
	if (PINSW) {
		if (value)   digitalWrite(ResetPin, HIGH);
		else        digitalWrite(ResetPin, LOW);
	}
	else {
		if (value)   CONTROL |= 0x0100;
		else        CONTROL &= ~0x0100;
		SendWord(CONTROL);
	}
}

void AD9834::Sleep(int value) {
	if (PINSW) {
		if (value)   digitalWrite(SleepPin, HIGH);
		else        digitalWrite(SleepPin, LOW);
	}
	else {
		if (value)   CONTROL |= 0x0080;
		else        CONTROL &= ~0x0080;
		SendWord(CONTROL);
	}
}

void AD9834::Triangle_Output(int value) {
	if (value)   CONTROL |= 0x0002;
	else        CONTROL &= ~0x0002;
	SendWord(CONTROL);
}

// Sets FREG0 to the value required to produce a specified frequency, in Hz.
unsigned long AD9834::SetFreq(int f_reg, unsigned long freq) {
	unsigned int data;
	unsigned long temp;
	unsigned int f_LSB;
	unsigned int f_MSB;
	temp = ((unsigned long)((float)freq*5.36871)) & 0x0FFFFFFF;
	if (f_reg == 1) {
		f_LSB = (0x8000 | (unsigned int)(temp & 0x00003FFF));
		f_MSB = (0x8000 | (unsigned int)((temp >> 14) & 0x3FFF));
	}
	else {
		f_LSB = (0x4000 | (unsigned int)(temp & 0x00003FFF));
		f_MSB = (0x4000 | ((unsigned int)(temp >> 14) & 0x3FFF));
	}
	SendWord(CONTROL | 0x2000);
	SendWord(f_LSB);
	SendWord(f_MSB);
	SendWord(CONTROL);
	return temp;
}

// Sets FREG0 to the value required to produce a specified frequency, in Hz.
// Also set an offset in units of 0.18626448439197 Hz
unsigned long AD9834::SetFreq(int f_reg, unsigned long freq, unsigned long offset) {
	unsigned int data;
	unsigned long temp;
	unsigned int f_LSB;
	unsigned int f_MSB;
	temp = ((unsigned long)((float)freq*5.36871)) & 0x0FFFFFFF;
	temp = temp + offset;
	if (f_reg == 1) {
		f_LSB = (0x8000 | (unsigned int)(temp & 0x00003FFF));
		f_MSB = (0x8000 | (unsigned int)((temp >> 14) & 0x3FFF));
	}
	else {
		f_LSB = (0x4000 | (unsigned int)(temp & 0x00003FFF));
		f_MSB = (0x4000 | ((unsigned int)(temp >> 14) & 0x3FFF));
	}
	SendWord(CONTROL | 0x2000);
	SendWord(f_LSB);
	SendWord(f_MSB);
	SendWord(CONTROL);
	return temp;
}


/* Initialises the AD9834, clearing all the registers, and putting it into sleep mode. 
	Leaving pins set to -1 forces system to use SPI version of the function (i.e. 
	use RESET command rather than RESET pin)*/
void AD9834::Setup(int cspin, 
	int phaseselpint=-1, 
	int fselpin=-1,
	int resetpin=-1,
	int sleeppin=-1) {

	CSPin = cspin;
	pinMode(CSPin, OUTPUT);
	digitalWrite(CSPin, HIGH); // Set FSYNC high - this goes low when writing to the registers.


	FreqSelPin = fselpin;
	if (FreqSelPin != -1)
	{
		pinMode(FreqSelPin, OUTPUT);
		digitalWrite(FreqSelPin, LOW); // Use FREG0 by default.
	}
	
	PhaseSelPin = phaseselpint;
	if (PhaseSelPin != -1)
	{
		pinMode(PhaseSelPin, OUTPUT);
		digitalWrite(PhaseSelPin, LOW); // Use FREG0 by default.
	}

	ResetPin = resetpin;
	if (ResetPin != -1)
	{
		pinMode(ResetPin, OUTPUT);
		digitalWrite(ResetPin, LOW); // Use FREG0 by default.
	}

	SleepPin = sleeppin;
	if (SleepPin != -1)
	{
		pinMode(SleepPin, OUTPUT);
		digitalWrite(SleepPin, LOW); // Use FREG0 by default.
	}


	SPI.beginTransaction(settings);
	// Set everything to off, basically.
	SendWord(CONTROL);

	// Null the phase Registers.
	SendWord(0xC000); // Phase register 0
	SendWord(0xE000); // Phase register 1

}

void AD9834::SINE_ON() {
	Sleep(0);
	DAC_ON(1);
	Reset(0);
}


// Send a 16-bit data word to the AD9834, MSB first.
// WARNING: I am using direct register writes in this function, and hence
// the FSEL pin is hardcoded to Port B pin 7. 
// To use this on any other PCB, this function will have to be modified.
void AD9834::SendWord(unsigned int data) {
	PORTB &= ~_BV(7);
	SPI.transfer((data >> 8) & 0xFF);
	SPI.transfer(data & 0xFF);
	PORTB |= _BV(7);
}

// Set the phase of a register, in degrees (0-359). 
void AD9834::SetPREG(unsigned int preg, unsigned int phase) {
	// check for an invalid phase.
	if (phase<0 || phase>359) {
		return;
	}

	unsigned int phaseval = 0x0FFF * ((float)phase / 360.0);

	if (preg == 0) {
		SendWord((0x0FFF & phaseval) + 0xC000);
	}
	else {
		SendWord((0x0FFF & phaseval) + 0xE000);
	}
}



