// EDIT TO LAUNCH SITE ALTITUDE (in meters)(Only required for absolute altitude readings)
const double launchAltitude = 0;

// EDIT DEPENDING ON WHAT STORAGE TYPE IS BEING USED (true or false)
const bool useInternalEEPROM = false; // Only records Max data due to memory limitations
const int internalEEPROMSize = 1024;  // Must provide internal eeprom size(328p has 1kb or 1024b)
const bool useExternalEEPROM = true;  // Records everything. will stop recording once end of memory is reached.
                                      // If smaller memory in use or longer flight time, increase loop delay(will measure data & write less often)
const int loopDelay = 500;

// START OF PROGRAM
#include <Arduino.h>
#include <Wire.h>
#include <MS5611.h>
MS5611 ms5611(0x77);
long initialPressure;
double initialTemperature;

#include <EEPROM.h>
bool EEPROMLock = false;

#include <I2C_eeprom.h>
I2C_eeprom i2cEEPROM(0x50, I2C_DEVICESIZE_24LC512); // 0x50 if all address pins pulled low(gnd)
uint32_t i2cEEPROMSize, i2cEEPROMLoc;

struct flightData
{
  unsigned long time;
  double temperature;
  long pressure;
  double altitude;
} flightData;

void setup()
{
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(115200); // PlatformIO default speed (still declared as such in plorformio.ini file)
  Serial.println("Program Starting...");

  if (useInternalEEPROM) // nothing really has to be done here
  {
    double oldMaxAlt;
    EEPROM.get(1, oldMaxAlt);

    Serial.print("previous flight data from EEPROM: ");
    Serial.print(oldMaxAlt, 3);
    Serial.println("m");
  }

  if (useExternalEEPROM)
  {
    i2cEEPROM.begin();
    if (!i2cEEPROM.isConnected())
    {
      Serial.println("ERROR: Can't find external eeprom...");
      Serial.println("Program Stopped");
      while (1); // loop forever
    }

    delay(10);

    i2cEEPROMSize = i2cEEPROM.determineSize(false);
    Serial.println("EXTERNAL EEPROM:");
    Serial.print("SIZE: ");
    Serial.print(i2cEEPROMSize / 1024);
    Serial.println(" KB.");
    Serial.print(i2cEEPROMSize);
    Serial.println(" bytes.");
  }

  // check if EEPROM is locked
  if (eepromLocked())
  {
    Serial.println("EEPROM do not write detected. Printing contents of EEPROM: ");
    EEPROMLogger(); // will never return from here.
  }
  else
  {
    // Lock eeprom and continue
    if (useExternalEEPROM)
      EEPROM.write(0, 1);
    if (useExternalEEPROM)
      i2cEEPROM.writeByte(0, 1);
  }
  
  Serial.println("Initializing MS5611 Sensor...");
  if (!ms5611.begin())
  {
    Serial.println("MS5611 connection failed");
    while (1); // loop forever
  }
  Serial.println("MS5611 connection successful");

  ms5611.setOversampling(OSR_ULTRA_HIGH);

  int result = ms5611.read();
  if (result != MS5611_READ_OK)
  {
    Serial.print("Error in read: ");
    Serial.println(result);
  }
  else
  {
    initialTemperature = ms5611.getTemperature();
    initialPressure = ms5611.getPressure() * 100;
  }
}

void loop()
{
  flightData = {millis(), 0, 0, 0}; // reset values, add millis
  double maxAlt = 0;

  digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(loopDelay);
  digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW

  Serial.println("");
  Serial.print(flightData.time);

  // DATA COLLECTION
  // Read true temperature & Pressure
  ms5611.read(OSR_ULTRA_HIGH);
  flightData.temperature = ms5611.getTemperature();
  flightData.pressure = ms5611.getPressure() * 100;

  // Calculate altitude
  flightData.altitude = calculateAltitude(initialPressure, initialTemperature, flightData.pressure, flightData.temperature);
  serialPrintMS5611();

  if (maxAlt < flightData.altitude) // Set Maximum Values
  {
    Serial.print(" - New Max Altitude reached");
    maxAlt = flightData.altitude;
    /**************** log flight data as max val? or just only maxAlt. Would simplify things. *********************/
  }

  if (useInternalEEPROM) // Only writes max height
  {
    if (EEPROMLock == false && flightData.time >= 120000 && abs(flightData.altitude) < 10) // if nothing has been written before && if after 120 seconds && if within 10 meters of starting height.
    {
      EEPROM.put(1, maxAlt);

      Serial.println("\n--- DATA HAS BEEN WRITTEN TO INTERNAL EEPROM ---");
      EEPROMLock = true; // stops arduino from writing again.
    }
  }

  if (useExternalEEPROM)
  {
    if (i2cEEPROMLoc < i2cEEPROMSize)
    {
      i2cEEPROM.writeBlock(i2cEEPROMLoc, (uint8_t *)&flightData, sizeof(flightData));
      i2cEEPROMLoc += sizeof(flightData);
      Serial.print(", external write: ");
      Serial.print(i2cEEPROMLoc);
    }
  }
}

