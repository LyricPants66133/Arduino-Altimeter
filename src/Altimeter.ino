// EDIT TO LAUNCH SITE ALTITUDE (in meters)
double myRealAltitude = 0;

// EDIT DEPENDING ON WHAT SENSORS ARE BEING USED (true or false)
bool useMS5611 = false;	 // Can be stored in EEPROM & SD card
bool useMPU6050 = false; // Can ONLY be stored in SD Card

// EDIT DEPENDING ON WHAT STORAGE TYPE IS BEING USED (true or false)
bool useEEPROM = false;
bool useSD = false;

// START OF PROGRAM
#include <Arduino.h>
#include <Wire.h>
#include <MS5611.h>
MS5611 ms5611;
long realPressure, seaLevelPressure, maxAltPressure;
double realTemperature, maxAltTemperature;
double absoluteAltitude, relativeAltitude;
double maxAltitude = 0;
long millisMaxAlt;

#include <I2Cdev.h>
#include <MPU6050.h>
MPU6050 accelgyro;
int16_t ax, ay, az; // Define acceleration
int16_t gx, gy, gz; // Define rotation(gyroscope)

#include <EEPROM.h>
bool EEPROMLock = false;
int EEPROMLength = EEPROM.length();

#include <SPI.h>
#include <SD.h>
const int chipSelect = 4; // Defines the CS (chip select) pin

void setup()
{
	Serial.begin(9600); // PlatformIO default speed

	if (useMS5611) // Initialize MS5611 sensor
	{
		Serial.println("Initializing MS5611 Sensor...");
		Serial.println(ms5611.begin(MS5611_ULTRA_HIGH_RES) ? "MS5611 connection successful" : "MS5611 connection failed");

		// Calculate sealevel pressure
		seaLevelPressure = ms5611.getSeaLevel(ms5611.readPressure(), myRealAltitude);
	}

	if (useMPU6050) // Initialize MPU6050 sensor
	{
		Serial.println("Initializing MPU6050 sensor...");
		accelgyro.initialize();
		Serial.println(!accelgyro.testConnection() ? "MPU6050 connection successful" : "MPU6050 connection failed");
	}

	if (useSD) // Initialize SD Card
	{
		Serial.print("Initializing SD card...");
		Serial.println(!SD.begin(chipSelect) ? "SD Card connection successful" : "SD Card connection failed");
	}

	if (useEEPROM)
	{
		Serial.println("The previous text saved in the EEPROM was: ");

		for (int i = 0; i < EEPROMLength; i++) // Read EEPROM
		{
			Serial.write(EEPROM.read(i));
		}

		for (int i = 0; i < EEPROMLength; i++) // Clear EEPROM
		{
			EEPROM.write(i, atoi("*"));
		}
	}
}

void loop()
{
	String data = "";

	Serial.print(millis());

	if (useMS5611) // Reads MS5611 data
	{
		// Read true temperature & Pressure
		realTemperature = ms5611.readTemperature();
		realPressure = ms5611.readPressure();

		// Calculate altitude
		absoluteAltitude = ms5611.getAltitude(realPressure, seaLevelPressure);
		relativeAltitude = absoluteAltitude - myRealAltitude;

		Serial.print(", realPressure: " + String(realPressure) + "Pa");
		Serial.print(", realTemp: " + String(realTemperature) + "*C");
		Serial.print(", absoluteAltitude: " + String(absoluteAltitude) + "m");
		Serial.print(", relativeAltitude: " + String(relativeAltitude) + "m");

		if (maxAltitude < relativeAltitude) // Set Maximum Values
		{
			millisMaxAlt = millis();
			maxAltTemperature = realTemperature;
			maxAltPressure = realPressure;
			maxAltitude = relativeAltitude;
		}
	}

	if (useMPU6050) // Reads MPU5060 data
	{
		// read raw accel/gyro measurements from device
		accelgyro.getMotion6(&gx, &gy, &gz, &ax, &ay, &az);
		Serial.println(", gx: " + String(gx) + ", gy: " + String(gy) + ", gz: " + String(gz) + ", ax: " + String(ax) + ", ay: " + String(ay) + ", az: " + String(az));
	}

	if (useSD) // Writes data from MS5611 & MPU6050 to SD Card
	{
		if (useMS5611)
		{
			data = String(millis()) + ", " + String(realTemperature) + ", " + String(realPressure) + ", " + String(absoluteAltitude) + ", " + String(relativeAltitude);
		}

		if (useMS5611 && useMPU6050) // add an extra comma if both sensors are being used
		{
			data += ", ";
		}

		if (useMPU6050)
		{
			data += String(ax) + ", " + String(ay) + ", " + String(az) + ", " + String(gx) + ", " + String(gy) + ", " + String(gz);
		}

		SD_Write(data);
	}

	if (useEEPROM) // Only writes MS5611 data at highest altitude to EEPROM
	{

		if (EEPROMLock == 0 && millis() >= 120000 && abs(relativeAltitude) < 10) // if nothing has been written before ; if after 120 seconds ; if within 10 meters of starting height.
		{
			data = String(millisMaxAlt) + "," + String(maxAltPressure) + "," + String(maxAltTemperature) + "," + String(maxAltitude);
			EEPROM_Write(data);
		}
	}

	Serial.println("");

	delay(100);
}

void EEPROM_Write(String data)
{
	byte len = data.length(); // Get length of data

	for (int i = 0; i < len || i < EEPROMLength; i++) // write up to string length or EEPROM size(shouldnt ever hit this point, but good precatution)
	{
		EEPROM.write(i, data[i]);
	}

	Serial.println("--- DATA HAS BEEN WRITTEN TO EEPROM ---");
	Serial.println("--- " + data + "---");
	EEPROMLock = true; // 'lock' the EEPROM so that it cant be written over
}

void SD_Write(String data)
{
	File dataFile = SD.open("datalog.txt", FILE_WRITE);

	if (dataFile)
	{ // if the file is available, write to it:
		dataFile.println(data);
		dataFile.close();
	}
	else
	{ // if the file isn't open, pop up an error:
		Serial.println("error opening datalog.txt");
	}
}