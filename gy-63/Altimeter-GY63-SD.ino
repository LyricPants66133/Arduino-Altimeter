/*
JOB: reads pressure, altitude and temprature. Records values every 0.1 seconds in Micro SD Card.
GY-63:
    VCC     :	5V
	GND	    :	GND
	SCL 	:	A5
	SDA	    :	A4
	CSB 	:	UNUSED
	SDO	    :	UNUSED
	PS  	:	UNUSED

SD READER:
    GND     :   GND
    VCC     :   5V
    MISO    :   12
    MOSI    :   11
    SCK     :   13
    CS      :   4

*/

// Setup sensor libraries & variables for each
#include <Wire.h>
#include <MS5611.h>
MS5611 ms5611;
double reference_Pressure;

#include <SPI.h>
#include <SD.h>
const int chip_Select = 4; // this defines the CS (chip select) pin

void setup()
{
    Serial.begin(9600); //baudrate 9600 because value is standard

    // Initialize MS5611 sensor
    Serial.println("Initializing MS5611 Sensor...");
    Serial.println(ms5611.begin(MS5611_ULTRA_HIGH_RES) ? "MS5611 connection successful" : "MS5611 connection failed");

    //Initialize SD Card
    Serial.print("Initializing SD card...");
    Serial.println(!SD.begin(chip_Select) ? "SD Card connection successful" : "SD Card connection failed");

    // Get reference pressure for relative altitude
    reference_Pressure = ms5611.readPressure();
}

void loop()
{
    // Calculate altitude
    float absolute_Altitude = ms5611.getAltitude(real_Pressure);
    float relative_Altitude = ms5611.getAltitude(real_Pressure, reference_Pressure);

    // Print out all values
    Serial.println("--");

    Serial.print("realTemp: ");
    Serial.print(real_Temperature);
    Serial.println(" *C");

    Serial.print("real_Pressure: ");
    Serial.print(real_Pressure);
    Serial.println(" Pa");

    Serial.print("absoluteAlt: ");
    Serial.print(absolute_Altitude);
    Serial.println(" m");

    Serial.print("relativeAlt: ");
    Serial.print(relative_Altitude);
    Serial.println(" m");

    String data = String(millis()) + ", " + String(real_Temperature) + ", " + String(real_Pressure) + ", " + String(absolute_Altitude) + ", " + String(relative_Altitude);
    SD_Write(data);

    delay(100);
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