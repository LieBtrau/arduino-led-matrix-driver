#include <Arduino.h>
#include <SPI.h>
#include "seven_eighty_rg.h"

// Similar to F(), but for PROGMEM string pointers rather than literals
#define F2(progmem_ptr) (const __FlashStringHelper *)progmem_ptr

seven_eighty_rg matrix(4,5,6,7, false);
const char str[] PROGMEM = "Adafruit 16x32 RGB LED Matrix";

//Dpin must be kept low at all times to enable LEDs

void setup() {
    // put your setup code here, to run once:
    Serial.begin(9600);
    Serial.println("Started");
    matrix.begin();
    matrix.setTextWrap(false); // Allow text to run off right edge
    matrix.setTextSize(1);
    matrix.setTextColor(matrix.ORANGE);
    matrix.drawLine(0,0,79,6,matrix.RED);
    matrix.drawCircle(40, 3, 3, matrix.GREEN);
    matrix.setFont();
    matrix.drawChar(0, 0, 'a',matrix.ORANGE, matrix.BLACK, 1);
}


void loop() {

    // Clear background
    //matrix.fillScreen(0);

    //matrix.setCursor(0, 0);
    //matrix.print(F2(str));

    // Update display
    matrix.updateDisplay();
}

