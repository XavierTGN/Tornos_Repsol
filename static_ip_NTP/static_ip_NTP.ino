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
#include <WebServer.h>



//const char *ssid = "ELECTRONICA";
//const char *password = "repsol01";

//const char *ssid = "MOVISTAR-WIFI6-6810";
//const char *password = "N9HEPszqVUT93Js79xqs";



struct tm timeinfo;

char dt[16];
char tm[16];
char sm[16];
unsigned long t_unix_date1;


// Configuración BANUS
char *ssid = "MOVISTAR-WIFI6-6810";
char *password = "N9HEPszqVUT93Js79xqs";
//char *ssid = "MI-9";
//char *password = "viscaTarracoII";
// Ntp test
char *ntpServer = "pool.ntp.org";
long gmtOffset_sec = 3600;  //3600L * 1;
int daylightOffset_sec = 3600;

// WIFI Configuración de IP estática
IPAddress wifiIP(192, 168, 1, 10);  // IP estática para Wi-Fi
IPAddress wifiGateway(192, 168, 1, 1);
IPAddress wifiSubnet(255, 255, 255, 0);
IPAddress wifiprimaryDNS(80, 58, 61, 250);  // Opcional
IPAddress wifisecondaryDNS(8, 8, 4, 4);     // Opcional

// Ethernet configuracion de IP estatica
IPAddress ETHstaticIP(192, 168, 1, 11);
IPAddress ETHgateway(192, 168, 1, 1);
IPAddress ETHsubnet(255, 255, 255, 0);
// la dns , si no es correcte no te access a internet i no es pot accedir al servidor ntp
// dns de casa -->80.58.61.250
IPAddress dns(80, 58, 61, 250);


/*
    // Configuracion REPSOL
    char *ssid = "ELECTRONICA";
    char *password = "repsol01";
    // Ntp Repsol
    char *ntpServer = "172.24.147.1";
    //const char *ntpServer = "hora.rediris.es";
    long gmtOffset_sec = 0;  //3600L * 1;
    int daylightOffset_sec = 0;
    // WIFI Configuración de IP estática
    IPAddress wifiIP(192, 168, 1, 10);  // IP estática para Wi-Fi
    IPAddress wifiGateway(192, 168, 1, 1);
    IPAddress wifiSubnet(255, 255, 255, 0);
    IPAddress wifiprimaryDNS(8, 8, 8, 8);    // Opcional
    IPAddress wifisecondaryDNS(8, 8, 4, 4);  // Opcional

    // Ethernet configuracion de IP estatica
    IPAddress ETHstaticIP(10, 82, 103, 215);
    IPAddress ETHgateway(10, 82, 103, 209);
    IPAddress ETHsubnet(255, 255, 255, 240);
    // la dns , si no es correcte no te access a internet i no es pot accedir al servidor ntp
    // dns de casa -->80.58.61.250
    IPAddress dns(80, 58, 61, 250);
*/

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, ntpServer, 3600, 60000);
// server
WebServer server(80);
String serialData = "";



void setup() {
  //configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  Serial.begin(115200);
  WiFi.onEvent(WiFiEvent);

  //***************************************************
  start_eth();
  start_wifi();


  //***************************************************
  delay(1000);
  timeClient.begin();
  delay(1000);
  timeClient.update();

  //***** SERVER ***********************************************************************
  server.on("/", handleRoot);
  server.on("/login", handleLogin);
  server.on("/inline", []() {
    server.send(200, "text/plain", "this works without need of authentication");
  });

  server.onNotFound(handleNotFound);
  //here the list of headers to be recorded
  const char *headerkeys[] = { "User-Agent", "Cookie" };
  size_t headerkeyssize = sizeof(headerkeys) / sizeof(char *);
  //ask server to track these headers
  server.collectHeaders(headerkeys, headerkeyssize);
  server.begin();
  Serial.println("HTTP server started");
  //**********************************************************************************
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
}




