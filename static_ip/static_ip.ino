/**
 * @file      static_ip.ino
 * @author    Xavier Banus
 * @license   NIT
 * @copyright Copyright (c) 2025 
 * @date      2025-06-17
 *  Funciona correcto con el servidor de Repsol , mediante EThernet
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
#include <stdio.h>
#include <TimeLib.h>
#include <ESP32Time.h>
ESP32Time rtc;

//*** Temps **************************
struct tm timeinfo;
char dt[16];
char tm[16];
char sm[16];
unsigned long t_unix_date1;
//************************************

//Change to IP and DNS corresponding to your network, gateway
IPAddress staticIP(10, 82, 103, 215);
IPAddress gateway(10, 82, 103, 209);
IPAddress subnet(255, 255, 255, 240);
IPAddress dns(172, 16, 138, 119);

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "172.18.155.2", 3600, 60000);

static bool eth_connected = false;

void setup() {
  Serial.begin(115200);
  WiFi.onEvent(WiFiEvent);
#ifdef ETH_POWER_PIN
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
  timeClient.update();
  delay(1000);


  //rtc.setTime(30, 45, 3, 1, 6, 2030);  // segundos, minutos, horas, día, mes, año
  ///////rtc.setTime(0, 0, 0, 1, 1, 2025); // segundos, minutos, horas, día, mes, año

  struct tm timeinfo;
  timeinfo.tm_year = 2019 - 1900;  // Año desde 1900
  timeinfo.tm_mon = 0;             // Mes (0-11)
  timeinfo.tm_mday = 1;            // Día del mes
  timeinfo.tm_hour = 0;
  timeinfo.tm_min = 0;
  timeinfo.tm_sec = 0;
  time_t t = mktime(&timeinfo);
  struct timeval now = { .tv_sec = t };
  settimeofday(&now, NULL);
}

void loop() {
  // *** temps *****
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    Serial.println("Error obteniendo la hora");
  }
  //Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
  //Serial.print("getmForm: ");
  //Serial.println(timeClient.getFormattedTime());

  t_unix_date1 = timeClient.getEpochTime();
  sprintf(tm, "UNIX#: %4d-%02d-%02d %02d:%02d:%02d\n", year(t_unix_date1), month(t_unix_date1), day(t_unix_date1), hour(t_unix_date1), minute(t_unix_date1), second(t_unix_date1));
  Serial.println(tm);

  //*******************

  timeClient.update();
  //Serial.print(timeClient.getFormattedTime());

  //////if (eth_connected) {
  //////    testClient("172.24.147.1", 80);
  //////}
  /*
  Serial.print("Hora rtc:");
  Serial.println(rtc.getTime());  // Imprime la fecha y hora actual


  Serial.print("Fecha rtc: ");
  Serial.print(rtc.getDay());
  Serial.print("/");
  Serial.print(rtc.getMonth());
  Serial.print("/");
  int year = rtc.getYear();
  Serial.println(year);
*/


 Serial.print("Fecha rtc: ");
 Serial.print(timeinfo.tm_mday);
 Serial.print("/");
 Serial.print(timeinfo.tm_mon + 1);
 Serial.print("/");
 Serial.println(timeinfo.tm_year + 1900);

 Serial.print("Hora rtc: ");
 Serial.print(timeinfo.tm_hour);
 Serial.print(":");
 Serial.print(timeinfo.tm_min);
 Serial.print(":");
 Serial.println(timeinfo.tm_sec);




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
      Serial.println("ETH Started");
      //set eth hostname here
      ETH.setHostname("esp32-ethernet");
      break;
    case ARDUINO_EVENT_ETH_CONNECTED:
      Serial.println("ETH Connected");
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