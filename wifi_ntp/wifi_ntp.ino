#include <WiFi.h>
#include "time.h"

const char *ssid = "MI-9";
const char *password = "viscaTarracoII";



const char *ntpServer = "ntp.lonelybinary.com";
const long gmtOffset_sec = 0;//3600L * 1;
const int daylightOffset_sec = 0;

void setup()
{
    Serial.begin(115200);

    Serial.print ("Connecting to WiFi network ");
      WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED)
    {
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
    while (!getLocalTime(&timeinfo))
    {
        configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
        delay(500);
        Serial.print("");
    }
    Serial.println("");

    // disconnect WiFi
    WiFi.disconnect(true);
    WiFi.mode(WIFI_OFF);
}

void loop()
{

    struct tm timeinfo;
    getLocalTime(&timeinfo);
    
    Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
    delay(10000);
}