void loop() {

  //*******************
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    Serial.println("Error obteniendo la hora");
  }
  Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
  //**********************
  timeClient.update();
  Serial.print("getmForm: ");
  Serial.println(timeClient.getFormattedTime());
  delay(1000);
  //if (Serial.available()) {
  serialData += Serial.readString() + "\n";  // Captura datos del puerto serie
  //}

  server.handleClient();



  t_unix_date1 = timeClient.getEpochTime();
  sprintf(tm, "Date: %4d-%02d-%02d %02d:%02d:%02d\n", year(t_unix_date1), month(t_unix_date1), day(t_unix_date1), hour(t_unix_date1), minute(t_unix_date1), second(t_unix_date1));
  handleRoot();
  Serial.println(tm);
  delay(1000);

  Lleigir_serial();
}

void start_wifi() {
  //Configurar Wifi
  if (!WiFi.config(wifiIP, wifiGateway, wifiSubnet, wifiprimaryDNS)) {
    Serial.println("Error al configurar la IP estática");
  }
  // Conectar a WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nConectado a WiFi");
  Serial.print("Dirección IP: ");
  Serial.println(WiFi.localIP());
}
void start_eth() {
  ETH.begin(ETH_PHY_W5500, 1, ETH_CS_PIN, ETH_INT_PIN, ETH_RST_PIN, SPI3_HOST, ETH_SCLK_PIN, ETH_MISO_PIN, ETH_MOSI_PIN);
  //  ETH.config(IPAddress(192, 168, 1, 100), IPAddress(192, 168, 1, 1), IPAddress(255, 255, 255, 0));
  ETH.config(ETHstaticIP, ETHgateway, ETHsubnet);
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
      start_wifi();
    }
    if (incomingByte == 'E') {
      Serial.print("Ethernet");
      start_eth();
    }
    if (incomingByte == 'S') {
      Serial.print("Wifi IP address: ");
      Serial.println(WiFi.localIP());
      Serial.print("Ethernet IP Address: ");
      Serial.println(ETH.localIP());
      Serial.print("BUSCA EN SERVIDOR ntp: ");
      Serial.println(ntpServer);
      Serial.println();
    }
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
    case ARDUINO_EVENT_WIFI_STA_CONNECTED:
      Serial.print("Connected to access point:");
      Serial.println(ssid);
      break;
    case ARDUINO_EVENT_WIFI_STA_DISCONNECTED:
      Serial.print("Disconnected from WiFi access point");
      Serial.println(ssid);
      break;
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
    case ARDUINO_EVENT_ETH_START:
      Serial.print("Ethernet started:");
      Serial.println(ETH.localIP());
      break;
    case ARDUINO_EVENT_ETH_STOP: Serial.println("Ethernet stopped"); break;
    case ARDUINO_EVENT_ETH_CONNECTED: Serial.println("Ethernet connected"); break;
    case ARDUINO_EVENT_ETH_DISCONNECTED: Serial.println("Ethernet disconnected"); break;
    case ARDUINO_EVENT_ETH_GOT_IP:
      Serial.print("Obtained IP address:");
      Serial.println(ETH.localIP());
      break;
    default: break;
  }
}



//Check if header is present and correct
bool is_authentified() {
  Serial.println("Enter is_authentified");
  if (server.hasHeader("Cookie")) {
    Serial.print("Found cookie: ");
    String cookie = server.header("Cookie");
    Serial.println(cookie);
    if (cookie.indexOf("ESPSESSIONID=1") != -1) {
      Serial.println("Authentication Successful");
      return true;
    }
  }
  Serial.println("Authentication Failed");
  return false;
}

