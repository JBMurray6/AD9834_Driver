// SerialFuncInterface.h

#ifndef _SERIALFUNCINTERFACE_h
#define _SERIALFUNCINTERFACE_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "wprogram.h"
#else
	#include "WProgram.h"
#endif

struct NameFuncCombo
{
	String Identifier;
	String(*Func)(String *);
};

typedef struct NameFuncCombo NameFuncCombo;

class SerialFuncInterfaceClass
{
 protected:
	 const char RequestChar = '?';
	 const char Terminator = ';';
	 const char Delimiter = ',';

	 const String ConnectString = "Hi computer!\n";
	 String ID = "20171227_JBM";

	 const String IDPrefix = "ID";

	 unsigned int MAX_NUM_FUNC = 16;
	 unsigned int CurrentLen = 0;
	 NameFuncCombo * FuncList;



 public:
	 SerialFuncInterfaceClass();
	 SerialFuncInterfaceClass(unsigned int maxfuncnum);
	 SerialFuncInterfaceClass(unsigned int maxfuncnum, String id);

	bool AddFunc(NameFuncCombo nfc);
	bool AddFunc(NameFuncCombo nfc[],unsigned int length);
	String ParseSerial();

	String IDFunc(String * val);
};


#endif

