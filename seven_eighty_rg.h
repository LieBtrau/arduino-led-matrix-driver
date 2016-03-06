#ifndef SEVEN_EIGHTY_RG_H
#define SEVEN_EIGHTY_RG_H
//#if ARDUINO >= 100
 #include "Arduino.h"
//#else
// #include "WProgram.h"
// #include "pins_arduino.h"
//#endif
#include "Adafruit_GFX.h"

class seven_eighty_rg: public Adafruit_GFX
{
public:
    typedef enum
    {
      ORANGE = 0,
      RED,
      GREEN,
      BLACK
    } COLOR;
    const byte WIDTH=80;
    const byte HEIGHT=7;
    seven_eighty_rg(byte a, byte b, byte c, byte ss, boolean dbuf);
    void begin();
    void drawPixel(int16_t x, int16_t y, uint16_t c);
    void fillscreen(word c);
    void updateDisplay(void);
    void swapBuffers(boolean);
    byte* *backBuffer(void);
private:
    byte Apin,Bpin,Cpin, SSpin;
    byte* matrixbuff[2];
    volatile byte backindex;
    volatile boolean swapflag;
    byte ctr;
};

#endif // SEVEN_EIGHTY_RG_H
