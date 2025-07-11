#include <Arduino.h>
#if ESP_ARDUINO_VERSION < ESP_ARDUINO_VERSION_VAL(3, 0, 0)
#include "ETHClass2.h"  //Is to use the modified ETHClass
#define ETH ETH2
#else
#include <ETH.h>
#endif
#include <SPI.h>
#include "utilities.h"  //Board PinMap
#include <WiFi.h>
#include <NTPClient.h>
#include <stdio.h>
#include <TimeLib.h>
#include <ESP32Time.h>
#include <Ethernet.h>

// IP de Repsol
IPAddress staticIP(10, 82, 103, 215);
IPAddress gateway(10, 82, 103, 209);
IPAddress subnet(255, 255, 255, 240);
IPAddress dns(172, 16, 138, 119);

//*** Temps **************************
struct tm Hora_RTC;
char dt[16];
char tm[16];
char sm[16];
unsigned long t_unix_date1;
int data = 0;
//************************************
WiFiUDP ntpUDP;
NTPClient Hora_NTP(ntpUDP, "172.18.155.2", 3600, 60000);  //360000);

bool eth_connected = false;

String icono_red_ok = "✅";
String icono_red_ko = "❌";

void setup() {
  Serial.begin(115200);

#ifdef ETH_POWER_PIN
  pinMode(ETH_POWER_PIN, OUTPUT);
  digitalWrite(ETH_POWER_PIN, HIGH);
#endif

  WiFi.onEvent(WiFiEvent);

  if (!ETH.begin(
        ETH_PHY_W5500,  // PHY
        1,              // SPI bus num
        ETH_CS_PIN, ETH_INT_PIN, ETH_RST_PIN,
        SPI3_HOST,  // SPI host
        ETH_SCLK_PIN, ETH_MISO_PIN, ETH_MOSI_PIN)) {
    Serial.println("ETH start Failed!");
  }

  if (!ETH.config(staticIP, gateway, subnet, dns)) {
    Serial.println("❌ Falló configuración IP estática.");
  } else {
    Serial.println("✅ IP estática configurada.");
  }


  if (eth_connected) {
    Serial.print("PING: IP = ");
    Serial.println(ETH.localIP());
  }
}

void loop() {
  delay(2000);
  // *** temps *****
  t_unix_date1 = Hora_NTP.getEpochTime();
  struct tm Hora_RTC;
  if (eth_connected == true) {
  sprintf(tm, "✅⏰NTP:UNIX#: %4d-%02d-%02d %02d:%02d:%02d\n" , year(t_unix_date1), month(t_unix_date1), day(t_unix_date1), hour(t_unix_date1), minute(t_unix_date1), second(t_unix_date1));
    }else{
  sprintf(tm, "❌⏰NTP:UNIX#: %4d-%02d-%02d %02d:%02d:%02d\n" , year(t_unix_date1), month(t_unix_date1), day(t_unix_date1), hour(t_unix_date1), minute(t_unix_date1), second(t_unix_date1));
  }
  Serial.print(tm);
  //*********************************************************
  //  el Update, solo enviará una solicitud al servidor NTP
  // si ha pasado el intervalo de actualización configurado
  Hora_NTP.update();
  // *********************************************************

  Lleigir_serial();
}

void WiFiEvent(arduino_event_id_t event) {
  switch (event) {
    case ARDUINO_EVENT_ETH_GOT_IP:
      Serial.print("🟢 IP asignada: ");
      Serial.println(ETH.localIP());
      eth_connected = true;
      break;
    case ARDUINO_EVENT_ETH_DISCONNECTED:
      Serial.println("🔴 ETH desconectado");
      eth_connected = false;
      break;
    default:
      break;
  }
}

void Lleigir_serial() {
  char incomingByte;
  // send data only when you receive data:
  if (Serial.available() > 0) {
    // read the incoming byte:
    incomingByte = Serial.read();
    if (incomingByte == 'H') {
      Serial.println("W-->activa la Wifi");
      Serial.println("E-->activa la Ethernet");
      Serial.println("S-->Status IPs");
      Serial.println("H-->Aquest menu");
    }
    if (incomingByte == 'W') {
      Serial.print("Wifi");
    }
    if (incomingByte == 'U') {
      Serial.print("Update date");
      t_unix_date1 = Hora_NTP.getEpochTime();
    }
    if (incomingByte == '-') {
      // Restar una hora (3600 segundos)
      t_unix_date1 -= 3600;
    }
    if (incomingByte == 'S') {
      Serial.println("Actualizando hora de RTC desde UNIX");
    }
  }
}
