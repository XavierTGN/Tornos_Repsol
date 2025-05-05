#include <WiFi.h>
#include <WebServer.h>

char *ssid = "MOVISTAR-WIFI6-6810";
char *password = "N9HEPszqVUT93Js79xqs";

WebServer server(80);

// Estados de los círculos
bool estados[4] = {false, false, false, false};

// HTML con actualización automática
const char htmlPage[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
    <title>ESP32 Web Server</title>
    <script>
        function cambiarEstado(id) {
            fetch("/toggle?id=" + id)
            .then(response => response.text())
            .then(data => {
                document.getElementById("circle" + id).setAttribute("fill", data == "1" ? "green" : "red");
            });
        }

        function actualizarEstados() {
            fetch("/status")
            .then(response => response.json())
            .then(data => {
                for (let i = 0; i < 4; i++) {
                    document.getElementById("circle" + i).setAttribute("fill", data[i] == "1" ? "green" : "red");
                }
            });
        }

        setInterval(actualizarEstados, 2000); // Actualiza cada 2 segundos
    </script>
</head>
<body>
    <h1>Control de Círculos</h1>
    <svg width="200" height="200">
        <circle id="circle0" cx="50" cy="50" r="20" fill="red" />
        <circle id="circle1" cx="150" cy="50" r="20" fill="red" />
        <circle id="circle2" cx="50" cy="150" r="20" fill="red" />
        <circle id="circle3" cx="150" cy="150" r="20" fill="red" />
    </svg>
    <br/>
    <button onclick="cambiarEstado(0)">Botón 1</button>
    <button onclick="cambiarEstado(1)">Botón 2</button>
    <button onclick="cambiarEstado(2)">Botón 3</button>
    <button onclick="cambiarEstado(3)">Botón 4</button>
</body>
</html>
)rawliteral";

// Manejador para actualizar el estado desde Arduino
void handleToggle() {
    if (server.hasArg("id")) {
        int id = server.arg("id").toInt();
        if (id >= 0 && id < 4) {
            estados[id] = !estados[id];
            server.send(200, "text/plain", String(estados[id]));
        }
    }
}

// Manejador para devolver el estado actual de los círculos
void handleStatus() {
    String json = "[";
    for (int i = 0; i < 4; i++) {
        json += String(estados[i] ? "1" : "0");
        if (i < 3) json += ",";
    }
    json += "]";
    server.send(200, "application/json", json);
}

void handleRoot() {
    server.send(200, "text/html", htmlPage);
}

void setup() {
    Serial.begin(115200);
    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.println("Conectando...");
    }

    Serial.println("Conectado a WiFi");

    server.on("/", handleRoot);
    server.on("/toggle", handleToggle);
    server.on("/status", handleStatus); // Nueva ruta para estado de los círculos

    server.begin();
}

void loop() {
    server.handleClient();

    // Ejemplo: Cambiar el estado de un círculo automáticamente cada 5 segundos
    static unsigned long lastChange = 0;
    if (millis() - lastChange > 5000) {
        estados[0] = !estados[0]; // Alternar círculo 0 como prueba
        lastChange = millis();
    }
}

