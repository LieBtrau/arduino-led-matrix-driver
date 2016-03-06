#include <Arduino.h>
#include <SPI.h>
#include "seven_eighty_rg.h"

byte x=0;
//Dpin must be kept low at all times to enable LEDs
seven_eighty_rg ledMatrix(4,5,6,7, false);
unsigned long starttime;

void setup() {
    // put your setup code here, to run once:
  Serial.begin(9600);
  ledMatrix.begin();
}


void loop() {
  // put your main code here, to run repeatedly:
  if(millis()>starttime+1000)
  {
      starttime=millis();
      ledMatrix.drawPixel(x++,3,ledMatrix.ORANGE);
      if(x==80)
      {
          x=0;
      }
  }
  ledMatrix.updateDisplay();
}

