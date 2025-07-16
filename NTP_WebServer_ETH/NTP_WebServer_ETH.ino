#include <Arduino.h>
#include <ETH.h>
#include <SPI.h>
#include "utilities.h"
#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiServer.h>
#include <NTPClient.h>
#include <TimeLib.h>
#include <WiFiUdp.h>
#include <Timezone.h>       // Nueva librería para zonas horarias

// IP Fija
IPAddress staticIP(10, 82, 103, 215);
IPAddress gateway(10, 82, 103, 209);
IPAddress subnet(255, 255, 255, 240);
IPAddress dns(172, 16, 138, 119);
int data = 0;

WiFiUDP ntpUDP;
NTPClient Hora_NTP(ntpUDP, "172.18.155.2", 0, 60000);

bool eth_connected = false;
WiFiServer server(80);

char tm[64];

// *** Estado de estaciones ***
// Inicializamos todas como desconectadas (false)
bool estacionesConectadas[21] = { false, true, false, true, true, false, false, false, false, false, false, false, false, false, true, false, true, false, false, false };
char* Nom_Torn[21] = { "Estacio_0", "Estacio_1", "Estacio_2", "Estacio_3", "Estacio_4", "Estacio_5",
                       "Estacio_6", "Estacio_7", "Estacio_8", "Estacio_9", "Estacio_10", "Estacio_11",
                       "Estacio_12", "Estacio_13", "Estacio_14", "Estacio_15", "Estacio_16", "Estacio_17", "Estacio_18", "Estacio_19", "Estacio_20" };
// Define reglas para horario verano/invierno Madrid
TimeChangeRule CEST = {"CEST", Last, Sun, Mar, 2, 120};  // Horario verano UTC+2
TimeChangeRule CET  = {"CET",  Last, Sun, Oct, 3, 60};   // Horario invierno UTC+1
Timezone Madrid(CEST, CET);

void setup() {
  Serial.begin(115200);
  delay(1000);

#ifdef ETH_POWER_PIN
  pinMode(ETH_POWER_PIN, OUTPUT);
  digitalWrite(ETH_POWER_PIN, HIGH);
#endif

  WiFi.onEvent(WiFiEvent);

  if (!ETH.begin(ETH_PHY_W5500, 1, ETH_CS_PIN, ETH_INT_PIN, ETH_RST_PIN,
                 SPI3_HOST, ETH_SCLK_PIN, ETH_MISO_PIN, ETH_MOSI_PIN)) {
    Serial.println("ETH start Failed!");
  }

  if (!ETH.config(staticIP, gateway, subnet, dns)) {
    Serial.println("❌ Falló configuración IP estática.");
  } else {
    Serial.println("✅ IP estática configurada.");
  }

  Hora_NTP.begin();
  server.begin();  // Iniciar servidor web
  Serial.println("🌐 Servidor web iniciado en puerto 80.");
}

