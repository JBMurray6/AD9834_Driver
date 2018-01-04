#include "stdlib.h"
#include <SPI.h>
#include "AD9834.h"
#include "SerialFuncInterface.h"
#include <arm_math.h>




#define FUNCNAME(x) x
#define GETFUNC(x,y) String FUNCNAME(x)(String * input){return String(y);}
#define SET_INT_FUNC(x,y) String FUNCNAME(x)(String * input){y=((String)(*input)).toInt();return String(y);}

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
	LIN = 0,
	LOG = 1
};
const int NumSweepParams = 4;

const float initfloat = 0;
const TypedParameter FreqSweepParams[NumSweepParams] = {
	{ initfloat , FloatVar,  true },//Start
	{ initfloat , FloatVar, true },//Stop
	{ (int)1000 , IntVar, false },//Num of milliseconds
	{  (int)0 , IntVar, true }//SweepType enum
};
TypedParameter CurrentSweep[NumSweepParams];
bool SweepGoing = false;
uint32_t StartMillisecods = 0;
float SweepRate;


void setup()
{

	//Serial.begin(115200);
	//Serial.println("test");
  /* add setup code here */
	FuncGen.Init();





	SerialFuncInterface.AddFunc(ModeFuncs);
	SerialFuncInterface.AddFunc(FreqFuncs);
	SerialFuncInterface.AddFunc(FreqSweepFuncs);
	FuncGen.SetFreq(AD9834::FreqReg::FREQ0, 1000, 0);
	FuncGen.SelectFREG(AD9834::FreqReg::FREQ0);
	FuncGen.SetMode(AD9834::SINUSOIDAL_WAVEFORM);
}

void loop()
{

	/* add main program code here */
	SerialFuncInterface.ParseSerial();
	//Serial.println("test");

	if (SweepGoing)
	{
		long del = millis() - StartMillisecods;
		float NewFreq=1000;
		if (CurrentSweep[2].Param.ival < del)//time
		{
			NewFreq = CurrentSweep[1].Param.fval;//make sure to hit the last val
			SweepGoing = false;
		}
		else
		{
			//Serial.println(CurrentSweep[3].Param.ival);
			switch (CurrentSweep[3].Param.ival)
			{
			case (SweepTypes::LIN):
				NewFreq = CurrentSweep[0].Param.fval + ((float)del)*(SweepRate);
				break;
			case (SweepTypes::LOG):
				NewFreq=(float)pow(10.0,((float)del)*(SweepRate)+log10(CurrentSweep[0].Param.fval));
				break;
			default:
				break;
			}			
		}
		//Serial.println("del: "+String(del)+" f: "+String(NewFreq,6));

		//Serial.println("1234asdf");
		FuncGen.SetFreq(AD9834::FreqReg::FREQ0, NewFreq, 0);
	}


}


String GetFreq(String *)
{
	return String(FuncGen.GetFreq());
}

String SetFreq(String * S)
{
	float F = S->toFloat();
	FuncGen.SetFreq(AD9834::FreqReg::FREQ0, F, 0);
	return "Set to " + String(FuncGen.GetFreq());
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
	else if (S->startsWith(TriangleModeStr))
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

	if (!SerialFuncInterface.ParseArguments(CurrentSweep, NumSweepParams, S))
	{
		Serial.println("Failed");
	}

	StartMillisecods = millis();
	switch (CurrentSweep[3].Param.ival)
	{
	case (SweepTypes::LIN):
		SweepRate = (CurrentSweep[1].Param.fval - CurrentSweep[0].Param.fval) / ((float)CurrentSweep[2].Param.ival);//delta freq/time
		break;
	case (SweepTypes::LOG):
		SweepRate = ((float)(log10(CurrentSweep[1].Param.fval) - log10(CurrentSweep[0].Param.fval))) /((float)CurrentSweep[2].Param.ival);//delta freq/time
		Serial.println(String((log10(CurrentSweep[1].Param.fval)-log10(CurrentSweep[0].Param.fval))/((float)CurrentSweep[2].Param.ival),10));
		//Serial.println(String((float)CurrentSweep[2].Param.ival));
		break;
	default:
		return "Not a sweep type";
	}

	SweepGoing = true;
	return "";
}

String GetFreqSweep(String * S)
{
	return String(CurrentSweep[0].Param.fval);
}