double calculateAltitude(long lowerPressure, double lowerTemperature, long higherPressure, double higherTemperature) // pressure in pascals, temperature in celcius
{
  // Uses the hypsometric equation. Information here: https://glossary.ametsoc.org/wiki/Hypsometric_equation

  double p1 = lowerPressure;
  double p2 = higherPressure;
  double t1 = 273.15 + lowerTemperature;  // conversion to kelvin
  double t2 = 273.15 + higherTemperature; // conversion to kelvin

  double rv1 = (0.622 * EULER) / (p1 - EULER); // mixing ratio : https://glossary.ametsoc.org/wiki/Mixing_ratio
  double rv2 = (0.622 * EULER) / (p2 - EULER); // mixing ratio : https://glossary.ametsoc.org/wiki/Mixing_ratio

  double Tv1 = (1 + 0.61 * rv1) * t1; // approximate virtual temperature : https://glossary.ametsoc.org/wiki/Virtual_temperature
  double Tv2 = (1 + 0.61 * rv2) * t2; // approximate virtual temperature : https://glossary.ametsoc.org/wiki/Virtual_temperature
  double M_Tv = (Tv1 + Tv2) / 2;      // mean virtual temperature in kelvins [K] : https://glossary.ametsoc.org/wiki/Virtual_temperature

  double Rd = 287.058; // specific gas constant for dry air [J/(kg⋅K)]: https://en.wikipedia.org/wiki/Gas_constant
  double g = 9.80665;  // gravitational acceleration [m/s2] : https://en.wikipedia.org/wiki/Standard_gravity

  double h = (Rd * M_Tv / g) * log(p1 / p2); // hypsometric equation : https://glossary.ametsoc.org/wiki/Hypsometric_equation

  return h;
}

bool eepromLocked()
{
  bool lockByte = 0;
  if (useInternalEEPROM)
  {
    EEPROM.get(0, lockByte);

    if (lockByte == 1)
    {
      EEPROMLock = true;
    }
  }

  lockByte = 0;
  if (useExternalEEPROM)
  {
    lockByte = i2cEEPROM.readByte(0);

    if (lockByte == 1)
    {
      EEPROMLock = true;
    }
  }
  return EEPROMLock;
}

void EEPROMLogger()
{
  if (useInternalEEPROM)
  {
    double maxAlt;
    EEPROM.get(1, maxAlt);

    Serial.print("Max Alt logged on internal eeprom: ");
    Serial.print(maxAlt, 3);
  }

  if (useExternalEEPROM)
  {
    Serial.println("External EEPROM flight log: ");
    for (uint32_t i = 0; i < i2cEEPROMSize; i += sizeof(flightData))
    {
      flightData = {0, 0, 0, 0};
      i2cEEPROM.readBlock(i, (uint8_t *)&flightData, sizeof(flightData));
      delay(10);
      Serial.print(flightData.time);
      Serial.print(",");
      Serial.print(flightData.temperature);
      Serial.print(",");
      Serial.print(flightData.pressure);
      Serial.print(",");
      Serial.print(flightData.altitude);
      Serial.println("");
    }
    Serial.println("Done...");
  }

  int clearEEPROM = -1;
  do
  {
    Serial.println("Unlock EEPROM(s)?(0=no, 1=yes)");
    while (Serial.available() == 0)
      ;

    clearEEPROM = Serial.parseInt();

    if (clearEEPROM == 0)
    {
      return;
    }
    else if (clearEEPROM == 1)
    {
      //Unlock EEPROM
      if (useExternalEEPROM)
        EEPROM.write(0, 0);
      if (useExternalEEPROM)
        i2cEEPROM.writeByte(0, 0);

      Serial.println("EEPROM has been unlocked. Logging will resume on next startup.");
      Serial.println("Program exiting...");
      while (1); // loop forever
    }
    else
    {
      clearEEPROM = -1;
    }

  } while (clearEEPROM == -1);
}

void serialPrintMS5611()
{
  Serial.print(", Real Temperature: ");
  Serial.print(flightData.temperature);
  Serial.print(", Real Pressure: ");
  Serial.print(flightData.pressure);
  Serial.print(", Absolute Altitude: ");
  Serial.print(flightData.altitude);
}