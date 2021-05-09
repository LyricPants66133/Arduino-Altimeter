# Arduino-Altimeter
A collection of simple Arduino scripts to record Altimeter data(among other things) using an Arduino. This was created with model rocketry in mind, especially after struggling to find specific information on creating compact and light Arduino Altimeters.

# Materials / Supplies

The materials you will need to create this depends on what you want to make. However, you will need a couple basic things.

- An [Arduino Nano](https://www.amazon.com/gp/product/B0713XK923/ref=ppx_yo_dt_b_search_asin_title?ie=UTF8&psc=1)
- A Sensor that can read barometric pressure(MS5611)
    - I recommend either a GY-63 or GY-86 sensor as they are the ones I have specifically written code for. However, any sensor with an MS5611 should work.
- (OPTIONAL) [Micro SD Card Reader](https://www.amazon.com/gp/product/B07BJ2P6X6/ref=ppx_yo_dt_b_search_asin_title?ie=UTF8&psc=1) and Micro SD Card
    - NOTE: any Micro SD Card will work; however, it must be [formatted for FAT16](https://aesmit.org/2020/05/24/format-sd-card-fat16-on-windows-10/)
- [A 7V battery](https://www.amazon.com/gp/product/B016ZM3CVA/ref=ppx_yo_dt_b_search_asin_title?ie=UTF8&psc=1)
- Hookup Wires
- Soldering iron(and knowledge on how to use one)

# Wiring

BATTERY:

    +       :   VIN
    -       :   GND

GY-63:

    VCC     :	5V
	GND	    :	GND
	SCL 	:	A5
	SDA	    :	A4
	CSB 	:	UNUSED
	SDO	    :	UNUSED
	PS  	:	UNUSED

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