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
	String(*GetFunc)(String *);	
};

typedef struct NameFuncCombo NameFuncCombo;

union ParameterUnion
{
	int ival;
	float fval;
};

enum ParameterTypes
{
	FloatVar = 0,
	IntVar = 1
};

struct TypedParameter
{
	ParameterUnion Param;
	ParameterTypes Type;
	bool Required;
};

typedef struct TypedParameter TypedParameter;

class SerialFuncInterfaceClass
{
 protected:
	 static const char RequestChar = '?';
	 static const char Terminator = ';';
	 static const char Delimiter = ',';

	 const String ConnectString = "Hi computer!\n";
	 static String ID ;

	 const String IDPrefix = "ID";
	 const String ErrorPrefix = "Error";

	 unsigned int MAX_NUM_FUNC = 16;
	 unsigned int CurrentLen = 0;
	 NameFuncCombo * FuncList;

	 unsigned int Baud = 115200;
	 unsigned int Timeout = 5;



 public:
	SerialFuncInterfaceClass();
	SerialFuncInterfaceClass(unsigned int maxfuncnum);
	SerialFuncInterfaceClass(unsigned int maxfuncnum, String id);

	void SetBaud(unsigned int baud);

	bool AddFunc(NameFuncCombo nfc);
	bool AddFunc(NameFuncCombo nfc[],unsigned int length);
	bool ParseSerial();

	static String IDFunc(String * val);

	static bool ParseArguments(TypedParameter * out, int outlen, String * In);

};


#endif

