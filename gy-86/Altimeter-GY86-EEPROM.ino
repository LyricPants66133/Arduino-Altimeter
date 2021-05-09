/*
JOB: reads pressure, altitude and temprature. Records MAX Values in EEPROM. Values can be retrieved using EEPROM-READ.ino.
GY-86:
    VCC_IN  :   5V
    3.3V    :   UNUSED
    GND     :   GND
    SCL     :   A5
    SDA     :   A4
    FSYNC   :   UNUSED
    INTA    :   2
    DRDY    :   UNUSED
*/

#include <Wire.h>
#include <MS5611.h>
MS5611 ms5611;
double referencePressure;
long max_Pressure = 0;
float max_Altitude = 0;
double max_Temprature = 0;

#include <EEPROM.h>
int EEPROM_Data = 0;
int EEPROM_Size = 1024;
int EEPROM_Lock = 0;

void setup()
{
	Serial.begin(9600); //standard value

	// Initialize MS5611 sensor
	Serial.println("Initializing MS5611 Sensor...");
	Serial.println(ms5611.begin(MS5611_ULTRA_HIGH_RES) ? "MS5611 connection successful" : "MS5611 connection failed");

	// Get reference pressure for relative altitude
	referencePressure = ms5611.readPressure();

	Serial.println("The previous text saved in the EEPROM was: ");
	for (int i = 0; i < EEPROM_Size; i++) // Read EEPROM
	{
		EEPROM_Data = EEPROM.read(i);
		Serial.write(EEPROM_Data);
	}

	for (int i = 0; i < EEPROM_Size; i++) // Clear EEPROM
	{
		EEPROM.write(i, uint8_t("")); // uint8_t is there to clear up warnings
	}
}

void loop()
{
	// Read raw values
	uint32_t rawTemp = ms5611.readRawTemperature();
	uint32_t rawPressure = ms5611.readRawPressure();
	// Read true temperature & Pressure
	double real_Temperature = ms5611.readTemperature();
	long real_Pressure = ms5611.readPressure();

	// Calculate altitude
	float absolute_Altitude = ms5611.getAltitude(real_Pressure);
	float relative_Altitude = ms5611.getAltitude(real_Pressure, referencePressure);

	Serial.print(millis());
	Serial.print(", real_Pressure:");
	Serial.print(real_Pressure);
	Serial.print(", real_Temp:");
	Serial.print(real_Temperature);
	Serial.print(", absoluteAltitude:");
	Serial.print(absolute_Altitude);
	Serial.print(", relativeAltitude:");
	Serial.println(relative_Altitude);

	if (max_Pressure < real_Pressure)
	{
		max_Pressure = real_Pressure;
	}
	if (max_Altitude < absolute_Altitude)
	{
		max_Altitude = relative_Altitude;
	}
	if (max_Temprature < real_Temperature)
	{
		max_Temprature = real_Temperature;
	}
	if (EEPROM_Lock == 0 && millis() >= 120000 && abs(max_Altitude - relative_Altitude) < 10) // if nothing has been written before ; if after 120 seconds ; if within 10 meters of starting height.
	{
		String data = String(millis()) + String(max_Pressure) + "," + String(max_Altitude) + "," + String(max_Temprature);
		EEPROM_Write(data);
	}

	delay(100);
}

void EEPROM_Write(String data)
{
	byte len = data.length();						 // Get length of data
	for (int i = 0; i < len && i < EEPROM_Size; i++) //write up to string length or EEPROM size(shouldnt ever hit this point, but good precatution)
	{
		EEPROM.write(i, data[i]);
	}

	Serial.print("--- DATA HAS BEEN WRITTEN TO EEPROM ---");
	Serial.println("--- " + data + "---");
	EEPROM_Lock = 1; // 'lock' the EEPROM so that it cant be written over
}