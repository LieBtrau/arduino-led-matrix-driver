# arduino-led-matrix-driver
Arduino driver for 7x80 Red/Green LED matrix.  The code has been developed for an M500-7X80RG, version 4.3 PCB.  I guess it's compatible for comparable 7X80 Red/Green matrices.

# Connections to Arduino Uno
## Signal connections
The LED matrix is interfaced through the SPI-bus.
You'll have to connect the following:   

| Signal name | LED matrix          | Arduino Uno  |
|-------------|---------------------|--------------|
| GND         | JP2.19 (next to IR) | ICSP.6 / GND |
| MOSI        | JP2.14 (DATA)       | ICSP.4 / 11  |
| SCLK        | JP2.12 (CLK)        | ICSP.3 / 13  |
| /CS         | JP2.10 (STB)        | any GPIO     |
| D           | JP2.8  (D)          | GND          |
| C           | JP2.6  (C)          | any GPIO     |
| B           | JP2.4  (B)          | any GPIO     |
| A           | JP2.2  (A)          | any GPIO     |

## Power connections
When all LEDs are on, the LED matrix draws about 4A.  Your Arduino can't supply that.
Use 2.5mm plug P1 on the LED matrix to supply it with 5V.

# Firmware
This library is heavily based on the [Adafruit RGB matrix panel driver](https://github.com/adafruit/RGB-matrix-Panel).  The driver implements the low level functionality for the [Adafruit GFX library](https://github.com/adafruit/Adafruit-GFX-Library), which allows you to draw graphical objects such as lines, circles, rectangles, ... and of course text.
