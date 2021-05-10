/*
JOB: reads pressure, altitude, temprature, gyroscope, and accelerometer. Records values every 0.1 seconds in Micro SD Card.
GY-86:
    VCC_IN  :   5V
    3.3V    :   UNUSED
    GND     :   GND
    SCL     :   A5
    SDA     :   A4
    FSYNC   :   UNUSED
    INTA    :   2
    DRDY    :   UNUSED

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

#include <I2Cdev.h>
#include <MPU6050.h>
MPU6050 accelgyro;
int16_t ax, ay, az;
int16_t gx, gy, gz;

#include <SPI.h>
#include <SD.h>
const int chip_Select = 4; // this defines the CS (chip select) pin

void setup()
{
// join I2C bus (I2Cdev library doesn't do this automatically)
#if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
    Wire.begin();
#elif I2CDEV_IMPLEMENTATION == I2CDEV_BUILTIN_FASTWIRE
    Fastwire::setup(400, true);
#endif

    Serial.begin(9600); //baudrate 9600 because value is standard

    // Initialize MS5611 sensor
    Serial.println("Initializing MS5611 Sensor...");
    Serial.println(ms5611.begin(MS5611_ULTRA_HIGH_RES) ? "MS5611 connection successful" : "MS5611 connection failed");

    // Initialize MPU6050 sensor
    Serial.println("Initializing MPU6050 sensor...");
    accelgyro.initialize();
    Serial.println(!accelgyro.testConnection() ? "MPU6050 connection successful" : "MPU6050 connection failed");

    //Initialize SD Card
    Serial.print("Initializing SD card...");
    Serial.println(!SD.begin(chip_Select) ? "SD Card connection successful" : "SD Card connection failed");

    // Get reference pressure for relative altitude
    reference_Pressure = ms5611.readPressure();
}

void loop()
{
    // Read true temperature & Pressure
    double real_Temperature = ms5611.readTemperature();
    long real_Pressure = ms5611.readPressure();
    // Calculate altitude
    float absolute_Altitude = ms5611.getAltitude(real_Pressure);
    float relative_Altitude = ms5611.getAltitude(real_Pressure, reference_Pressure);
    // read raw accel/gyro measurements from device
    // 'a' is acceleration
    // 'g' is rotation
    accelgyro.getMotion6(&gx, &gy, &gz, &ax, &ay, &az);

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

    Serial.print("ax: ");
    Serial.print(ax);
    Serial.print("\tay: ");
    Serial.print(ay);
    Serial.print("\taz: ");
    Serial.println(az);

    Serial.print("gx: ");
    Serial.print(gx);
    Serial.print("\tgy: ");
    Serial.print(gy);
    Serial.print("\tgz: ");
    Serial.println(gz);

    String data = String(millis()) + ", " + String(real_Temperature) + ", " + String(real_Pressure) + ", " + String(absolute_Altitude) + ", " + String(relative_Altitude) + ", " + String(ax) + ", " + String(ay) + ", " + String(az) + ", " + String(gx) + ", " + String(gy) + ", " + String(gz);
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