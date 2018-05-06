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
const char str[] /*PROGMEM*/ = "# HOU AFSTAND # KEEP DISTANCE ";
typedef struct
{
    byte centerX;
    byte centerY;
    byte radius;
    byte maxRadius;
    seven_eighty_rg::COLOR color;
    unsigned long ultime;
}GROWING_CIRCLE;

GROWING_CIRCLE gc[4]={ {4,3,0,3,seven_eighty_rg::ORANGE,0}, {4,3,2,3,seven_eighty_rg::RED,0},
                       {75,3,0,3,seven_eighty_rg::ORANGE,0}, {75,3,2,3,seven_eighty_rg::RED,0} };

void setup() {
    Serial.begin(9600);
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
    infiniteScrollingText(str);
    for(byte i=0;i<4; i++)
    {
        growingCircles(&gc[i]);
    }
}

void growingCircles(GROWING_CIRCLE* gc)
{
    if(millis()>gc->ultime+100)
    {
        gc->ultime=millis();
        gc->radius = gc->radius > gc->maxRadius ? 0 : gc->radius+1;
        matrix.drawCircle(gc->centerX, gc->centerY, gc->radius, gc->color);
    }
}

void infiniteScrollingText(char* string)
{
    static unsigned long ulTime=0;
    static const byte CHAR_WIDTH=6;
    static const byte CURSORX_OFFSET=9;
    static int cursorX=CHAR_WIDTH-1;
    static int offset=0;
    int msgLen=strlen(string);
    if(millis()>ulTime+50)
    {
        ulTime=millis();
        matrix.fillScreen(seven_eighty_rg::BLACK);
        matrix.setCursor(CURSORX_OFFSET+cursorX,0);
        cursorX=(cursorX==0?CHAR_WIDTH-1:cursorX-1);
        matrix.print(string+offset);
        if(cursorX==CHAR_WIDTH-1)
        {
            offset=(offset==msgLen-1?0:offset+1);
        }
        matrix.print(string);
    }
}


