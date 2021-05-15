# Arduino-Altimeter
A collection of simple Arduino scripts to record Altimeter data (among other things) using an Arduino. This was created with model rocketry in mind, especially after struggling to find specific information on creating compact and light Arduino Altimeters.

# Materials / Supplies

The materials you will need to create this depends on what you want to make. However, you will need a couple basic things.

- An [Arduino Nano](https://www.amazon.com/gp/product/B0713XK923/ref=ppx_yo_dt_b_search_asin_title?ie=UTF8&psc=1)
- A Sensor that can read barometric pressure(MS5611)
	- I recommend either a GY-63 or GY-86 sensor as they are the ones I have specifically written code for. However, any sensor with an MS5611 should work.
- (OPTIONAL) [Micro SD Card Reader](https://www.amazon.com/gp/product/B07BJ2P6X6/ref=ppx_yo_dt_b_search_asin_title?ie=UTF8&psc=1) and Micro SD Card
	- NOTE: any Micro SD Card will work; however, it must be [formatted for FAT16](https://aesmit.org/2020/05/24/format-sd-card-fat16-on-windows-10/)
- [A 7V battery](https://www.amazon.com/gp/product/B016ZM3CVA/ref=ppx_yo_dt_b_search_asin_title?ie=UTF8&psc=1)
- Hookup Wires
- Soldering iron (and knowledge on how to use one)

# Library Requirements

[How to install Libraries](https://www.arduino.cc/en/guide/libraries)

GY-63:
- Wire (Comes Preinstalled with Arduino)
- [Arduino-MS5611](https://github.com/jarzebski/Arduino-MS5611)

GY-86:
- Wire (Comes Preinstalled with Arduino)
- [I2CDev](https://github.com/jrowberg/i2cdevlib/tree/master/Arduino/I2Cdev)
- [MPU6050](https://github.com/jrowberg/i2cdevlib/tree/master/Arduino/MPU6050)

SD Card:
- SD (Comes Preinstalled with Arduino)
- SPI (Comes Preinstalled with Arduino)

# EEPROM vs Micro SD Card

EEPROM or Electrically Erasable Programmable Read-Only Memory, is the only permanent(meaning will be kept after power shutoff) memory in the Arduino. As the name states, its read only, howwver, it actually has 100,000 read/write cycles before it becomes unstable and unusable. For our intents and purposes, this is plenty, as we are only writing 2 per launch(one at the beggining to clear the memory, another at the end to write the highest Altitude). A downside however is that we only have 1KB(Arduino Nano; Varies from model to model) of memory, meaning we cant data log the whole flight, only the most important info. This is where the Micro SD Card comes in. It allows you to data log the entire flight(with 4GB+ of storage), however requring an entirely seperate unit, therefore adding very noticable weight to smaller rockets.(In larger rockets a couple grams dont matter as much). So which one to pick?

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
Once you have selected the file you want to use, you must make a small change to it before use(this excludes EEPROM_READ.ino). At the top of each file there is avariable called 'myRealAltitude'. You must edit the file to include your Altitude(in Meters) for accurate results. You can find this data easily using Google Earth or similar tools. For example, if your Altitude before launching is 100m above sealevel, that is what you put in.


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