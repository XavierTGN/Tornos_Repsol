/*
 An example digital clock using a TFT LCD screen to show the time.
 Demonstrates use of the font printing routines. (Time updates but date does not.)

 It uses the time of compile/upload to set the time
 For a more accurate clock, it would be better to use the RTClib library.
 But this is just a demo...

 Make sure all the display driver and pin connections are correct by
 editing the User_Setup.h file in the TFT_eSPI library folder.

 #########################################################################
 ###### DON'T FORGET TO UPDATE THE User_Setup.h FILE IN THE LIBRARY ######
 #########################################################################

 Based on clock sketch by Gilchrist 6/2/2014 1.0

A few colour codes:

code	color
0x0000	Black
0xFFFF	White
0xBDF7	Light Gray
0x7BEF	Dark Gray
0xF800	Red
0xFFE0	Yellow
0xFBE0	Orange
0x79E0	Brown
0x7E0	Green
0x7FF	Cyan
0x1F	Blue
0xF81F	Pink

 */

#include <TFT_eSPI.h> // Hardware-specific library
#include "repsol.h"
#include <SPI.h>
#include <ESP32Time.h>
#include <TimeLib.h>

ESP32Time rtc(3600);  // offset in seconds GMT+1





#define TFT_GREY 0x5AEB

TFT_eSPI tft = TFT_eSPI();       // Invoke custom library
TFT_eSprite logo = TFT_eSprite(&tft);

uint32_t targetTime = 0;                    // for next 1 second timeout

static uint8_t conv2d(const char* p); // Forward declaration needed for IDE 1.6.x

/////////////uint8_t hh = conv2d(__TIME__), mm = conv2d(__TIME__ + 3), ss = conv2d(__TIME__ + 6); // Get H, M, S from compile time



byte omm = 99, oss = 99;
byte xcolon = 0, xsecs = 0;
unsigned int colour = 0;
int hh=0;
int mm=00;
int ss=00;
void setup(void) {
  //Serial.begin(115200);
  tft.init();
  tft.setRotation(1);
  tft.fillScreen(TFT_BLACK);
  setTime(13,47,11,24,3,2025);
  logo.setColorDepth(16);
  logo.setSwapBytes(true);
  logo.createSprite(110, 80);
  logo.setTextWrap(true);
  //logo.setCursor(0, 0);
  //logo.setTextFont(4);
  //logo.fillScreen(TFT_TRANSPARENT);
  //logo.print("logo logo sunt in culpa qui officia deserunt mollit anim id est laborum");
  logo.pushSprite(190, 0);
  logo.pushImage(0, 0, 110, 80, repsol);


  tft.setTextSize(1);
  tft.setTextColor(TFT_YELLOW, TFT_BLACK);
  rtc.setTime(30, 24, 15, 17, 1, 2021);  // 17th Jan 2021 15:24:30
  targetTime = millis() + 1000;
}
void leer_hora(){
  hh=hour();
  mm=minute();
  ss=second();
}
void loop() {
  logo.pushImage(0, 0, 110, 80, repsol);
  logo.pushSprite(190, 0);  //logo on
  leer_hora();
  
  if (targetTime < millis()) {
    // Set next update for 1 second later
    targetTime = millis() + 1000;
    /*
    // Adjust the time values by adding 1 second
    ss++;              // Advance second
    if (ss == 60) {    // Check for roll-over
      ss = 0;          // Reset seconds to zero
      omm = mm;        // Save last minute time for display update
      mm++;            // Advance minute
      if (mm > 59) {   // Check for roll-over
        mm = 0;
        hh++;          // Advance hour
        if (hh > 23) { // Check for 24hr roll-over (could roll-over on 13)
          hh = 0;      // 0 for 24 hour clock, set to 1 for 12 hour clock
        }
      }
    }
    */

    // Update digital time
    int xpos = 0;
    int ypos = 85; // Top left corner ot clock text, about half way down
    int ysecs = ypos + 24;

    if (omm != mm) { // Redraw hours and minutes time every minute
      omm = mm;
      // Draw hours and minutes
      if (hh < 10) xpos += tft.drawChar('0', xpos, ypos, 8); // Add hours leading zero for 24 hr clock
      xpos += tft.drawNumber(hh, xpos, ypos, 8);             // Draw hours
      xcolon = xpos; // Save colon coord for later to flash on/off later
      xpos += tft.drawChar(':', xpos, ypos - 8, 8);
      if (mm < 10) xpos += tft.drawChar('0', xpos, ypos, 8); // Add minutes leading zero
      xpos += tft.drawNumber(mm, xpos, ypos, 8);             // Draw minutes
      xsecs = xpos; // Sae seconds 'x' position for later display updates
    }
    if (oss != ss) { // Redraw seconds time every second
      oss = ss;
      xpos = xsecs;

      if (ss % 2) { // Flash the colons on/off
        tft.setTextColor(0x39C4, TFT_BLACK);        // Set colour to grey to dim colon
        tft.drawChar(':', xcolon, ypos - 8, 8);     // Hour:minute colon
        xpos += tft.drawChar(':', xsecs, ysecs, 6); // Seconds colon
        tft.setTextColor(TFT_YELLOW, TFT_BLACK);    // Set colour back to yellow
      }
      else {
        tft.drawChar(':', xcolon, ypos - 8, 8);     // Hour:minute colon
        xpos += tft.drawChar(':', xsecs, ysecs, 6); // Seconds colon
      }

      //Draw seconds
      if (ss < 10) xpos += tft.drawChar('0', xpos, ysecs, 6); // Add leading zero
      tft.drawNumber(ss, xpos, ysecs, 6);                     // Draw seconds
    }
  }
}


// Function to extract numbers from compile time string
static uint8_t conv2d(const char* p) {
  uint8_t v = 0;
  if ('0' <= *p && *p <= '9')
    v = *p - '0';
  return 10 * v + *++p - '0';
}



