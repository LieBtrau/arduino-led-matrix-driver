//based on: https://github.com/adafruit/RGB-matrix-Panel
#include "seven_eighty_rg.h"
#include "SPI.h"

static seven_eighty_rg *activePanel = NULL;
static void update();

seven_eighty_rg::seven_eighty_rg(byte a, byte b, byte c, byte ss, boolean dbuf) :
    Adafruit_GFX(80, ROWCOUNT), Apin(a), Bpin(b), Cpin(c), SSpin(ss)
{
    // Allocate and initialize matrix buffer:
    int allocsize = (dbuf == true) ? (VIDEOBUFFSIZE * 2) : VIDEOBUFFSIZE;
    if(NULL == (matrixbuff[0] = (uint8_t *)malloc(allocsize))) return;
    memset(matrixbuff[0], 0, allocsize);
    // If not double-buffered, both buffers then point to the same address:
    matrixbuff[1] = (dbuf == true) ? &matrixbuff[0][VIDEOBUFFSIZE] : matrixbuff[0];
    addraport = portOutputRegister(digitalPinToPort(a));
    addrapin  = digitalPinToBitMask(a);
    addrbport = portOutputRegister(digitalPinToPort(b));
    addrbpin  = digitalPinToBitMask(b);
    addrcport = portOutputRegister(digitalPinToPort(c));
    addrcpin  = digitalPinToBitMask(c);
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
#ifdef ARDUINO_ARCH_AVR
    // Set up Timer1 for interrupt:
    TCCR1A  = _BV(WGM11); // Mode 14 (fast PWM), OC1A off
    TCCR1B  = _BV(WGM13) | _BV(WGM12) | _BV(CS10); // Mode 14, no prescale
    ICR1    = 10000;
    TIMSK1 |= _BV(TOIE1); // Enable Timer1 interrupt
    sei();                // Enable global interrupts
#elif ARDUINO_ARCH_STM32F1
    Timer2.setChannel1Mode(TIMER_OUTPUTCOMPARE);
    Timer2.setPeriod(100); // in microseconds
    Timer2.setCompare(TIMER_CH1, 1);      // overflow might be small
    Timer2.attachInterrupt(TIMER_CH1, update);
#endif
    fillscreen(BLACK);
}

void seven_eighty_rg::drawPixel(int16_t x, int16_t y, uint16_t c)
{
    if ((x < 0) || (x >= _width) || (y < 0) || (y >= _width)) return;
    if (c & 1)
    {
	bitSet(matrixbuff[backindex][y*BYTESPERROW + ((x >> 3)<<1)], x & 0x7);
    } else
    {
	bitClear(matrixbuff[backindex][y*BYTESPERROW + ((x >> 3)<<1)], x & 0x7);
    }
    if (c & 2)
    {
	bitSet(matrixbuff[backindex][y*BYTESPERROW + ((x >> 2) | 1)], x & 0x7);
    } else
    {
	bitClear(matrixbuff[backindex][y*BYTESPERROW + ((x >> 2) | 1)], x & 0x7);
    }
}

void seven_eighty_rg::fillscreen(word c) {
    for (byte i = 0; i < VIDEOBUFFSIZE; i++)
    {
	switch (c)
	{
	case ORANGE:
	    matrixbuff[backindex][i] = 0x00;
	    break;
	case RED:
	    matrixbuff[backindex][i] = (i & 0x1 ? 0x00 : 0xFF);
	    break;
	case GREEN:
	    matrixbuff[backindex][i] = (i & 0x1 ? 0xFF : 0x00);
	    break;
	case BLACK:
	    matrixbuff[backindex][i] = 0xFF;
	    break;
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
    row = row < ROWCOUNT-1 ? row+1 : 0;
    if(swapflag == true)
    {    // Swap front/back buffers if requested
	backindex = 1 - backindex;
	swapflag  = false;
    }
    if(row<8)
    {
	buffptr = matrixbuff[1-backindex]+row*BYTESPERROW;
	SPI.beginTransaction(SPISettings(1000000, LSBFIRST, SPI_MODE0));
	digitalWrite(SSpin, LOW);
	for (byte i = 0; i < BYTESPERROW; i++)
	{
	    SPI.transfer(buffptr[i]);
	}
	digitalWrite(SSpin, HIGH);
	SPI.endTransaction();
	//Enable display row
#ifdef ARDUINO_ARCH_AVR
	if(row & 0x1)   *addraport |=  addrapin;
	else            *addraport &= ~addrapin;
	if(row & 0x2)   *addrbport |=  addrbpin;
	else            *addrbport &= ~addrbpin;
	if(row & 0x4)   *addrcport |=  addrcpin;
	else            *addrcport &= ~addrcpin;
	if(row & 0x4)digitalWrite(Cpin, HIGH);
	else            digitalWrite(Cpin, LOW);
#elif ARDUINO_ARCH_STM32F1
	digitalWrite(Apin, row & 0x1 ? HIGH : LOW);
	digitalWrite(Bpin, row & 0x2 ? HIGH : LOW);
	digitalWrite(Cpin, row & 0x4 ? HIGH : LOW);
#endif
    }
}

// -------------------- Interrupt handler stuff --------------------

#ifdef ARDUINO_ARCH_AVR
ISR(TIMER1_OVF_vect, ISR_BLOCK) { // ISR_BLOCK important -- see notes later
    TIFR1 |= TOV1;                  // Clear Timer1 interrupt flag
#elif ARDUINO_ARCH_STM32F1
void update(){
#endif
    activePanel->updateDisplay();   // Call refresh func for active display
}