void loop() {
  Hora_NTP.update();
  time_t utc = Hora_NTP.getEpochTime();
  // Convertir UTC a hora local Madrid (con horario verano/invierno)
  time_t horaLocalMadrid = Madrid.toLocal(utc);
  setTime(horaLocalMadrid);  // Actualiza TimeLib con hora local

  WiFiClient client = server.available();
  if (client) {
    //Serial.println("🌐 Nuevo cliente conectado");
    String req = client.readStringUntil('\r');
    client.flush();
    if (req.indexOf("GET /accion") >= 0) {
      // Aquí haces lo que quieras cuando se presione el botón
      Serial.println("🔘 Botón presionado");
      Comprobar_comunicaciones_I2C();
    }
    // Ruta AJAX para hora
    if (req.indexOf("GET /hora") >= 0) {
      handleAjaxHora(client);
    } else {
      // Página web completa con tabla de estaciones
      client.println("HTTP/1.1 200 OK");
      client.println("Content-Type: text/html; charset=utf-8");
      client.println();
      client.println("<!DOCTYPE html><html><head><meta charset='utf-8'>");
      client.println("<title>Servidor NTP y Estado de Estaciones</title>");

      client.println("<script>");
      client.println("function actualizarHora() {");
      client.println("  fetch('/hora')");
      client.println("    .then(r => r.text())");
      client.println("    .then(t => { document.getElementById('hora').innerHTML = t; });");
      client.println("  setTimeout(actualizarHora, 1000);");
      client.println("}");
      client.println("window.onload = actualizarHora;");
      client.println("</script>");

      client.println("</head><body>");
      client.println("<img src='data:image/jpeg;base64,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' alt='Repsol Logo' width='200'><br>");
      client.println("<h1>Servidor Horario NTP <br>con WebServerWeb ESP32ETH)</h1>");
      if (eth_connected) {
        client.print("<p><strong>Estado Ethernet:</strong> Conectado ✅</p>");
        client.print("<p><strong>IP:</strong> ");
        client.print(ETH.localIP());
        client.println("</p>");
      } else {
        client.println("<p><strong>Estado Ethernet:</strong> Desconectado ❌</p>");
      }
      client.println("<p><strong>Hora NTP:</strong> <span id='hora'>Cargando...</span></p>");
      // Tabla de estaciones
      client.println("<h1>Estado de las Estaciones</h2>");

      client.println("<div style='display: flex; gap: 40px;'>");
      // primer grupo de 1 al 10
      client.println("<table border='1' style='border-collapse: collapse;'>");
      client.println("<tr><th>ID</th><th>Nombre</th><th>Estado</th></tr>");
      for (int i = 1; i <= 10; i++) {
        client.print("<tr><td>");
        client.print(i);
        client.print("</td><td> ");
        client.print(Nom_Torn[i]);
        client.print("</td><td style='text-align: center;'>");
        if (estacionesConectadas[i] == true) {
          client.print("✅");
        } else {
          client.print("❌");
        }
        client.print("</td></tr>");
      }
      client.println("</table>");

      // segon grup del 11-20
      client.println("<table border='1' style='border-collapse: collapse;'>");
      client.println("<tr><th>ID</th><th>Nombre</th><th>Estado</th></tr>");
      for (int i = 11; i <= 20; i++) {
        client.print("<tr><td>");
        client.print(i);
        client.print("</td><td> ");
        client.print(Nom_Torn[i]);
        client.print("</td><td style='text-align: center;'>");
        if (estacionesConectadas[i] == true) {
          client.print("✅");
        } else {
          client.print("❌");
        }
        client.print("</td></tr>");
      }
      client.println("</table>");
      client.println("</div>");
      client.println("<form action=\"/accion\" method=\"GET\">");
      client.println("<button type='submit' style='padding:10px 20px; font-size:16px; background-color:#007BFF; color:white; border:none; border-radius:5px;'> Comprobar comunicaciones estaciones </button>");

      client.println("</body></html>");
    }

    client.println("</form>");
    opcions_teclat();
    client.stop();
    //Serial.println("❎ Cliente desconectado");
  }

  delay(10);  // No bloquear, responder rápido a AJAX
}

void handleAjaxHora(WiFiClient& client) {
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/plain; charset=utf-8");
  client.println("Connection: close");
  client.println();

  snprintf(tm, sizeof(tm), "%04d-%02d-%02d %02d:%02d:%02d",
           year(), month(), day(), hour(), minute(), second());          
  client.println(tm);
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
void Comprobar_comunicaciones_I2C(){
  // Recorrer todas las estaciones I2C para comprobar la counicación
}
void opcions_teclat() {
  if (Serial.available() > 0) {
    data = Serial.read();
    //Serial.println(data, DEC);
    if (data == 'S') {
      estacionesConectadas[3]=true;
      delay(200);
    }
    if (data == 'L') {
      estacionesConectadas[3]=false;
      delay(1000);
    }
    if (data == '-') {  // resta 1 hora
      delay(200);
    }
    if (data == 'U') {  // actualiza hora
      delay(200);
    }
  }
}