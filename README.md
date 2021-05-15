# Arduino-Altimeter

A customizable Arduino script to record Altimeter data (among other things) using an Arduino, a sensor, and an optional SD card. This was created with model rocketry in mind, especially after struggling to find specific information on creating compact and light Arduino Altimeters.

# Materials / Supplies

The materials you will need to create this depends on what you want to make. However, you will need a couple basic things.

- An [Arduino Nano](https://www.amazon.com/gp/product/B0713XK923/ref=ppx_yo_dt_b_search_asin_title?ie=UTF8&psc=1)
- A Sensor that can read barometric pressure(MS5611)
	- GY-63 and GY-86 are currentlty the only two tested sensors, however there should be no reason a sensor with an MS5611 wouldn't work.
- (OPTIONAL) [Micro SD Card Reader](https://www.amazon.com/gp/product/B07BJ2P6X6/ref=ppx_yo_dt_b_search_asin_title?ie=UTF8&psc=1) and Micro SD Card
	- NOTE: any Micro SD Card will work; however, it must be [formatted for FAT16](https://aesmit.org/2020/05/24/format-sd-card-fat16-on-windows-10/)
- [A 7V battery](https://www.amazon.com/gp/product/B016ZM3CVA/ref=ppx_yo_dt_b_search_asin_title?ie=UTF8&psc=1) to power to Arduino
- Hookup Wires
- Soldering iron (and knowledge on how to use one)

# EEPROM vs Micro SD Card

EEPROM or Electrically Erasable Programmable Read-Only Memory is the only permanent (i.e., not erased after power shutoff) memory in the Arduino. As the name states, it's read-only; however, it actually has 100,000 read/write cycles before it becomes unstable and unusable. For our intents and purposes, this is plenty, as we are only writing 2 per launch(one at the beginning to clear the memory, another at the end to write the highest Altitude). However, a downside is that we only have 1KB of memory(Arduino Nano; Varies from model to model). This means we cant data log the whole flight, only the most important info. This is where the Micro SD Card comes in. It allows you to datalog the entire flight(with 4GB+ of storage), however requiring an entirely separate unit, therefore adding very noticeable weight to smaller rockets. (In larger rockets, a couple of grams don't matter as much). So which one to pick?

EEPROM:
- PROS:
	- Built-in; no need to buy additional parts
	- Lighter
	- More than enough for most people
- CONS: 
	- Small storage
	- limited amounts of Read-Write Cycles

Micro SD Card
- PROS:
	- Large amounts of memory (allows for full-flight data-logging)
	- no limit to Read-Write cycles
	- Easier to read data from (popular and standard storage device)
- CONS: 
	- WEIGHT. For smaller rockets, the extra grams can be massively detrimental
	- Another point of failure. If something happens to the wiring, you wont record anything for that flight.
	- Be honest, besides looking cool in a graph, you most likely dont need the extra data.
	- Cash. They cost money to buy.

# Using the Code

## PlatformIO (reccomended)

1. Download this Repo if you havent done so already
2. Open PlatformIO and press `Import Project`
3. Select your board type and then select the Repo folder
4. Open `src/Altimeter.ino` and change the settings at the top
5. Run the script like you normally would (All dependancies are automatically downloaded)

## Arduino IDE

1. Download this Repo if you havent done so already
2. Download all external dependancies, and place them in your library folder
	- [MS5611](https://github.com/jarzebski/Arduino-MS5611)
	- [I2Cdev-Core](https://github.com/jrowberg/i2cdevlib/tree/master/Arduino/I2Cdev)
	- [MPU6050](https://github.com/jrowberg/i2cdevlib/tree/master/Arduino/MPU6050)
3. Open `src/Altimeter.ino` in a new sketch and change the settings at the top
4. Run the sketch

# Wiring

BATTERY:

	+		:	VIN
	-		:	GND

GY-63:

	VCC		:	5V
	GND		:	GND
	SCL		:	A5
	SDA		:	A4
	CSB		:	UNUSED
	SDO		:	UNUSED
	PS		:	UNUSED

GY-86:

	VCC_IN	:	5V
	3.3V	:	UNUSED
	GND		:	GND
	SCL		:	A5
	SDA		:	A4
	FSYNC	:	UNUSED
	INTA	:	2
	DRDY	:	UNUSED

SD READER:

	GND		:	GND
	VCC		:	5V
	MISO	:	12
	MOSI	:	11
	SCK		:	13
	CS		:	4