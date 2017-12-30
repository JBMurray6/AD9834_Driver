#include "SerialFuncInterface.h"
#include <SPI.h>
#include "AD9834.h"

AD9834 FuncGen = AD9834(40000000.0, 11);

//SerialFuncInterfaceClass SerialFuncInterface = SerialFuncInterfaceClass(3);

void setup()
{
	Serial.println("test");
  /* add setup code here */

}

void loop()
{

  /* add main program code here */
	//SerialFuncInterface.ParseSerial();
	Serial.println("test");
	delay(150);
}
