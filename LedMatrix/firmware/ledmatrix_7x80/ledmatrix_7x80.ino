#include <SPI.h>

typedef enum
{
  ORANGE = 0,
  RED,
  GREEN,
  BLACK
} COLOR;

const byte SSpin = 7;
const byte Apin = 4;
const byte Bpin = 5;
const byte Cpin = 6;
//Dpin must be kept low at all times to enable LEDs

byte mem[7][20];// 7 rows, 80 bits = 10 bytes green, 80 bits = 10 bytes red

void setup() {
  Serial.begin(9600);
  // put your setup code here, to run once:
  pinMode(SSpin, OUTPUT);
  pinMode(Apin, OUTPUT);
  pinMode(Bpin, OUTPUT);
  pinMode(Cpin, OUTPUT);
  SPI.begin();
  fillscreen(BLACK);
  drawPixel(25,5,ORANGE);
}

byte ctr = 1;
byte _width = 80;
byte _height = 7;
//A B C Result
//0 0 0 Row 0 R+G
//1 0 0 Row 1 R+G
//0 1 0 Row 2 R+G
//1 1 0 Row 3 R+G
//0 0 1 Row 4 R+G
//1 0 1 Row 5 R+G
//0 1 1 Row 6 R+G

void drawPixel(word x, word y, word c)
{
  if ((x < 0) || (x >= _width) || (y < 0) || (y >= _height)) return;
  if (c & 1)
  {
    bitSet(mem[y][x >> 2], x & 0x7);
  } else {
    bitClear(mem[y][x >> 2], x & 0x7);
  }
  if (c & 2)
  {
    bitSet(mem[y][(x >> 2) + 1], x & 0x7);
  } else {
    bitClear(mem[y][(x >> 2) + 1], x & 0x7);
  }

}

void fillscreen(word c) {
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
void loop() {
  // put your main code here, to run repeatedly:
  if (Serial.available())
  {
    byte c = Serial.read();
  }
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
