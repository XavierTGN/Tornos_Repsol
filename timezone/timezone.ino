// Arduino Timezone Library Copyright (C) 2018 by Jack Christensen and
// licensed under GNU GPL v3.0, https://www.gnu.org/licenses/gpl.html
//
// Arduino Timezone Library example sketch.
// Sketch to verify operation of Timezone library.
// Jack Christensen 14Nov2018

#include <WiFi.h>
#include <Timezone.h>  // https://github.com/JChristensen/Timezone
#include <TimeLib.h>   // https://github.com/PaulStoffregen/Time
#include "time.h"

const char *ssid = "MI-9";
const char *password = "viscaTarracoII";

const char *ntpServer = "ntp.lonelybinary.com";
const long gmtOffset_sec = 0;  //3600L * 1;
const int daylightOffset_sec = 0;


// Spain Time Zone
TimeChangeRule esEstiu = { "CET", Last, Sun, Mar, 2, 60 };     // UTC + 2 hours
TimeChangeRule esHivern = { "CEST", Last, Sun, Oct, 3, 120 };  // UTC + 1 hours
Timezone cet(esEstiu, esHivern);
/*
// US Eastern Time Zone (New York, Detroit)
TimeChangeRule etDST = {"EDT", Second, Sun, Mar, 2, -240};  // Daylight time = UTC - 4 hours
TimeChangeRule etSTD = {"EST", First, Sun, Nov, 2, -300};   // Standard time = UTC - 5 hours
Timezone et(etDST, etSTD);
*/
void setup() {
  Serial.begin(115200);

  conectar_wifi_ntp();

  // Spain
  printTimes(1, 3, 2025, esEstiu.hour, esHivern.offset, cet);  // day, month, year, hour, offset, tz
  printTimes(30, 10, 2025, esEstiu.hour, esHivern.offset, cet);
}

void loop() {

  struct tm timeinfo;
  getLocalTime(&timeinfo);

  Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
  delay(10000);
}

// print corresponding UTC and local times "n" seconds before and after the time change.
// h is the hour to change the clock using the *current* time (i.e. before the change).
// offset is the utc offset in minutes for the time *after* the change.
void printTimes(uint8_t d, uint8_t m, int y, uint8_t h, int offset, Timezone tz) {
  const time_t n(3);  // number of times to print before and after the time change
  tmElements_t tm;
  tm.Hour = h;
  tm.Minute = 0;
  tm.Second = 0;
  tm.Day = d;
  tm.Month = m;
  tm.Year = y - 1970;  // offset from 1970
  time_t utc = makeTime(tm) - offset * SECS_PER_MIN - n;

  Serial.print(F("\n-------- "));
  Serial.print(monthShortStr(m));
  Serial.print('-');
  Serial.print(y);
  Serial.print(F(" time change --------\n"));

  for (uint16_t i = 0; i < n * 2; i++) {
    TimeChangeRule *tcr;  // pointer to the time change rule, use to get TZ abbrev
    time_t local = tz.toLocal(utc, &tcr);
    printDateTime(utc, "UTC = ");
    printDateTime(local, tcr->abbrev);
    Serial.println();
    ++utc;
  }
}

// format and print a time_t value, with a time zone appended.
void printDateTime(time_t t, const char *tz) {
  char buf[32];
  char m[4];  // temporary storage for month string (DateStrings.cpp uses shared buffer)
  strcpy(m, monthShortStr(month(t)));
  sprintf(buf, "%.2d:%.2d:%.2d %s %.2d %s %d %s",
          hour(t), minute(t), second(t), dayShortStr(weekday(t)), day(t), m, year(t), tz);
  Serial.print(buf);
}
void conectar_wifi_ntp() {
  Serial.print("Connecting to WiFi network ");
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");

  /*
        Sync time with NTP server and update ESP32 RTC
        getLocalTime() return false if time is not set
    */
  Serial.print("Syncing time with NTP server ");
  struct tm timeinfo;
  while (!getLocalTime(&timeinfo)) {
    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
    delay(500);
    Serial.print("");
  }
  Serial.println("Wifi desconectada");

  // disconnect WiFi
  WiFi.disconnect(true);
  WiFi.mode(WIFI_OFF);
}