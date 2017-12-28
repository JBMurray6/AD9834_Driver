// 
// 
// 

#include "SerialFuncInterface.h"

SerialFuncInterfaceClass::SerialFuncInterfaceClass()
{
	FuncList = new NameFuncCombo[MAX_NUM_FUNC];
	NameFuncCombo IDlink = { ID, this->IDFunc, this->IDFunc };
	FuncList[0] = IDlink;
	CurrentLen++;

	Serial.begin(Baud);//baud is always 2M on teensy
	Serial.setTimeout(Timeout);
}

SerialFuncInterfaceClass::SerialFuncInterfaceClass(unsigned int maxfuncnum)
{
	MAX_NUM_FUNC = maxfuncnum;
	FuncList = new NameFuncCombo[MAX_NUM_FUNC];
	NameFuncCombo IDlink = { ID, this->IDFunc, this->IDFunc };
	FuncList[0] = IDlink;
	CurrentLen++;

	Serial.begin(Baud);//baud is always 2M on teensy
	Serial.setTimeout(Timeout);
}

SerialFuncInterfaceClass::SerialFuncInterfaceClass(unsigned int maxfuncnum, String id)
{
	MAX_NUM_FUNC = maxfuncnum;
	ID = id;
	FuncList = new NameFuncCombo[MAX_NUM_FUNC];
	NameFuncCombo IDlink = { ID, this->IDFunc, this->IDFunc };
	FuncList[0] = IDlink;
	CurrentLen++;

	Serial.begin(Baud);//baud is always 2M on teensy
	Serial.setTimeout(Timeout);
}

void SerialFuncInterfaceClass::SetBaud(unsigned int baud)
{
	Baud = baud;
	Serial.begin(Baud);
}

String SerialFuncInterfaceClass::IDFunc(String * val)
{
	Serial.println(this->ID);
	return "";
}

bool SerialFuncInterfaceClass::AddFunc(NameFuncCombo nfc)
{
	if (CurrentLen >= MAX_NUM_FUNC)
	{
		return false;
	}
	else
	{
		FuncList[CurrentLen] = nfc;
		CurrentLen++;
	}
	return true;
}

bool SerialFuncInterfaceClass::AddFunc(NameFuncCombo nfc[], unsigned int length)
{
	for (int i = 0; i < length; i++)
	{
		if (!AddFunc(nfc[i]))
		{
			return false;
		}
	}
	return true;
}


bool SerialFuncInterfaceClass::ParseSerial()
{
	bool done = false;

	String InMessage = Serial.readString();

	int FirstPos = InMessage.indexOf(Terminator);
	if (FirstPos == -1) //Check to see if it is a message at all
	{
		return false;
	}
	//Serial.println(String(millis()-t));
	while (!done)
	{
		String TempStr = InMessage.substring(0, FirstPos);
		bool found = false;
		for (int i = 0; i < CurrentLen; i++)
		{
			if (TempStr.startsWith(FuncList[i].Identifier))
			{
				found = true;
				TempStr.remove(0, FuncList[i].Identifier.length());
				TempStr.trim();
				if (TempStr.indexOf(RequestChar) != -1) //contains '?'
				{
					FuncList->GetFunc(&TempStr);
				}
				else
				{
					FuncList->Func(&TempStr);
				}
				break;
			}
		}
		if(!found)//no message was found
		{
			Serial.println(ErrorPrefix + ": unrecognized message ");
		}

		//Check for multiple messages
		InMessage.remove(0, FirstPos + 1);
		FirstPos = InMessage.indexOf(Terminator);
		if (FirstPos == -1) //Check to see if it is a message at all
		{
			done = true;
		}

	}
	return done;
}