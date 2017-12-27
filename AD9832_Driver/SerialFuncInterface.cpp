// 
// 
// 

#include "SerialFuncInterface.h"

SerialFuncInterfaceClass::SerialFuncInterfaceClass()
{
	FuncList = new NameFuncCombo[MAX_NUM_FUNC];
	NameFuncCombo IDlink = { ID, this->IDFunc };
	FuncList[0] = IDlink;
	CurrentLen++;

	Serial.begin(115200);//baud is always 2M on teensy
	Serial.setTimeout(20);
}

String SerialFuncInterfaceClass::IDFunc(String * val)
{
	Serial.println(this->ID);
}

