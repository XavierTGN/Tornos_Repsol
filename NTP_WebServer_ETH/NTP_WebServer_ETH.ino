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

// IP Fija
IPAddress staticIP(10, 82, 103, 215);
IPAddress gateway(10, 82, 103, 209);
IPAddress subnet(255, 255, 255, 240);
IPAddress dns(172, 16, 138, 119);

WiFiUDP ntpUDP;
NTPClient Hora_NTP(ntpUDP, "172.18.155.2", 3600, 60000);

bool eth_connected = false;
WiFiServer server(80);

unsigned long t_unix_date1;
char tm[64];

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
  t_unix_date1 = Hora_NTP.getEpochTime();

  WiFiClient client = server.available();
  if (client) {
    Serial.println("🌐 Nuevo cliente conectado");

    String req = client.readStringUntil('\r');
    client.flush();

    // Ruta AJAX
    if (req.indexOf("GET /hora") >= 0) {
      handleAjaxHora(client);
    } else {
      // Página web completa
      client.println("HTTP/1.1 200 OK");
      client.println("Content-Type: text/html; charset=utf-8");
      client.println();
      client.println("<!DOCTYPE html><html><head><meta charset='utf-8'>");
      client.println("<title>ESP32 ETH</title>");
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
      client.println("<h1>Servidor Web ESP32 (ETH)</h1>");

      if (eth_connected) {
        client.print("<p><strong>Estado:</strong> Conectado ✅</p>");
        client.print("<p><strong>IP:</strong> ");
        client.print(ETH.localIP());
        client.println("</p>");
      } else {
        client.println("<p><strong>Estado:</strong> Desconectado ❌</p>");
      }

      client.println("<p><strong>Hora NTP:</strong> <span id='hora'>Cargando...</span></p>");
      client.println("</body></html>");
    }

    client.stop();
    Serial.println("❎ Cliente desconectado");
  }

  delay(10);  // No bloquear, responder rápido a AJAX
}

void handleAjaxHora(WiFiClient &client) {
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/plain; charset=utf-8");
  client.println("Connection: close");
  client.println();

  sprintf(tm, "%04d-%02d-%02d %02d:%02d:%02d",
          year(t_unix_date1), month(t_unix_date1), day(t_unix_date1),
          hour(t_unix_date1), minute(t_unix_date1), second(t_unix_date1));
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
