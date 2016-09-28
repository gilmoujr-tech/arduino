// This sketch has been Refurbished by BUHOSOFT
// IMPORTANT: Adafruit_TFTLCD LIBRARY MUST BE SPECIFICALLY
// CONFIGURED FOR EITHER THE TFT SHIELD OR THE BREAKOUT BOARD.
// SEE RELEVANT COMMENTS IN Adafruit_TFTLCD.h FOR SETUP.
#define DEBUG
#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_TFTLCD.h> // Hardware-specific library
#include <stdint.h>
#include "TouchScreen.h"

// The control pins for the LCD can be assigned to any digital or
// analog pins...but we'll use the analog pins as this allows us to
// double up the pins with the touch screen (see the TFT paint example).
#define LCD_CS A3 // Chip Select goes to Analog 3
#define LCD_CD A2 // Command/Data goes to Analog 2
#define LCD_WR A1 // LCD Write goes to Analog 1
#define LCD_RD A0 // LCD Read goes to Analog 0
#define LCD_RESET A4 // Can alternately just connect to Arduino's reset pin

// When using the BREAKOUT BOARD only, use these 8 data lines to the LCD:
// For the Arduino Uno, Duemilanove, Diecimila, etc.:
//   D0 connects to digital pin 8  (Notice these are
//   D1 connects to digital pin 9   NOT in order!)
//   D2 connects to digital pin 2
//   D3 connects to digital pin 3
//   D4 connects to digital pin 4
//   D5 connects to digital pin 5
//   D6 connects to digital pin 6
//   D7 connects to digital pin 7
// For the Arduino Mega, use digital pins 22 through 29
// (on the 2-row header at the end of the board).

// Assign human-readable names to some common 16-bit color values:
#define	BLACK   0x0000
#define	BLUE    0x001F
#define	RED     0xF800
#define	GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF

Adafruit_TFTLCD tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET);
// If using the shield, all control and data lines are fixed, and
// a simpler declaration can optionally be used:
// Adafruit_TFTLCD tft;

// These are the pins for the shield!
//Original 
//#define YP A1  // must be an analog pin, use "An" notation!
//#define XM A2  // must be an analog pin, use "An" notation!
//#define YM 7   // can be a digital pin
//#define XP 6   // can be a digital pin
//Edited
#define YP A3  // must be an analog pin, use "An" notation!
#define XM A2  // must be an analog pin, use "An" notation!
#define YM 9   // can be a digital pin
#define XP 8   // can be a digital pin

#define MINPRESSURE 100
#define MAXPRESSURE 900

#define BOXSIZE 40
#define PENRADIUS 3
int oldcolor, currentcolor;
int screenRotation = 1; // allows automatic screen rotation [values: 1-4]
int touchPoint = 0;   //allows screen rotation to pinpoint color selection based on x or y axis

#define TS_MINX 140
#define TS_MINY 140
#define TS_MAXX 900
#define TS_MAXY 950
// For better pressure precision, we need to know the resistance
// between X+ and X- Use any multimeter to read it
// For the one we're using, its 300 ohms across the X plate

TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);

void setup(void) {
  Serial.begin(9600);
  Serial.println(F("TFT LCD test"));
  Serial.print("TFT size is "); Serial.print(tft.width()); Serial.print("x"); Serial.println(tft.height());

  tft.reset();

  uint16_t identifier = tft.readID();

#ifdef DEBUG
if(identifier == 0x9325) {
    Serial.println(F("Found ILI9325 LCD driver"));
  } else if(identifier == 0x9328) {
    Serial.println(F("Found ILI9328 LCD driver"));
  } else if(identifier == 0x7575) {
    Serial.println(F("Found HX8347G LCD driver"));
  } else if(identifier == 0x9341) {
    Serial.println(F("Found ILI9341 LCD driver"));
  } else if(identifier == 0x8357) {
    Serial.println(F("Found HX8357D LCD driver"));
  } else {
    Serial.print(F("Unknown LCD driver chip: "));
    Serial.println(identifier, HEX);
    Serial.println(F("If using the Adafruit 2.8\" TFT Arduino shield, the line:"));
    Serial.println(F("  #define USE_ADAFRUIT_SHIELD_PINOUT"));
    Serial.println(F("should appear in the library header (Adafruit_TFT.h)."));
    Serial.println(F("If using the breakout board, it should NOT be #defined!"));
    Serial.println(F("Also if using the breakout, double-check that all wiring"));
    Serial.println(F("matches the tutorial."));
    //return;
    identifier = 0x9341;
  }
#endif

  tft.begin(identifier);

  tft.setRotation(screenRotation);

#ifdef DEBUG
  Serial.println("Starting TFT Display...");
  tft.fillScreen(BLACK);
  unsigned long start = micros();
  tft.setCursor(0, 0);
  tft.setTextColor(WHITE);  
  tft.setTextSize(2);
  tft.println(F("TFT LCD test"));
  tft.print("TFT size is "); tft.print(tft.width()); tft.print("x"); tft.println(tft.height());  
  tft.print("Using Rotation: "); tft.println(screenRotation); 
  delay(3000);
#endif
  
  tft.fillScreen(BLACK);
  tft.setCursor(0, 0);
  tft.setTextColor(WHITE); 
  tft.setTextSize(2);
  Serial.println("Starting Paint Program...");

  tft.println("Starting Paint...");
  delay(3000);

  tft.fillScreen(BLACK);
  
  // make the color selection boxes
  tft.fillRect(0, 0, BOXSIZE, BOXSIZE, RED);
  tft.fillRect(BOXSIZE, 0, BOXSIZE, BOXSIZE, YELLOW);
  tft.fillRect(BOXSIZE*2, 0, BOXSIZE, BOXSIZE, GREEN);
  tft.fillRect(BOXSIZE*3, 0, BOXSIZE, BOXSIZE, CYAN);
  tft.fillRect(BOXSIZE*4, 0, BOXSIZE, BOXSIZE, BLUE);
  tft.fillRect(BOXSIZE*5, 0, BOXSIZE, BOXSIZE, MAGENTA);
 
  // select the current color 'red'
  tft.drawRect(0, 0, BOXSIZE, BOXSIZE, WHITE);
  currentcolor = RED;

}

