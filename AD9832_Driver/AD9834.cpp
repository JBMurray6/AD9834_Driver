/*
AD9834.cpp - AD9834 Direct Digital Synthesis IC Library

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




/* Initialises the AD9834, clearing all the registers, and putting it into sleep mode.
master_freq is the clock used to drive the AD9834 (not the SPI clock). This is needed
to properly set the freq
Leaving pins set to -1 forces system to use SPI version of the function (i.e.
use RESET command rather than RESET pin)*/
AD9834::AD9834(float master_freq, int cspin,
	int phaseselpint,
	int fselpin,
	int resetpin,
	int sleeppin) {

	MasterClkFreq = master_freq;
	MasterClkFreqStep = MasterClkFreq / ((float)0xFFFFFFF);//7 F's for 28 bits

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


}

void AD9834::Init()
{
	SPI.begin();


	// Set everything to off, basically.
	SendWord(CONTROL);
	// Null the phase Registers.
	SendWord(0xC000); // Phase register 0
	SendWord(0xE000); // Phase register 1
}

//Master swtiches for pin vs software control of phase, freq, reset, and sleep functions
void AD9834::Use_Pins(int value) {
	if (value) {
		CONTROL |= PIN_SW;
		PinSwitch = 1;
	}
	else {
		CONTROL &= ~PIN_SW;
		PinSwitch = 0;
	}
	SendWord(CONTROL);
}

void AD9834::Sign_Bit_On(bool value) {
	if (value) {
		CONTROL |= OPBITEN;
	}
	else {
		CONTROL &= ~OPBITEN;
	}
	SendWord(CONTROL);
}

void AD9834::Comparator_On(bool value)
{
	if (value) {
		CONTROL |= SIGN_PIB;
	}
	else {
		CONTROL &= ~SIGN_PIB;
	}
	SendWord(CONTROL);
}

void AD9834::DAC_ON(int value) {
	if (value) {
		CONTROL &= ~SLEEP12; // Clear the SLEEP12 bit.
	}
	else {
		CONTROL |= SLEEP12; // Set the SLEEP12 bit.
	}
	SendWord(CONTROL);
}

void AD9834::SelectFREG(FreqReg value) {
	if (PinSwitch&(FreqSelPin!=-1)) {
		if (value)   digitalWrite(FreqSelPin, HIGH);
		else        digitalWrite(FreqSelPin, LOW);
	}
	else {
		if (value)   CONTROL |= FSEL;
		else        CONTROL &= ~FSEL;
		SendWord(CONTROL);
	}
}

void AD9834::SelectPREG(PhaseReg value) {
	if (PinSwitch&(PhaseSelPin!=-1)) {
		if (value)   digitalWrite(PhaseSelPin, HIGH);
		else        digitalWrite(PhaseSelPin, LOW);
	}
	else {
		if (value)   CONTROL |= PSEL;
		else        CONTROL &= ~PSEL;
		SendWord(CONTROL);
	}
}

void AD9834::Reset(bool value) {
	if (PinSwitch&(ResetPin != -1)) {
		if (value)   digitalWrite(ResetPin, HIGH);
		else        digitalWrite(ResetPin, LOW);
	}
	else {
		if (value)   CONTROL |= RESET;
		else        CONTROL &= ~RESET;
		SendWord(CONTROL);
	}
}

void AD9834::Sleep(bool value) {
	if (PinSwitch&(SleepPin != -1)) {
		if (value)   digitalWrite(SleepPin, HIGH);
		else        digitalWrite(SleepPin, LOW);
	}
	else {
		if (value)   CONTROL |= SLEEP1;
		else        CONTROL &= SLEEP1;
		SendWord(CONTROL);
	}
}
//todo come back to this
void AD9834::Triangle_Output(int value) {

}


void AD9834::WaveOutput_ON() {
	Sleep(0);
	DAC_ON(1);
	Reset(0);
}

// Sets FREG0 to the value required to produce a specified frequency, in Hz.
unsigned long AD9834::SetFreq(FreqReg f_reg, float freq, uint16_t offset=0) {
	

	uint16_t f_LSB;
	uint16_t  f_MSB;
	FreqRegVal = ((uint32_t)(freq / MasterClkFreqStep)) & 0x0FFFFFFF;//7 F's for 28 bits
	FreqRegVal += offset;
	if (f_reg == 1) {
		f_LSB = (FREQ1_ADDR | (uint16_t)(FreqRegVal & 0x00003FFF));
		f_MSB = (FREQ1_ADDR | (uint16_t)((FreqRegVal >> 14) & 0x3FFF));
	}
	else {
		f_LSB = (FREQ0_ADDR | (uint16_t)(FreqRegVal & 0x00003FFF));
		f_MSB = (FREQ0_ADDR | ((uint16_t)(FreqRegVal >> 14) & 0x3FFF));
	}
	SendWord(CONTROL | B28);
	SendWord(f_LSB);
	SendWord(f_MSB);
	SendWord(CONTROL);//todo drop this but check other usage first
	return FreqRegVal;
}

float AD9834::GetFreq()
{
	return ((float)FreqRegVal)*MasterClkFreqStep;
}

//Sets the waveform 
void AD9834::SetMode(Waveform value)
{
	WaveOutput_ON();
	switch (value)
	{
	case (Waveform::TRIANGULAR_WAVEFORM):
		CONTROL |= MODE;
		break;

	case (Waveform::SINUSOIDAL_WAVEFORM):
		CONTROL &= ~MODE;
		break;
	default:
		break;
	}
	SendWord(CONTROL);
}

AD9834::Waveform AD9834::GetMode()
{
	if (CONTROL&MODE)
	{
		return TRIANGULAR_WAVEFORM;
	}
	else
	{
		return SINUSOIDAL_WAVEFORM;
	}
}




// Send a 16-bit data word to the AD9834, MSB first.
// WARNING: I am using direct register writes in this function, and hence
// the FSEL pin is hardcoded to Port B pin 7. 
// To use this on any other PCB, this function will have to be modified.
void AD9834::SendWord(uint16_t data) {
	SPI.beginTransaction(settings);

	

	digitalWrite(CSPin, LOW);

	//delay(1);
	SPI.transfer((data >> 8) & 0xFF);
	SPI.transfer(data & 0xFF);
	digitalWrite(CSPin, HIGH);
	SPI.endTransaction();
}

// Set the phase of a register, in degrees (0-359). 
void AD9834::SetPhase(PhaseReg preg, unsigned int phase) {
	// check for an invalid phase.
	if (phase<0 || phase>359) {
		return;
	}

	unsigned int PhaseRegVal = 0x0FFF * ((float)phase / 360.0);//phase is 12 bits


	if (preg == 0) {
		SendWord((0x0FFF & PhaseRegVal) + 0xC000);
	}
	else {
		SendWord((0x0FFF & PhaseRegVal) + 0xE000);
	}
}

float AD9834::GetPhase()
{
	return PhaseRegVal*360.0;
}