//login page, also called for disconnect
void handleLogin() {
  String msg;
  if (server.hasHeader("Cookie")) {
    Serial.print("Found cookie: ");
    String cookie = server.header("Cookie");
    Serial.println(cookie);
  }
  if (server.hasArg("DISCONNECT")) {
    Serial.println("Disconnection");
    server.sendHeader("Location", "/login");
    server.sendHeader("Cache-Control", "no-cache");
    server.sendHeader("Set-Cookie", "ESPSESSIONID=0");
    server.send(301);
    return;
  }
  if (server.hasArg("PASSWORD")) {
    // if (server.arg("USERNAME") == "admin" && server.arg("PASSWORD") == "admin") {
    if (server.arg("PASSWORD") == "repsol01") {
      server.sendHeader("Location", "/");
      server.sendHeader("Cache-Control", "no-cache");
      server.sendHeader("Set-Cookie", "ESPSESSIONID=1");
      server.send(301);
      Serial.println("Log in Successful");
      return;
    }
    msg = "Wrong username/password! try again.";
    Serial.println("Log in Failed");
  }
  /*
    String content = "<html><body><form action='/login' method='POST'>Entre el password<br>";
  content += "U2ser:<input type='text' name='USERNAME' placeholder='user name'><br>";
  content += "Password:<input type='password' name='PASSWORD' placeholder='password'><br>";
  content += "<input type='submit' name='SUBMIT' value='Submit'></form>" + msg + "<br>";
  content += "You also can go <a href='/inline'>here</a></body></html>";
*/
  String content = "<html><head><meta http-equiv='refresh' content='30'></head><body><form action='/login' method='POST'>Entre el password<br>";
  content += "Password:<input type='password' name='PASSWORD' placeholder='password'><br>";
  content += "<input type='submit' name='SUBMIT' value='Submit'></form>" + msg + "<br>";
  content += "<svg width='100' height='100' ><circle cx='100' cy='50' r='40' stroke='green' stroke-width='4' fill='yellow' />Sorry, your browser does not support inline SVG.</svg>";
  content += "També pots anar <a href='/inline'>aquí</a></body></html>";
  server.send(200, "text/html", content);
}

//root page can be accessed only if authentication is ok
void handleRoot() {
  Serial.println("Enter handleRoot");
  String header;
  if (!is_authentified()) {
    server.sendHeader("Location", "/login");
    server.sendHeader("Cache-Control", "no-cache");
    server.send(301);
    return;
  }
  String content = "<html><head><style>#vermell {color: red;border: 5px solid currentcolor;}#verd {color: black;border: 5px solid green;}</style></head>";
  content += "<button onclick='window.location.reload();'>Actualizar Pagina</button><body><H2>Hola, estas conectat a la Wifi de gestor de hora NTP de repsol</H2><br>";
/*
  if (server.hasHeader("User-Agent")) {
    content += "the user agent used is : " + server.header("User-Agent") + "<br><br>";
  }
*/
  if (WiFi.status() == WL_CONNECTED){
      content += "<div id='verd'>";
    }
  else {
    content += "<div id='vermell'>";
  }
  content += "IP wifi.........:" + WiFi.localIP().toString() + "</div>";


  if (ETH.linkUp()) {
    content += "<div id='verd'>";
  } else {
    content += "<div id='vermell'>";
  }


  content += "IP Ethernet..:" + ETH.localIP().toString() + "</div>";
  content += "<div id='verd'>Servidor NTP :" + String(ntpServer) + "</div>";
  content += String(tm) + " Podeu accedir a aquesta pagina fins que us <a href=\"/login?DISCONNECT=YES\">desconnecteu</a></body></html>";

  content += "<form>";
  content += "<p>Valor actual: <span id='valor'>Cargando...</span></p><textarea id='serialBox' rows='10' cols='50'>" + serialData + "</textarea>";
  content += "</form>";
  content += "<script>document.getElementById('serialBox').scrollTop = document.getElementById('serialBox').scrollHeight;</script>";
  content += "</body></html>";

  server.send(200, "text/html", content);
}

//no need authentication
void handleNotFound() {
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";



  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
}
