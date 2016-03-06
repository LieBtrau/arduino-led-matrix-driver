#include "seven_eighty_rg.h"
#include "SPI.h"

byte mem[7][20];// 7 rows, 80 bits = 10 bytes green, 80 bits = 10 bytes red

seven_eighty_rg::seven_eighty_rg(byte a, byte b, byte c, byte ss, boolean dbuf) :
    Apin(a), Bpin(b), Cpin(c), SSpin(ss),
    Adafruit_GFX(80, 7)
{
}

void seven_eighty_rg::begin(void)
{
    pinMode(SSpin, OUTPUT);
    pinMode(Apin, OUTPUT);
    pinMode(Bpin, OUTPUT);
    pinMode(Cpin, OUTPUT);
    SPI.begin();
    fillscreen(BLACK);
}

void seven_eighty_rg::drawPixel(int16_t x, int16_t y, uint16_t c)
{
  if ((x < 0) || (x >= _width) || (y < 0) || (y >= _width)) return;
  if (c & 1)
  {
    bitSet(mem[y][(x >> 3)<<1], x & 0x7);
  } else
  {
    bitClear(mem[y][(x >> 3)<<1], x & 0x7);
  }
  if (c & 2)
  {
    bitSet(mem[y][(x >> 2) | 1 ], x & 0x7);
  } else
  {
    bitClear(mem[y][(x >> 2) | 1], x & 0x7);
  }
}

void seven_eighty_rg::fillscreen(word c) {
  for (byte i = 0; i < 7; i++)
  {
    for (byte j = 0; j < 20; j++) {
      switch (c)
      {
    case ORANGE:
      mem[i][j] = 0x00;
      break;
    case RED:
      mem[i][j] = (j & 0x1 ? 0x00 : 0xFF);
      break;
    case GREEN:
      mem[i][j] = (j & 0x1 ? 0xFF : 0x00);
      break;
    case BLACK:
      mem[i][j] = 0xFF;
      break;
      }
    }
  }
}

void seven_eighty_rg::updateDisplay()
{
    ctr++;
    if (ctr > 6)
    {
      ctr = 0;
    }
    SPI.beginTransaction(SPISettings(1000000, LSBFIRST, SPI_MODE0));
    bitClear(PORTD,SSpin);
    for (byte j = 0; j < 7; j++) {
      for (byte i = 0; i < 20; i++) {
        SPI.transfer(mem[ctr][i]);
      }
    }
    bitSet(PORTD,SSpin);
    SPI.endTransaction();
    PORTD= (PORTD & 0x80) | ((ctr&7)<<4) | (PORTD & 0x0F);
}
