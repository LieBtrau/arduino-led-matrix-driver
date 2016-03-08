//Based on: https://github.com/adafruit/RGB-matrix-Panel

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
    seven_eighty_rg(byte a, byte b, byte c, byte ss, boolean dbuf);
    void begin();
    void drawPixel(int16_t x, int16_t y, uint16_t c);
    void fillscreen(word c);
    void updateDisplay(void);
    void swapBuffers(boolean);
    byte* *backBuffer(void);
private:
    static const byte VIDEOBUFFSIZE=140;
    static const byte BYTESPERROW=20;
    static const byte ROWCOUNT=7;
    byte Apin,Bpin,Cpin, SSpin;
    byte* matrixbuff[2];
    volatile byte backindex;
    volatile boolean swapflag;
    volatile uint8_t *buffptr;
    byte row;
    volatile byte
      *addraport, *addrbport, *addrcport;
    uint8_t
      addrapin, addrbpin, addrcpin;

};

#endif // SEVEN_EIGHTY_RG_H
