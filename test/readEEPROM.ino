#include <Arduino.h>
#include <EEPROM.h>

struct MaxData {
	unsigned long millis;
	double temperature;
	long pressure;
	double altitude;
};

void setup() {
	Serial.begin(9600); // PlatformIO default speed
	
	Serial.println("The previous text saved in the EEPROM was: ");
	MaxData EEPROMMaxData;
	EEPROM.get(0, EEPROMMaxData);

	Serial.println( "Read custom object from EEPROM: ");
	Serial.println( EEPROMMaxData.millis);
	Serial.println( EEPROMMaxData.temperature);
	Serial.println( EEPROMMaxData.pressure);
	Serial.println( EEPROMMaxData.altitude);
}

void loop() {

}