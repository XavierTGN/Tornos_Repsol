/**
 * @file      StaticIPAddress.ino
 * @author    Lewis He (lewishe@outlook.com)
 * @license   MIT
 * @copyright Copyright (c) 2023  Shenzhen Xin Yuan Electronic Technology Co., Ltd
 * @date      2023-02-17
 *
 */
/*
 Definits a utilities.h
#define ETH_TYPE                        ETH_PHY_RTL8201
#define ETH_ADDR                        0
#define ETH_CLK_MODE                    ETH_CLOCK_GPIO0_IN
#define ETH_RESET_PIN                   -1
#define ETH_MDC_PIN                     23
#define ETH_POWER_PIN                   12
#define ETH_MDIO_PIN                    18
#define SD_MISO_PIN                     34
#define SD_MOSI_PIN                     13
#define SD_SCLK_PIN                     14
#define SD_CS_PIN                       5
*/
#include <TimeLib.h>
#include <Arduino.h>
#include <stdio.h>
#if ESP_ARDUINO_VERSION < ESP_ARDUINO_VERSION_VAL(3, 0, 0)
#include "ETHClass2.h"  //Is to use the modified ETHClass
#define ETH ETH2
#else
#include <ETH.h>
#endif
#include <SPI.h>
//#include <SD.h>
#include "utilities.h"  //Board PinMap
#include <WiFi.h>
#include <NTPClient.h>
#include "time.h"

const char *ssid = "MI-9";
const char *password = "viscaTarracoII";

const char *ntpServer = "ntp.lonelybinary.com";
const long gmtOffset_sec = 0;  //3600L * 1;
const int daylightOffset_sec = 0;
struct tm timeinfo;

char dt[16];
char tm[16];
char sm[16];
//bool is_Wifi = true;
bool is_Wifi=true;

/*
//Change to IP and DNS corresponding to your network, gateway
IPAddress staticIP(10, 82, 103, 215);
IPAddress gateway(10, 82, 103, 209);
IPAddress subnet(255, 255, 255, 240);
IPAddress dns(172, 16, 138, 119);
*/
IPAddress staticIP(192, 168, 1, 10);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);
// la dns , si no es correcte no te access a internet i no es pot accedir al servidor ntp
// dns de casa -->80.58.61.250
IPAddress dns(80, 58, 61, 250);

WiFiUDP ntpUDP;
//NTPClient timeClient(ntpUDP, "172.24.147.1", 3600, 60000);
NTPClient timeClient(ntpUDP, "hora.roa.es", 3600, 60000);

static bool eth_connected = false;


void setup() {
  Serial.begin(115200);
  WiFi.onEvent(WiFiEvent);

  /*
#ifdef ETH_POWER_PIN
  Serial.println("ETH_POWER_PIN");
  pinMode(ETH_POWER_PIN, OUTPUT);
  digitalWrite(ETH_POWER_PIN, HIGH);
#endif
*/
/*
#if CONFIG_IDF_TARGET_ESP32
  if (!ETH.begin(ETH_TYPE, ETH_ADDR, ETH_MDC_PIN,
                 ETH_MDIO_PIN, ETH_RESET_PIN, ETH_CLK_MODE)) {
    Serial.println("ETH start Failed!");
  }
#else
  if (!ETH.begin(ETH_PHY_W5500, 1, ETH_CS_PIN, ETH_INT_PIN, ETH_RST_PIN,
                 SPI3_HOST,
                 ETH_SCLK_PIN, ETH_MISO_PIN, ETH_MOSI_PIN)) {
    Serial.println("ETH start Failed!");
  }
#endif

*/


  //***************************************************
  //start_wifi();
  start_eth();
  //***************************************************

  timeClient.begin();
  delay(2000);
  timeClient.update();
}

void start_wifi() {
  Serial.print("Connecting to WiFi network ");
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
    Serial.print("Dirección IP WiFi: ");
    Serial.println(WiFi.localIP());
  //ETH.end();
}
void start_eth() {
  ETH.config(staticIP, gateway, subnet, dns, dns);
  ETH.begin(ETH_PHY_W5500, 1, ETH_CS_PIN, ETH_INT_PIN, ETH_RST_PIN,
                 SPI3_HOST, ETH_SCLK_PIN, ETH_MISO_PIN, ETH_MOSI_PIN);
  // no funciona Wifi.end();
}


void loop() {
  char incomingByte;
  unsigned long t_unix_date1;
  t_unix_date1 = timeClient.getEpochTime();
  sprintf(tm, "Date1: %4d-%02d-%02d %02d:%02d:%02d\n", year(t_unix_date1), month(t_unix_date1), day(t_unix_date1), hour(t_unix_date1), minute(t_unix_date1), second(t_unix_date1));
  Serial.println(tm);
  delay(1000);
  // send data only when you receive data:
  if (Serial.available() > 0) {
    // read the incoming byte:
    incomingByte = Serial.read();
    if (incomingByte=='A') {
    Serial.print("Wifi");
    start_wifi();
    }
    if (incomingByte=='E') {
    Serial.print("Ethernet");
    start_eth();
    }
    // say what you got:
    Serial.print("I received: ");
    Serial.println(incomingByte);
  }

}

