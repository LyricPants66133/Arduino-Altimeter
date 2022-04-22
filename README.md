# Arduino-Altimeter

A customizable Arduino script to record Altimeter data (among other things) using an Arduino, a sensor, and an optional external EEPROM. This was created with model rocketry in mind, especially after struggling to find specific information on creating compact and light Arduino Altimeters.

# Materials / Supplies

The materials you will need to create this depends on what you want to make. However, you will need a couple basic things.

- An Arduino Nano or Pro Mini(better option, but harder for new users)
- A Sensor that can read barometric pressure(MS5611)
	- GY-63 and GY-86 are currentlty the only two tested sensors, however there should be no reason a sensor with an MS5611 wouldn't work.
- I2C EEPROM. FOr example a 24LC256 or 24LC512
- A 7V battery for Nano or 3v button battery for Pro Mini
- Hookup Wires
- Soldering iron (and knowledge on how to use one)
# Using the Code

## VSCode - PlatformIO (reccomended)

1. Download this Repo if you havent done so already
2. Open PlatformIO and press `Import Project`
3. Select your board type and then select the Repo folder
4. Open `src/Altimeter.ino` and change the settings at the top
5. Run the script like you normally would (All dependancies are automatically downloaded)

## Arduino IDE

1. Download this Repo if you havent done so already
2. Download all external dependancies, and place them in your library folder
	- [MS5611](https://github.com/RobTillaart/MS5611)
	- [I2C](https://github.com/RobTillaart/I2C_EEPROM)
3. Open `src/Altimeter.ino` in a new sketch and change the settings at the top
4. Run the sketch

# Wiring
```
BATTERY:

	+     :     VIN
	-     :     GND

GY-63:

	VCC     :     5V
	GND     :     GND
	SCL     :     A5 / SCL
	SDA     :     A4 / SDA
	CSB     :     UNUSED
	SDO     :     UNUSED
	PS      :     UNUSED

GY-86:

	VCC_IN  :     5V
	3.3V    :     UNUSED
	GND     :     GND
	SCL     :     A5
	SDA		:     A4
	FSYNC   :     UNUSED
	INTA    :     2
	DRDY    :     UNUSED
```