void loop(void) {
  // Recently Point was renamed TSPoint in the TouchScreen library
  // If you are using an older version of the library, use the
  // commented definition instead.
  // Point p = ts.getPoint();
  int x,y;
  
  TSPoint p = ts.getPoint();
    // if sharing pins, you'll need to fix the directions of the touchscreen pins
  //pinMode(XP, OUTPUT);
  pinMode(XM, OUTPUT);
  pinMode(YP, OUTPUT);
  //pinMode(YM, OUTPUT);

  // we have some minimum pressure we consider 'valid'
  // pressure of 0 means no press

  if (p.z > MINPRESSURE && p.z < MAXPRESSURE)
  {
  #ifdef DEBUG  
    Serial.print("X = "); Serial.print(p.x);
    Serial.print("\tY = "); Serial.print(p.y);
    Serial.print("\tPressure = "); Serial.println(p.z);
  #endif      

    // scale from 0->1023 to tft.width

  if (screenRotation==1)
  {
    p.x = tft.height()-(map(p.x, TS_MAXX, TS_MINX, tft.height(), 0));
    p.y = tft.width()-(map(p.y, TS_MAXY, TS_MINY, tft.width(), 0));
  }
  if (screenRotation==3)
  {
    p.x = tft.height()-(map(p.x, TS_MINX, TS_MAXX, tft.height(), 0));
    p.y = tft.width()-(map(p.y, TS_MINY, TS_MAXY, tft.width(), 0));
  }
  if (screenRotation==2)
  {
    p.x = tft.width()-(map(p.x, TS_MAXX, TS_MINX, tft.width(), 0));
    p.y = tft.height()-(map(p.y, TS_MINY, TS_MAXY, tft.height(), 0));
  }
  if (screenRotation==4)
  {
    p.x = tft.width()-(map(p.x, TS_MINX, TS_MAXX, tft.width(), 0));
    p.y = tft.height()-(map(p.y, TS_MAXY, TS_MINY, tft.height(), 0));
  }

  x=(4*p.x)/tft.width();
  y=(4*p.y)/tft.height();

  oldcolor = currentcolor;

if (screenRotation==1 || screenRotation==3)
{      
  if (p.x < BOXSIZE) {
     colorSelect(p.y);
  } 
  tft.fillCircle(p.y, p.x, PENRADIUS, currentcolor);
}  
  if (screenRotation==2 || screenRotation==4)
{      
  if (p.y < BOXSIZE) {
     colorSelect(p.x);
  }
  tft.fillCircle(p.x, p.y, PENRADIUS, currentcolor);
}
    }
  }

void colorSelect(int selectValue){
     if (selectValue < BOXSIZE) { 
       currentcolor = RED; 
       tft.drawRect(0, 0, BOXSIZE, BOXSIZE, WHITE);
     } else if (selectValue < BOXSIZE*2) {
       currentcolor = YELLOW;
       tft.drawRect(BOXSIZE, 0, BOXSIZE, BOXSIZE, WHITE);
     } else if (selectValue < BOXSIZE*3) {
       currentcolor = GREEN;
       tft.drawRect(BOXSIZE*2, 0, BOXSIZE, BOXSIZE, WHITE);
     } else if (selectValue < BOXSIZE*4) {
       currentcolor = CYAN;
       tft.drawRect(BOXSIZE*3, 0, BOXSIZE, BOXSIZE, WHITE);
     } else if (selectValue < BOXSIZE*5) {
       currentcolor = BLUE;
       tft.drawRect(BOXSIZE*4, 0, BOXSIZE, BOXSIZE, WHITE);
     } else if (selectValue <= BOXSIZE*6) {
       currentcolor = MAGENTA;
       tft.drawRect(BOXSIZE*5, 0, BOXSIZE, BOXSIZE, WHITE);
     }
     
     if (oldcolor != currentcolor) {
        if (oldcolor == RED) 
          tft.fillRect(0, 0, BOXSIZE, BOXSIZE, RED);
        if (oldcolor == YELLOW) 
          tft.fillRect(BOXSIZE, 0, BOXSIZE, BOXSIZE, YELLOW);
        if (oldcolor == GREEN) 
          tft.fillRect(BOXSIZE*2, 0, BOXSIZE, BOXSIZE, GREEN);
        if (oldcolor == CYAN) 
          tft.fillRect(BOXSIZE*3, 0, BOXSIZE, BOXSIZE, CYAN);
        if (oldcolor == BLUE) 
          tft.fillRect(BOXSIZE*4, 0, BOXSIZE, BOXSIZE, BLUE);
        if (oldcolor == MAGENTA) 
          tft.fillRect(BOXSIZE*5, 0, BOXSIZE, BOXSIZE, MAGENTA);
     }
}