void WiFiEvent(arduino_event_id_t event) {
  //Serial.print("event: ");
  //Serial.println(event);
  switch (event) {
    case ARDUINO_EVENT_WIFI_READY: Serial.println("WiFi interface ready"); break;
    case ARDUINO_EVENT_WIFI_SCAN_DONE: Serial.println("Completed scan for access points"); break;
    case ARDUINO_EVENT_WIFI_STA_START: Serial.println("WiFi client started"); break;
    case ARDUINO_EVENT_WIFI_STA_STOP: Serial.println("WiFi clients stopped"); break;
    case ARDUINO_EVENT_WIFI_STA_CONNECTED: Serial.println("Connected to access point"); break;
    case ARDUINO_EVENT_WIFI_STA_DISCONNECTED: Serial.println("Disconnected from WiFi access point"); break;
    case ARDUINO_EVENT_WIFI_STA_AUTHMODE_CHANGE: Serial.println("Authentication mode of access point has changed"); break;
    case ARDUINO_EVENT_WIFI_STA_GOT_IP:
      Serial.print("Obtained IP address: ");
      Serial.println(WiFi.localIP());
      break;
    case ARDUINO_EVENT_WIFI_STA_LOST_IP: Serial.println("Lost IP address and IP address is reset to 0"); break;
    case ARDUINO_EVENT_WPS_ER_SUCCESS: Serial.println("WiFi Protected Setup (WPS): succeeded in enrollee mode"); break;
    case ARDUINO_EVENT_WPS_ER_FAILED: Serial.println("WiFi Protected Setup (WPS): failed in enrollee mode"); break;
    case ARDUINO_EVENT_WPS_ER_TIMEOUT: Serial.println("WiFi Protected Setup (WPS): timeout in enrollee mode"); break;
    case ARDUINO_EVENT_WPS_ER_PIN: Serial.println("WiFi Protected Setup (WPS): pin code in enrollee mode"); break;
    case ARDUINO_EVENT_WIFI_AP_START: Serial.println("WiFi access point started"); break;
    case ARDUINO_EVENT_WIFI_AP_STOP: Serial.println("WiFi access point  stopped"); break;
    case ARDUINO_EVENT_WIFI_AP_STACONNECTED: Serial.println("Client connected"); break;
    case ARDUINO_EVENT_WIFI_AP_STADISCONNECTED: Serial.println("Client disconnected"); break;
    case ARDUINO_EVENT_WIFI_AP_STAIPASSIGNED: Serial.println("Assigned IP address to client"); break;
    case ARDUINO_EVENT_WIFI_AP_PROBEREQRECVED: Serial.println("Received probe request"); break;
    case ARDUINO_EVENT_WIFI_AP_GOT_IP6: Serial.println("AP IPv6 is preferred"); break;
    case ARDUINO_EVENT_WIFI_STA_GOT_IP6: Serial.println("STA IPv6 is preferred"); break;
    case ARDUINO_EVENT_ETH_GOT_IP6: Serial.println("Ethernet IPv6 is preferred"); break;
    case ARDUINO_EVENT_ETH_START: Serial.println("Ethernet started"); break;
    case ARDUINO_EVENT_ETH_STOP: Serial.println("Ethernet stopped"); break;
    case ARDUINO_EVENT_ETH_CONNECTED:
         Serial.print("Ethernet connected--");
         Serial.print("Dirección IP ethernet: ");
         Serial.println(ETH.localIP());
         break;
    case ARDUINO_EVENT_ETH_DISCONNECTED: Serial.println("Ethernet disconnected"); break;
    case ARDUINO_EVENT_ETH_GOT_IP: Serial.println("Obtained IP address"); break;
    default: break;
  }
  /*
  switch (event) {
// wifi
    case ARDUINO_EVENT_ETH_START:
      Serial.print("ETH Started");
      //set eth hostname here
      ETH.setHostname("esp32-ethernet");
      break;
    case ARDUINO_EVENT_ETH_START:
      Serial.print("ETH Started");
      //set eth hostname here
      ETH.setHostname("esp32-ethernet");
      break;
    case ARDUINO_EVENT_ETH_START:
      Serial.print("ETH Started");
      //set eth hostname here
      ETH.setHostname("esp32-ethernet");
      break;

    case ARDUINO_EVENT_ETH_START:
      Serial.print("ETH Started");
      //set eth hostname here
      ETH.setHostname("esp32-ethernet");
      break;
    case ARDUINO_EVENT_ETH_CONNECTED:
      Serial.println("ETH Connected, ip->");
      Serial.println(ETH.localIP());
      break;
    case ARDUINO_EVENT_ETH_GOT_IP:
      Serial.print("ETH MAC: ");
      Serial.print(ETH.macAddress());
      Serial.print(", IPv4: ");
      Serial.print(ETH.localIP());
      if (ETH.fullDuplex()) {
        Serial.print(", FULL_DUPLEX");
      }
      Serial.print(", ");
      Serial.print(ETH.linkSpeed());
      Serial.println("Mbps");
      eth_connected = true;
      break;
    case ARDUINO_EVENT_ETH_DISCONNECTED:
      Serial.println("ETH Disconnected");
      eth_connected = false;
      break;
    case ARDUINO_EVENT_ETH_STOP:
      Serial.println("ETH Stopped");
      eth_connected = false;
      break;
    default:
      break;
  }
  */
}
/*
void testClient(const char *host, uint16_t port) {
  Serial.print("\nconnecting to ");
  Serial.println(host);

  WiFiClient client;
  if (!client.connect(host, port)) {
    Serial.println("connection failed");
    return;
  }
  client.printf("GET / HTTP/1.1\r\nHost: %s\r\n\r\n", host);
  while (client.connected() && !client.available())
    ;
  while (client.available()) {
    Serial.write(client.read());
  }

  Serial.println("closing connection\n");
  client.stop();
}
*/