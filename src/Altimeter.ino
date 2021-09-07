// EDIT TO LAUNCH SITE ALTITUDE (in meters)(Only required for absolute altitude readings)
const double launchAltitude = 0;

// EDIT DEPENDING ON WHAT SENSORS ARE BEING USED (true or false)
const bool useMS5611 = true;	 // Can be stored in EEPROM & SD card
const bool useMPU6050 = false; // Can ONLY be stored in SD Card

// EDIT DEPENDING ON WHAT STORAGE TYPE IS BEING USED (true or false)
const bool useEEPROM = true; // Only one of these should be used at the same time
const bool useSD = false;

// START OF PROGRAM
#include <Arduino.h>
#include <Math.h>
#include <Wire.h>
#include <MS5611.h>
MS5611 ms5611;
long initialPressure;
double initialTemperature;

#include <I2Cdev.h>
#include <MPU6050.h>
MPU6050 accelgyro;

#include <EEPROM.h>
bool EEPROMLock = false;

#include <SPI.h>
#include <SD.h>
const int chipSelect = 10; // Defines the CS (chip select) pin

struct MaxData {
	unsigned long millis;
	double temperature;
	long pressure;
	double altitude;
};

void setup()
{
	Serial.begin(9600); // PlatformIO default speed
	

	if (useMS5611) // Initialize MS5611 sensor
	{
		Serial.println("Initializing MS5611 Sensor...");
		Serial.println(ms5611.begin(MS5611_ULTRA_HIGH_RES) ? "MS5611 connection successful" : "MS5611 connection failed");
		initialTemperature = ms5611.readTemperature();
		initialPressure = ms5611.readPressure();
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
		Serial.println(SD.begin(chipSelect) ? "SD Card connection successful" : "SD Card connection failed");
	}
	
	if (useEEPROM) // Read and clear EEPROM
	{
		Serial.println("The previous text saved in the EEPROM was: ");
		MaxData EEPROMMaxData;
		EEPROM.get(0, EEPROMMaxData);

		Serial.println( "Read custom object from EEPROM: ");
		Serial.println( EEPROMMaxData.millis);
		Serial.println( EEPROMMaxData.temperature);
		Serial.println( EEPROMMaxData.pressure);
		Serial.println( EEPROMMaxData.altitude);
	}
}

void loop()
{
	static MaxData maxAlt = {0,0,0,0};
	long realPressure;
	double realTemperature;
	double absoluteAltitude, relativeAltitude;

	int16_t ax, ay, az; // Define acceleration
	int16_t gx, gy, gz; // Define rotation(gyroscope)

	Serial.print(millis());

	//DATA COLLECTION
	if (useMS5611) // Reads MS5611 data
	{
		// Read true temperature & Pressure
		realTemperature = ms5611.readTemperature();
		realPressure = ms5611.readPressure();

		// Calculate altitude
		relativeAltitude = calculateAltitude(initialPressure, initialTemperature, realPressure, realTemperature);
		absoluteAltitude = relativeAltitude + launchAltitude;

		SerialprintMS5611(millis(), realTemperature, realPressure, absoluteAltitude, relativeAltitude);

		if (maxAlt.altitude < relativeAltitude) // Set Maximum Values
		{
			Serial.print(" - New Max Altitude reached");
			maxAlt = {millis(), realTemperature, realPressure, relativeAltitude};
		}
	}

	if (useMPU6050) // Reads MPU5060 data
	{
		// read raw accel/gyro measurements from device
		accelgyro.getMotion6(&gx, &gy, &gz, &ax, &ay, &az);
		SerialprintMPU6050(millis(), gx, gy, gz, ax, ay, az);
	}
 
	if (useSD) // Writes data from MS5611 & MPU6050 to SD Card
	{
		if (useMS5611)
		{
			SDprintMS5611(millis(), realTemperature, realPressure, absoluteAltitude, relativeAltitude);
		}

		if (useMPU6050)
		{
			SDprintMPU6050(millis(), gx, gy, gz, ax, ay, az);
		}
	}

	if (useEEPROM) // Only writes MS5611 data at highest altitude to EEPROM
	{
		if (EEPROMLock == false && millis() >= 10000/*120000*/ && abs(relativeAltitude) < 10) // if nothing has been written before && if after 120 seconds && if within 10 meters of starting height.
		{
			EEPROMwriteMS5611(maxAlt);
		}
	}

	Serial.println("");

	delay(100);
}


