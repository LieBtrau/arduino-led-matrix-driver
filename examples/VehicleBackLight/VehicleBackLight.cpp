#include "seven_eighty_rg.h"
#include "sprites.h"
#include "Bounce2.h"

// Similar to F(), but for PROGMEM string pointers rather than literals
#define F2(progmem_ptr) (const __FlashStringHelper *)progmem_ptr

typedef struct
{
    byte centerX;
    byte centerY;
    byte radius;
    byte maxRadius;
    seven_eighty_rg::COLOR color;
    unsigned long ultime;
}GROWING_CIRCLE;

void initAnalogComparator();
void growingCircles(GROWING_CIRCLE* gc);
void infiniteScrollingText(const char* string, byte cursorXoffset,byte scrollCharLength);

GROWING_CIRCLE gc[4]={ {4,3,0,3,seven_eighty_rg::ORANGE,0}, {4,3,2,3,seven_eighty_rg::RED,0},
                       {75,3,0,3,seven_eighty_rg::ORANGE,0}, {75,3,2,3,seven_eighty_rg::RED,0} };
seven_eighty_rg matrix(A3,A2,A1,10, false);
const char str[] /*PROGMEM*/ = "# BABY ON BOARD ";
const byte SWITCH_PIN = 8;
const byte NFET_PIN = 6;
const byte LED_PIN = 5;
Bounce debouncer = Bounce();
static byte state = 0;

void setup()
{
    pinMode(SWITCH_PIN, INPUT_PULLUP);
    debouncer.attach(SWITCH_PIN);
    debouncer.interval(50); // interval in ms
    pinMode(NFET_PIN, OUTPUT);
    digitalWrite(NFET_PIN, HIGH);
    pinMode(LED_PIN, OUTPUT);
    analogReference(INTERNAL);
    Serial.begin(9600);
    //initAnalogComparator();
    matrix.begin();
    matrix.setTextWrap(false); // Allow text to run off right edge
    matrix.setTextSize(1);
    matrix.setTextColor(matrix.ORANGE);
}

void loop()
{
    debouncer.update();
    switch (state)
    {
    case 0:
        if (debouncer.read())
        {
            state = 1;
        }
        break;
    case 1:
        if (!debouncer.read())
        {
            digitalWrite(NFET_PIN, LOW);
        }
        break;
    }
   infiniteScrollingText(str, 11, 9);
   for(byte i=0;i<4; i++)
   {
       growingCircles(&gc[i]);
   }
//    showPacman();
    // if(!bitRead(ACSR, ACO))         //  check status of comparator output flag
    // {
    //     //Voltage too low!  Start beeping
    // }
    digitalWrite(LED_PIN, analogRead(A0) < 100 ? HIGH : LOW);
}

void showPacman()
{
    static unsigned long ulTimer=0;
    static byte state=0;
    static byte x=0;
    if(millis()>ulTimer+75)
    {
        ulTimer=millis();
        state=(state==2?0:state+1);
        matrix.fillRect(x,0,8,7, seven_eighty_rg::BLACK);
        x=(x==90?-10:x+1);
        matrix.drawBitmap(x,0, monster[state], 8, 7, seven_eighty_rg::GREEN);
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

void infiniteScrollingText(const char* string, byte cursorXoffset,byte scrollCharLength)
{
    static unsigned long ulTime=0;
    static const byte CHAR_WIDTH=6;
    static int cursorX=CHAR_WIDTH-1;
    static int offset=0;
    int msgLen=strlen(string);
    char strShow[msgLen];
    if(millis()>ulTime+30)
    {
        ulTime=millis();
        matrix.fillRect(9,0, 62,7,seven_eighty_rg::BLACK);
        matrix.setCursor(cursorXoffset+cursorX,0);
        strncpy(strShow, string+offset, msgLen);
        strncpy(strShow+msgLen-offset, string, offset);
        strShow[scrollCharLength>msgLen-1?msgLen-1:scrollCharLength]='\0';
        matrix.print(strShow);
        cursorX=(cursorX==0?CHAR_WIDTH-1:cursorX-1);
        if(cursorX==CHAR_WIDTH-1)
        {
            offset=(offset==msgLen-1?0:offset+1);
        }
    }
}

//https://forum.arduino.cc/index.php?topic=149840.0
void initAnalogComparator()
{
    bitClear(ADCSRB, ACME);     // AIN1 is applied to the negative input of the Analog Comparator
    bitSet(ACSR, ACI);          // Analog Comparator Interrupt Flag: Clear Pending Interrupt
    bitClear(ACSR, ACD);        // Analog Comparator: Enabled
    bitSet(ACSR, ACBG);         // Analog Comparator Bandgap Select: 1.1Vref is applied to the positive input of the Analog Comparator
    bitClear(ACSR, ACIE);       // Analog Comparator Interrupt: Disabled
    bitClear(ACSR, ACIC);       // Analog Comparator Input Capture: Disabled
}