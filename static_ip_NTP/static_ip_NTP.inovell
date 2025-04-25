/**
 * @file      StaticIPAddress.ino
 * @author    Lewis He (lewishe@outlook.com)
 * @license   MIT
 * @copyright Copyright (c) 2023  Shenzhen Xin Yuan Electronic Technology Co., Ltd
 * @date      2023-02-17
 *
 */
#include <Arduino.h>
#if ESP_ARDUINO_VERSION < ESP_ARDUINO_VERSION_VAL(3, 0, 0)
#include "ETHClass2.h"  //Is to use the modified ETHClass
#define ETH ETH2
#else
#include <ETH.h>
#endif
#include <SPI.h>
#include <SD.h>
#include "utilities.h"  //Board PinMap
#include <WiFi.h>
#include <NTPClient.h>
#include "time.h"

const char *ssid = "MI-9";
const char *password = "viscaTarracoII";

const char *ntpServer = "ntp.lonelybinary.com";
const long gmtOffset_sec = 0;//3600L * 1;
const int daylightOffset_sec = 0;

//Change to IP and DNS corresponding to your network, gateway
IPAddress staticIP(10, 82, 103, 215);
IPAddress gateway(10, 82, 103, 209);
IPAddress subnet(255, 255, 255, 240);
IPAddress dns(172, 16, 138, 119);

WiFiUDP ntpUDP;
//NTPClient timeClient(ntpUDP, "172.24.147.1", 3600, 60000);
NTPClient timeClient(ntpUDP, "hora.roa.es", 3600, 60000);

static bool eth_connected = false;


void setup() {
  Serial.begin(115200);
  WiFi.onEvent(WiFiEvent);

#ifdef ETH_POWER_PIN
  Serial.println("ETH_POWER_PIN");
  pinMode(ETH_POWER_PIN, OUTPUT);
  digitalWrite(ETH_POWER_PIN, HIGH);
#endif

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

  if (ETH.config(staticIP, gateway, subnet, dns, dns) == false) {
    Serial.println("Configuration failed.");
  }
  timeClient.begin();
  //***************************************************
    Serial.print ("Connecting to WiFi network ");
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }
    Serial.println("");
  //***************************************************
}

void loop() {
  timeClient.update();
  Serial.println(timeClient.getFormattedTime());

  //////if (eth_connected) {
  //////    testClient("172.24.147.1", 80);
  //////}
  delay(10000);
}

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
void WiFiEvent(arduino_event_id_t event) {
  switch (event) {
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
}