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
#include "utilities.h"  //Board PinMap
#include <WiFi.h>
#include <NTPClient.h>
#include <stdio.h>
#include <TimeLib.h>
#include <ESP32Time.h>
#include <Ethernet.h>


//*** Temps **************************
struct tm Hora_RTC;
char dt[16];
char tm[16];
char sm[16];
unsigned long t_unix_date1;
int data = 0;
//************************************

// IP de Repsol
IPAddress staticIP(10, 82, 103, 215);
IPAddress gateway(10, 82, 103, 209);
IPAddress subnet(255, 255, 255, 240);
IPAddress dns(172, 16, 138, 119);

// Dirección MAC del módulo Ethernet
byte mac[] = { 0xF2, 0xF5, 0xBD, 0x4B, 0xEA, 0x08 };

// Inicializar el servidor en el puerto 80
EthernetServer server(80);

// Iniciar Ethernet
if (Ethernet.begin(mac) == 0) {
  Serial.println("Error al iniciar Ethernet con DHCP");
  // Intentar con una IP estática
  Ethernet.begin(mac, IPAddress(10, 82, 103, 215));
}


//** es la hora? **********************************************************
//const unsigned long interval = 2 * 60 * 60 * 1000; // 2 horas en milisegundos
const unsigned long interval = 120 * 1000;  // 2 horas en milisegundos
unsigned long previousMillis = 0;
// ************************************************************************

WiFiUDP ntpUDP;
NTPClient Hora_NTP(ntpUDP, "172.18.155.2", 3600, 360000);

static bool eth_connected = false;

void setup() {
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
  Serial.begin(115200);
  WiFi.onEvent(WiFiEvent);

  Hora_NTP.begin();
  delay(1000);
  Hora_NTP.update();
  delay(1000);


  //rtc.setTime(30, 45, 3, 1, 6, 2030);  // segundos, minutos, horas, día, mes, año
  ///////rtc.setTime(0, 0, 0, 1, 1, 2025); // segundos, minutos, horas, día, mes, año

  struct tm Hora_RTC;
  Hora_RTC.tm_year = 2019 - 1900;  // Año desde 1900
  Hora_RTC.tm_mon = 0;             // Mes (0-11)
  Hora_RTC.tm_mday = 1;            // Día del mes
  Hora_RTC.tm_hour = 0;
  Hora_RTC.tm_min = 0;
  Hora_RTC.tm_sec = 0;
  time_t t = mktime(&Hora_RTC);
  struct timeval now = { .tv_sec = t };
  settimeofday(&now, NULL);
}

void loop() {

  EthernetClient client = server.available();
  if (client) {
    Serial.println("Nuevo cliente conectado");
    // Leer la solicitud del cliente
    String request = client.readStringUntil('\r');
    Serial.println(request);
    client.flush();

    // Enviar respuesta al cliente
    client.println("HTTP/1.1 200 OK");
    client.println("Content-Type: text/html");
    client.println();
    client.println("<!DOCTYPE HTML>");
    client.println("<html>");
    client.println("<h1>¡Hola, Mundo!</h1>");
    client.println("</html>");

    // Dar tiempo al navegador para recibir los datos
    delay(1);
    // Cerrar la conexión
    client.stop();
    Serial.println("Cliente desconectado");
  }




  // *** temps *****
  struct tm Hora_RTC;
  if (!getLocalTime(&Hora_RTC)) {
    Serial.println("Error obteniendo la hora");
  }
  t_unix_date1 = Hora_NTP.getEpochTime();
  sprintf(tm, "UNIX#: %4d-%02d-%02d %02d:%02d:%02d\n", year(t_unix_date1), month(t_unix_date1), day(t_unix_date1), hour(t_unix_date1), minute(t_unix_date1), second(t_unix_date1));
  Serial.print(tm);
  //*******************
  //  el Update, solo enviará una solicitud al servidor NTP si ha pasado el intervalo de actualización configurado
  Hora_NTP.update();


  //////if (eth_connected) {
  //////    testClient("172.24.147.1", 80);
  //////}
  // hora
  sprintf(tm, "RTC #: %4d-%02d-%02d %02d:%02d:%02d\n", Hora_RTC.tm_year + 1900, Hora_RTC.tm_mon, Hora_RTC.tm_mday, Hora_RTC.tm_hour, Hora_RTC.tm_min, Hora_RTC.tm_sec);
  Serial.println(tm);

  opcions_teclat();
  delay(5000);
  es_la_hora();
}
void opcions_teclat() {
  if (Serial.available() > 0) {
    data = Serial.read();
    //Serial.println(data, DEC);
    if (data == 'S') {
      Serial.println("Actualizando hora de RTC desde UNIX");
      t_unix_date1 = Hora_NTP.getEpochTime();
      Hora_RTC.tm_year = year(t_unix_date1) - 1900;  // Año desde 1900
      Hora_RTC.tm_mon = month(t_unix_date1);         // Mes (0-11)
      Hora_RTC.tm_mday = day(t_unix_date1);          // Día del mes
      Hora_RTC.tm_hour = hour(t_unix_date1);
      Hora_RTC.tm_min = minute(t_unix_date1);
      Hora_RTC.tm_sec = second(t_unix_date1);
      time_t t = mktime(&Hora_RTC);
      struct timeval now = { .tv_sec = t };
      settimeofday(&now, NULL);
      delay(200);
    }
    if (data == '-') {  // resta 1 hora
      Serial.println("Resta una hora");
      t_unix_date1 = Hora_NTP.getEpochTime();
      Hora_RTC.tm_year = year(t_unix_date1) - 1900;  // Año desde 1900
      Hora_RTC.tm_mon = month(t_unix_date1);         // Mes (0-11)
      Hora_RTC.tm_mday = day(t_unix_date1);          // Día del mes
      Hora_RTC.tm_hour = hour(t_unix_date1) - 1;
      Hora_RTC.tm_min = minute(t_unix_date1);
      Hora_RTC.tm_sec = second(t_unix_date1);
      time_t t = mktime(&Hora_RTC);
      struct timeval now = { .tv_sec = t };
      settimeofday(&now, NULL);
      delay(200);
    }
    if (data == 'U') {  // actualiza hora
      Serial.println("Actualiza hora");
      Hora_NTP.begin();
      Hora_NTP.update();
      delay(200);
    }
  }
}
void es_la_hora() {
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    // Código a ejecutar cada 2 horas
    Serial.println("Han pasado 2 minutos");
  }
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