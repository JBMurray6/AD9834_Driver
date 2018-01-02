#include "stdlib.h"
#include <SPI.h>
#include "AD9834.h"
#include "SerialFuncInterface.h"




#define FUNCNAME(x) x
#define GETFUNC(x,y) String FUNCNAME(x)(String * input){return String(y);}
#define SET_INT_FUNC(x,y) String FUNCNAME(x)(String * input){\
y=((String)(*input)).toInt();\
return String(y);}

AD9834 FuncGen = AD9834(48000000.0, 10);

SerialFuncInterfaceClass SerialFuncInterface = SerialFuncInterfaceClass(5);

int a = 10;
String aname = "Aname";
GETFUNC(GETA, a)
SET_INT_FUNC(SETA, a)


const String FreqPrefix = "Freq";
String GetFreq(String * S);
String SetFreq(String * S);
NameFuncCombo FreqFuncs = { FreqPrefix,SetFreq,GetFreq };

const String ModePrefix = "Mode";
String GetMode(String * S);
String SetMode(String * S);
const String SineModeStr = "Sine";
const String TriangleModeStr = "Triangle";
NameFuncCombo ModeFuncs = { ModePrefix,SetMode,GetMode };

const String FreqSweepPrefix = "Sweep";
String GetFreqSweep(String * S);
String SetFreqSweep(String * S);
NameFuncCombo FreqSweepFuncs = { FreqSweepPrefix,SetFreqSweep,GetFreqSweep };
enum SweepTypes
{
	LIN=0,
	LOG=1
};
const TypedParameter FreqSweepParams[] = {
	{ { (float)0.0 }, FloatVar, true },//Start
	{ { (float)0.0 }, FloatVar, true },//Stop
	{ { (int)1000 }, IntVar, false },//Num of milliseconds
	{ { (int)0 }, IntVar, true }//SweepType enum
};
TypedParameter CurrentSweep;


void setup()
{
	
	//Serial.begin(115200);
	//Serial.println("test");
  /* add setup code here */
	FuncGen.Init();
	




	SerialFuncInterface.AddFunc(ModeFuncs);
	SerialFuncInterface.AddFunc(FreqFuncs);
	FuncGen.SetFreq(AD9834::FreqReg::FREQ0, 1000, 0);
	FuncGen.SelectFREG(AD9834::FreqReg::FREQ0);
	FuncGen.SetMode(AD9834::SINUSOIDAL_WAVEFORM);
}

void loop()
{

  /* add main program code here */
	SerialFuncInterface.ParseSerial();
	//Serial.println("test");


}


String GetFreq(String *)
{
	return String(FuncGen.GetFreq());
}

String SetFreq(String * S)
{	
	float F = S->toFloat();
	FuncGen.SetFreq(AD9834::FreqReg::FREQ0, F,0);
	return "Set to "+ String(FuncGen.GetFreq());
}

String GetMode(String *)
{
	AD9834::Waveform mode = FuncGen.GetMode();
	switch (mode)
	{
	case (AD9834::TRIANGULAR_WAVEFORM):
		return TriangleModeStr;
	case (AD9834::SINUSOIDAL_WAVEFORM):
		return SineModeStr;
	default:
		return "Error impossible setting";
		break;
	}
	return String();
}

String SetMode(String * S)
{
	AD9834::Waveform mode;
	String modestr;
	if (S->startsWith(SineModeStr))
	{
		mode = AD9834::SINUSOIDAL_WAVEFORM;
		modestr = SineModeStr;
	}
	else if(S->startsWith(TriangleModeStr))
	{
		mode = AD9834::TRIANGULAR_WAVEFORM;
		modestr = TriangleModeStr;
	}
	else
	{
		return "Not a recognized type";
	}

	FuncGen.SetMode(mode);
	return "Set to " + modestr;
}

String SetFreqSweep(String * S)
{
	memcpy(&CurrentSweep, &FreqSweepParams, sizeof FreqSweepParams);
	int num = sizeof(CurrentSweep) / sizeof(TypedParameter);
	SerialFuncInterface.ParseArguments(&CurrentSweep, num, S);


}