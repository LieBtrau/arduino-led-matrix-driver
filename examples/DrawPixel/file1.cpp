#include <Arduino.h>
#include <SPI.h>
#include "seven_eighty_rg.h"

// Similar to F(), but for PROGMEM string pointers rather than literals
#define F2(progmem_ptr) (const __FlashStringHelper *)progmem_ptr

seven_eighty_rg matrix(4,5,6,7, false);
const char str[] PROGMEM = "Adafruit 16x32 RGB LED Matrix";



void setup() {
    matrix.begin();
    matrix.setTextWrap(false); // Allow text to run off right edge
    matrix.setTextSize(1);
    matrix.setTextColor(matrix.ORANGE);
    matrix.drawLine(0,0,79,6,matrix.RED);
    matrix.drawCircle(40, 3, 3, matrix.GREEN);
    matrix.setFont();
    matrix.drawChar(0, 0, 'a',matrix.ORANGE, matrix.BLACK, 1);
}

void rotatingBar()
{
    static byte x1, y1, state;
    static unsigned long timer=0;

    if(millis()>timer+2)
    {
        matrix.fillscreen(matrix.BLACK);
        matrix.drawLine(x1,y1,matrix.width()-x1-1,matrix.height()-y1-1,state ? matrix.RED : matrix.ORANGE);
        switch(state)
        {
        case 0:
            x1++;
            if(x1>=matrix.width())
            {
                x1=matrix.width()-1;
                state=1;
            }
            break;
        case 1:
            y1++;
            if(y1>=matrix.height())
            {
                y1=0;
                x1=0;
                state=0;
            }
            break;
        default:
            state=0;
            break;
        }
        timer=millis();
    }
}

void counter()
{
    static unsigned long ultime=millis();
    static byte ctr=0;
    if(millis()>ultime+500)
    {
        ultime=millis();
        matrix.drawChar(6,0,'0'+ctr,matrix.ORANGE, matrix.BLACK,1);
        ctr=ctr<9 ? ctr+1 : 0;
    }
}

void loop() {
    counter();
    //rotatingBar();
    // Clear background
    //matrix.fillScreen(0);

    //matrix.setCursor(0, 0);
    //matrix.print(F2(str));

}

