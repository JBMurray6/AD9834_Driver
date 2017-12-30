#include "SerialFuncInterface.h"
#include <SPI.h>
#include "AD9834.h"

#define FUNCNAME(x) x
#define GETFUNC(x,y) String FUNCNAME(x)(String * input){return String(y);}
#define SET_INT_FUNC(x,y) String FUNCNAME(x)(String * input){\
y=((String)(*input)).toInt();\
return String(y);}

AD9834 FuncGen = AD9834(40000000.0, 11);

SerialFuncInterfaceClass SerialFuncInterface = SerialFuncInterfaceClass(5);

int a = 10;
String aname = "Aname";
GETFUNC(GETA, a)
SET_INT_FUNC(SETA, a)

float b = 11;
String bname = "Freq";
GETFUNC(bnamef, b)

NameFuncCombo ANFC = { aname,FUNCNAME(SETA),FUNCNAME(GETA) };


void setup()
{
	
	//Serial.begin(115200);
	//Serial.println("test");
  /* add setup code here */
	FuncGen.Init();
	

	NameFuncCombo BNFC = { bname,SetFreq,GetFreq };

	SerialFuncInterface.AddFunc(ANFC);
	SerialFuncInterface.AddFunc(BNFC);
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
	return "Set to "+*S;
}

//String Afunc(String * input)
//{
//	return String(a);
//}