double calculateAltitude(long lowerPressure, double lowerTemperature, long higherPressure, double higherTemperature) // pressure in pascals, temperature in celcius
{
	// Uses the hypsometric equation. Information here: https://glossary.ametsoc.org/wiki/Hypsometric_equation

	double p1 = lowerPressure;
	double p2 = higherPressure;
	double t1 = 273.15 + lowerTemperature;	// conversion to kelvin
	double t2 = 273.15 + higherTemperature;	// conversion to kelvin

	double rv1 = (0.622 * EULER) / (p1 - EULER); // mixing ratio : https://glossary.ametsoc.org/wiki/Mixing_ratio
	double rv2 = (0.622 * EULER) / (p2 - EULER); // mixing ratio : https://glossary.ametsoc.org/wiki/Mixing_ratio

	double Tv1 = (1 + 0.61 * rv1) * t1;	// approximate virtual temperature : https://glossary.ametsoc.org/wiki/Virtual_temperature
	double Tv2 = (1 + 0.61 * rv2) * t2;	// approximate virtual temperature : https://glossary.ametsoc.org/wiki/Virtual_temperature
	double M_Tv = (Tv1 + Tv2) / 2;		// mean virtual temperature in kelvins [K] : https://glossary.ametsoc.org/wiki/Virtual_temperature

	double Rd = 287.058; // specific gas constant for dry air [J/(kg⋅K)]: https://en.wikipedia.org/wiki/Gas_constant
	double g = 9.80665;	 // gravitational acceleration [m/s2] : https://en.wikipedia.org/wiki/Standard_gravity

	double h = (Rd * M_Tv / g) * log(p1 / p2); // hypsometric equation : https://glossary.ametsoc.org/wiki/Hypsometric_equation

	return h;
}

void SerialprintMS5611(long time, double rT, long rP, double aA, double rA)
{
	Serial.print(", Real Temperature: ");
	Serial.print(rT);	Serial.print(", Real Pressure: ");
	Serial.print(rP);	Serial.print(", Absolute Altitude: ");
	Serial.print(aA);	Serial.print(", Relative Altitude: ");
	Serial.print(rA);
}

void SerialprintMPU6050(long time, int16_t gx, int16_t gy, int16_t gz, int16_t ax, int16_t ay, int16_t az)
{
	Serial.print(", gx: ");
	Serial.print(gx);	Serial.print(", gy: ");
	Serial.print(gy);	Serial.print(", gz: ");
	Serial.print(gz);	Serial.print(", ax: ");
	Serial.print(ax);	Serial.print(", ay: ");
	Serial.print(ay);	Serial.print(", az: ");
	Serial.print(az);
	
	
}

void SDprintMS5611(long time, double rT, long rP, double aA, double rA)
{
	File dataFile = SD.open("datalog.txt", FILE_WRITE);

	if (dataFile)
	{ // if the file is available, write to it:
		dataFile.print("\n");
		dataFile.print(time);	dataFile.print(",");
		dataFile.print(rT);		dataFile.print(",");
		dataFile.print(rP);		dataFile.print(",");
		dataFile.print(aA);		dataFile.print(",");
		dataFile.print(rA);		dataFile.close();
	}
	else // if the file isn't open, pop up an error:
	{
		Serial.println("error opening data file");
	}
}

void SDprintMPU6050(long time, int16_t gx, int16_t gy, int16_t gz, int16_t ax, int16_t ay, int16_t az)
{
	File dataFile = SD.open("datalog.txt", FILE_WRITE);

	if (dataFile)
	{ // if the file is available, write to it:
		if (!useMS5611) {
			dataFile.print("\n");
			Serial.print(time);
		} 

		dataFile.print(",");
		dataFile.print(ax);	dataFile.print(",");
		dataFile.print(ay);	dataFile.print(",");
		dataFile.print(az);	dataFile.print(",");
		dataFile.print(gx);	dataFile.print(",");
		dataFile.print(gy);	dataFile.print(",");
		dataFile.print(gz);	dataFile.close();
	}
	else // if the file isn't open, pop up an error:
	{
		Serial.println("error opening data file");
	}
}

void EEPROMwriteMS5611(MaxData maxAlt) 
{
	int i = 0;
	EEPROM.put(i, maxAlt);

	Serial.println("\n--- DATA HAS BEEN WRITTEN TO EEPROM ---");
	EEPROMLock = true; // stops arduino from writing again. This is to avoid memory degradation.
}