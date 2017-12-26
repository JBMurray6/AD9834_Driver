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

#ifndef AD9834_h
#define AD9834_h

#include "Arduino.h" 
#include "SPI.h"

/**
* bits composing the configuration word
*/
#define B28         0x2000    /*set to true here (frequency is 28 bits wide)*/
#define HLB         0x1000    /*ignored since B28 == true*/
#define FSEL        0x0800    /*select FREQ0 or FREQ1 registers*/
#define PSEL        0x0400    /*select PHASE0 or PHASE 1 registers*/
#define PIN_SW      0x0200    /*determines software vs pin control*/
#define RESET       0x0100    /*active high; set DAC to mid scale*/
#define SLEEP1      0x0080    /*active high; suspend internal clock; DAC output remains at its present value*/
#define SLEEP12     0x0040    /*DAC sleep*/
#define OPBITEN     0x0020    /*active high; enable the SIGN_BIT_OUT pin*/
#define SIGN_PIB    0x0010    /*configure the SIGN_BIT_OUT*/
#define DIV2        0x0008    /*divide sign bit out by 2*/
#define MODE        0x0002    /*true = triangle output, false = sinusoidal output*/

/**
* register addresses
* The 2 MSB represent the address; OR'd with 14 LSB of data
*/
#define FREQ0_ADDR  0x4000
#define FREQ1_ADDR  0x8000
#define PHASE0_ADDR 0xC000
#define PHASE1_ADDR 0xE000


class AD9834
{


public:

	typedef enum {
		SINUSOIDAL_WAVEFORM,
		TRIANGULAR_WAVEFORM
	} Waveform;

	// Function Definitions
	void SendWord(unsigned int data);
	void Use_Pins(int value);
	void Sign_Bit_On(int value);
	void DAC_ON(int value);
	void SelectFREG(int value);
	void SelectPREG(int value);
	void Reset(int value);
	void Sleep(int value);
	void Triangle_Output(int value);
	unsigned long SetFreq(int f_reg, unsigned long freq);
	unsigned long SetFreq(int f_reg, unsigned long freq, unsigned long offset);
	
	
	void Setup(int cspin,
		int phaseselpint = -1,
		int fselpin = -1,
		int resetpin = -1,
		int sleeppin = -1);
	void SINE_ON();
	void SetPREG(unsigned int preg, unsigned int phase);

private:
	unsigned int CSPin = 11;
	unsigned int FreqSelPin = -1;
	unsigned int PhaseSelPin = -1;
	unsigned int ResetPin = -1;
	unsigned int SleepPin = -1;

	unsigned int CONTROL = 0x0000;
	int Pinswitch = 0;
	int FREG = 0;

	const SPISettings settings= SPISettings(16000000, LSBFIRST, SPI_MODE2);
};


#endif