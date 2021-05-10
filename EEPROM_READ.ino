#include <EEPROM.h>
int EEPROM_Data = 0;
int EEPROM_Size = 1024;

void setup() {
    Serial.println("The previous text saved in the EEPROM was: ");
	for (int i = 0; i < EEPROM_Size; i++) // Read EEPROM
	{
		EEPROM_Data = EEPROM.read(i);
		Serial.write(EEPROM_Data);
	}
}

void loop(){}