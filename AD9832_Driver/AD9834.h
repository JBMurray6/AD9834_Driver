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
		TRIANGULAR_WAVEFORM,
		SQUARE_WAVEFORM
	} Waveform;

	enum FreqReg {
		FREQ1 = 1,
		FREQ0 = 0
	};

	enum PhaseReg {
		PAHSE1 = 1,
		PHASE0 = 0
	};

	// Function Definitions
	void Init();
	void Use_Pins(int value);
	void Sign_Bit_On(bool value);
	void Comparator_On(bool value);
	void DAC_ON(int value);
	void SelectFREG(FreqReg value);
	void SelectPREG(PhaseReg value);
	void Reset(bool value);
	void Sleep(bool value);

	unsigned long SetFreq(FreqReg f_reg, float freq, uint16_t offset);
	float GetFreq();


	void Mode(Waveform value);
	AD9834(float master_freq, int cspin,
		int phaseselpint = -1,
		int fselpin = -1,
		int resetpin = -1,
		int sleeppin = -1);

	void SetPhase(PhaseReg preg, unsigned int phase);
	float GetPhase();

private:
	int CSPin = 11;
	int FreqSelPin = -1;
	int PhaseSelPin = -1;
	int ResetPin = -1;
	int SleepPin = -1;

	float MasterClkFreq;
	float MasterClkFreqStep;

	//holds the current control word 
	uint16_t CONTROL = 0x0000;
	//master switch for pin vs. software control
	int PinSwitch = 0;

	//Current freq and phase values
	uint32_t FreqRegVal = 0;
	uint16_t PhaseRegVal = 0;

	const SPISettings settings = SPISettings(1000000, LSBFIRST, SPI_MODE2);

	void WaveOutput_ON();
	void Triangle_Output(int value);
	void SendWord(uint16_t data);
};


#endif