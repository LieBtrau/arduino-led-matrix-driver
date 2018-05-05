#include <SPI.h>
#include "seven_eighty_rg.h"
#include "Adafruit_GFX.h"
#include "Fonts/TomThumb.h"
#include "Fonts/Tiny3x3a2pt7b.h"
#include "Fonts/Picopixel.h"

//git clone git@github.com:adafruit/Adafruit-GFX-Library.git
//ln -s ~/git/Adafruit-GFX-Library/ ~/Arduino/libraries

// Similar to F(), but for PROGMEM string pointers rather than literals
#define F2(progmem_ptr) (const __FlashStringHelper *)progmem_ptr

seven_eighty_rg matrix(4,5,6,10, false);
const char str[] PROGMEM = "Adafruit 16x32 RGB LED Matrix";
typedef struct
{
    byte centerX;
    byte centerY;
    byte radius;
    byte maxRadius;
    seven_eighty_rg::COLOR color;
    unsigned long ultime;
}GROWING_CIRCLE;

GROWING_CIRCLE gc1={4,3,0,3,seven_eighty_rg::ORANGE,0};
GROWING_CIRCLE gc2={4,3,2,3,seven_eighty_rg::RED,0};
GROWING_CIRCLE gc3={75,3,0,3,seven_eighty_rg::ORANGE,0};
GROWING_CIRCLE gc4={75,3,2,3,seven_eighty_rg::RED,0};

void setup() {
    matrix.begin();
    matrix.setTextWrap(false); // Allow text to run off right edge
    matrix.setTextSize(1);
    matrix.setTextColor(matrix.ORANGE);
//    matrix.drawLine(0,0,79,6,matrix.RED);
    //matrix.setFont(&TomThumb);
    //matrix.setFont(&Tiny3x3a2pt7b);
//    matrix.setFont(&Picopixel);
//    matrix.drawChar(0, 7, 'a',matrix.ORANGE, matrix.BLACK, 1);
}

void loop()
{
    growingCircles(&gc1);
    growingCircles(&gc2);
    growingCircles(&gc3);
    growingCircles(&gc4);
}

void growingCircles(GROWING_CIRCLE* gc)
{
    if(millis()>gc->ultime+100)
    {
        gc->ultime=millis();
        matrix.drawCircle(gc->centerX, gc->centerY, gc->radius, seven_eighty_rg::BLACK);
        gc->radius = gc->radius > gc->maxRadius ? 0 : gc->radius+1;
        matrix.drawCircle(gc->centerX, gc->centerY, gc->radius, gc->color);
    }
}


