//based on: https://github.com/adafruit/RGB-matrix-Panel
#include "seven_eighty_rg.h"
#include "SPI.h"

static seven_eighty_rg *activePanel = NULL;

seven_eighty_rg::seven_eighty_rg(byte a, byte b, byte c, byte ss, boolean dbuf) :
    Apin(a), Bpin(b), Cpin(c), SSpin(ss),
    Adafruit_GFX(80, 7)
{
    // Allocate and initialize matrix buffer:
    int allocsize = (dbuf == true) ? (VIDEOBUFFSIZE * 2) : VIDEOBUFFSIZE;
    if(NULL == (matrixbuff[0] = (uint8_t *)malloc(allocsize))) return;
    memset(matrixbuff[0], 0, allocsize);
    // If not double-buffered, both buffers then point to the same address:
    matrixbuff[1] = (dbuf == true) ? &matrixbuff[0][VIDEOBUFFSIZE] : matrixbuff[0];
}

void seven_eighty_rg::begin(void)
{
    backindex   = 0;                         // Back buffer
    buffptr     = matrixbuff[1 - backindex]; // -> front buffer

    pinMode(SSpin, OUTPUT);
    pinMode(Apin, OUTPUT);
    pinMode(Bpin, OUTPUT);
    pinMode(Cpin, OUTPUT);
    SPI.begin();
    activePanel = this;                      // For interrupt hander
    // Set up Timer1 for interrupt:
    TCCR1B  = _BV(WGM13) | _BV(WGM12) | _BV(CS10); // Mode 12, no prescale
    ICR1    = 10000; //freq  = 16MHz / (ICR1+1)
    TIMSK1 |= _BV(TOIE1); // Enable Timer1 interrupt
    sei();                // Enable global interrupts
    fillscreen(BLACK);
}

void seven_eighty_rg::drawPixel(int16_t x, int16_t y, uint16_t c)
{
    if ((x < 0) || (x >= _width) || (y < 0) || (y >= _width)) return;
    if (c & 1)
    {
        bitSet(matrixbuff[backindex][y*20 + ((x >> 3)<<1)], x & 0x7);
    } else
    {
        bitClear(matrixbuff[backindex][y*20 + ((x >> 3)<<1)], x & 0x7);
    }
    if (c & 2)
    {
        bitSet(matrixbuff[backindex][y*20 + ((x >> 2) | 1)], x & 0x7);
    } else
    {
        bitClear(matrixbuff[backindex][y*20 + ((x >> 2) | 1)], x & 0x7);
    }
}

void seven_eighty_rg::fillscreen(word c) {
    for (byte i = 0; i < 7; i++)
    {
        for (byte j = 0; j < 20; j++) {
            switch (c)
            {
            case ORANGE:
                matrixbuff[backindex][20*i+j] = 0x00;
                break;
            case RED:
                matrixbuff[backindex][20*i+j] = (j & 0x1 ? 0x00 : 0xFF);
                break;
            case GREEN:
                matrixbuff[backindex][20*i+j] = (j & 0x1 ? 0xFF : 0x00);
                break;
            case BLACK:
                matrixbuff[backindex][20*i+j] = 0xFF;
                break;
            }
        }
    }
}

void seven_eighty_rg::swapBuffers(boolean copy) {
    if(matrixbuff[0] != matrixbuff[1]) {
        // To avoid 'tearing' display, actual swap takes place in the interrupt
        // handler, at the end of a complete screen refresh cycle.
        swapflag = true;                  // Set flag here, then...
        while(swapflag == true) delay(1); // wait for interrupt to clear it
        if(copy == true)
            memcpy(matrixbuff[backindex], matrixbuff[1-backindex], VIDEOBUFFSIZE);
    }
}


void seven_eighty_rg::updateDisplay()
{
    ctr++;
    if (ctr > 6)
    {
        ctr = 0;
    }
    if(swapflag == true) {    // Swap front/back buffers if requested
        backindex = 1 - backindex;
        swapflag  = false;
    }
    buffptr = matrixbuff[1-backindex]+ctr*20;
    SPI.beginTransaction(SPISettings(1000000, LSBFIRST, SPI_MODE0));
    bitClear(PORTD,SSpin);
    for (byte i = 0; i < 20; i++) {
        SPI.transfer(buffptr[i]);
    }
    bitSet(PORTD,SSpin);
    SPI.endTransaction();
    //Enable display row
    PORTD= (PORTD & 0x80) | ((ctr&7)<<4) | (PORTD & 0x0F);
}

// -------------------- Interrupt handler stuff --------------------

ISR(TIMER1_OVF_vect, ISR_BLOCK) { // ISR_BLOCK important -- see notes later
    activePanel->updateDisplay();   // Call refresh func for active display
    TIFR1 |= TOV1;                  // Clear Timer1 interrupt flag
}

