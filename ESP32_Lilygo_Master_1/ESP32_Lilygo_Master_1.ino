#include <ETH.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <Wire.h>

// DATOS DIRECCIÓN IP 
IPAddress ip(10, 82, 207, 13);         // IP 
IPAddress gateway(10, 82, 207, 1);    // Puerta de enlace
IPAddress subnet(255, 255, 255, 192);   // Máscara de subred

// DATOS SERVIDOR NTP
const char* ntpServer = "172.24.147.1";
const long  gmtOffset_sec = 3600;
const int   daylightOffset_sec = 3600;

// DIRECCIONES I2C
const int I2C_SDA = 19;
const int I2C_SCL = 20;
// TORNO 1
#define DIRECCION_ESCLAVO_8 8
#define DIRECCION_ESCLAVO_9 9
#define DIRECCION_ESCLAVO_10 10
#define DIRECCION_ESCLAVO_11 11
// TORNO 2
#define DIRECCION_ESCLAVO_12 12
#define DIRECCION_ESCLAVO_13 13
#define DIRECCION_ESCLAVO_14 14
#define DIRECCION_ESCLAVO_15 15
// TORNO 3
#define DIRECCION_ESCLAVO_16 16
#define DIRECCION_ESCLAVO_17 17
#define DIRECCION_ESCLAVO_18 18
#define DIRECCION_ESCLAVO_19 19
// TORNO 4
#define DIRECCION_ESCLAVO_20 20
#define DIRECCION_ESCLAVO_21 21
#define DIRECCION_ESCLAVO_22 22
#define DIRECCION_ESCLAVO_23 23

// DECLARAR VARIABLES, I2C
uint32_t i = 0;

// VARIABLES FECHA Y HORA
String formattedTime;
uint8_t horaActEnv;
uint8_t minutosActEnv;
uint8_t segundosActEnv;
String formattedDate;
uint8_t diaActSem;
uint8_t diaActEnv;
uint8_t mesActEnv;
uint8_t anoActEnv;

// INICIALZAR WiFiUDP y NTPClient
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, ntpServer, gmtOffset_sec, 3600);
bool resultadoUpdate = false;

// FUNCIÓN ENVIAR DATOS A ESCLAVO
void enviarMensajeEsclavo(int direccionEsclavo, uint8_t hora, uint8_t minutos, uint8_t segundos, uint8_t dia, uint8_t mes, uint8_t ano) {
  Wire.beginTransmission(direccionEsclavo);
  Wire.write(hora);
  Wire.write(minutos);
  Wire.write(segundos);
  Wire.write(dia);
  Wire.write(mes);
  Wire.write(ano);
  Wire.endTransmission();
}

void setup() {
  // INCIALIZAR MONITOR SERIE
  Serial.begin(115200);
  Serial.setDebugOutput(true);
  
  // INICIALIZAR I2C
  Wire.begin(I2C_SDA, I2C_SCL);

  // INICIALIZAR ETHERNET
  ETH.begin();
  ETH.config(IPAddress(ip), IPAddress(gateway), IPAddress(subnet));

  // INICIALIZAR CLIENTE NTP
  timeClient.begin();
  timeClient.update();
}

void loop() {
  // ACTUALIZAR HORA NTP
  timeClient.update();
  resultadoUpdate = timeClient.forceUpdate();
  
  // OBTENER HORA, MINUTOS, SEGUNDOS, DÍA, MES Y AÑO
  unsigned long epochTime = timeClient.getEpochTime();
  struct tm *ptm = gmtime((time_t *)&epochTime);

  formattedTime = timeClient.getFormattedTime();
  horaActEnv = timeClient.getHours();
  minutosActEnv = timeClient.getMinutes();
  segundosActEnv = timeClient.getSeconds();
  
  diaActSem = timeClient.getDay();
  diaActEnv = ptm -> tm_mday;
  mesActEnv = ptm -> tm_mon + 1;
  anoActEnv = ptm -> tm_year - 100;

  // ERROR NTP
  if( resultadoUpdate == false ){
    horaActEnv = 255;
    // ENVIAR DATOS A ESCLAVOS TORNO 1
    enviarMensajeEsclavo(DIRECCION_ESCLAVO_8, horaActEnv, minutosActEnv, segundosActEnv, diaActEnv, mesActEnv, anoActEnv);
    enviarMensajeEsclavo(DIRECCION_ESCLAVO_9, horaActEnv, minutosActEnv, segundosActEnv, diaActEnv, mesActEnv, anoActEnv);
    enviarMensajeEsclavo(DIRECCION_ESCLAVO_10, horaActEnv, minutosActEnv, segundosActEnv, diaActEnv, mesActEnv, anoActEnv);
    enviarMensajeEsclavo(DIRECCION_ESCLAVO_11, horaActEnv, minutosActEnv, segundosActEnv, diaActEnv, mesActEnv, anoActEnv);
    // ENVIAR DATOS A ESCLAVOS TORNO 2
    enviarMensajeEsclavo(DIRECCION_ESCLAVO_12, horaActEnv, minutosActEnv, segundosActEnv, diaActEnv, mesActEnv, anoActEnv);
    enviarMensajeEsclavo(DIRECCION_ESCLAVO_13, horaActEnv, minutosActEnv, segundosActEnv, diaActEnv, mesActEnv, anoActEnv);
    enviarMensajeEsclavo(DIRECCION_ESCLAVO_14, horaActEnv, minutosActEnv, segundosActEnv, diaActEnv, mesActEnv, anoActEnv);
    enviarMensajeEsclavo(DIRECCION_ESCLAVO_15, horaActEnv, minutosActEnv, segundosActEnv, diaActEnv, mesActEnv, anoActEnv);
    // ENVIAR DATOS A ESCLAVOS TORNO 3
    enviarMensajeEsclavo(DIRECCION_ESCLAVO_16, horaActEnv, minutosActEnv, segundosActEnv, diaActEnv, mesActEnv, anoActEnv);
    enviarMensajeEsclavo(DIRECCION_ESCLAVO_17, horaActEnv, minutosActEnv, segundosActEnv, diaActEnv, mesActEnv, anoActEnv);
    enviarMensajeEsclavo(DIRECCION_ESCLAVO_18, horaActEnv, minutosActEnv, segundosActEnv, diaActEnv, mesActEnv, anoActEnv);
    enviarMensajeEsclavo(DIRECCION_ESCLAVO_19, horaActEnv, minutosActEnv, segundosActEnv, diaActEnv, mesActEnv, anoActEnv);
    // ENVIAR DATOS A ESCLAVOS TORNO 4
    enviarMensajeEsclavo(DIRECCION_ESCLAVO_20, horaActEnv, minutosActEnv, segundosActEnv, diaActEnv, mesActEnv, anoActEnv);
    enviarMensajeEsclavo(DIRECCION_ESCLAVO_21, horaActEnv, minutosActEnv, segundosActEnv, diaActEnv, mesActEnv, anoActEnv);
    enviarMensajeEsclavo(DIRECCION_ESCLAVO_22, horaActEnv, minutosActEnv, segundosActEnv, diaActEnv, mesActEnv, anoActEnv);
    enviarMensajeEsclavo(DIRECCION_ESCLAVO_23, horaActEnv, minutosActEnv, segundosActEnv, diaActEnv, mesActEnv, anoActEnv);
    
    // IMPRIMIR VALORES MONITOR SERIE
    Serial.println("Error NTP");
    Serial.print("Valor hora: ");
    Serial.println(horaActEnv);    
    Serial.println("---------------------");

    timeClient.forceUpdate();
  }

  // ENVIAR DATOS A ESCLAVOS TORNO 1
  enviarMensajeEsclavo(DIRECCION_ESCLAVO_8, horaActEnv, minutosActEnv, segundosActEnv, diaActEnv, mesActEnv, anoActEnv);
  enviarMensajeEsclavo(DIRECCION_ESCLAVO_9, horaActEnv, minutosActEnv, segundosActEnv, diaActEnv, mesActEnv, anoActEnv);
  enviarMensajeEsclavo(DIRECCION_ESCLAVO_10, horaActEnv, minutosActEnv, segundosActEnv, diaActEnv, mesActEnv, anoActEnv);
  enviarMensajeEsclavo(DIRECCION_ESCLAVO_11, horaActEnv, minutosActEnv, segundosActEnv, diaActEnv, mesActEnv, anoActEnv);
  // ENVIAR DATOS A ESCLAVOS TORNO 2
  enviarMensajeEsclavo(DIRECCION_ESCLAVO_12, horaActEnv, minutosActEnv, segundosActEnv, diaActEnv, mesActEnv, anoActEnv);
  enviarMensajeEsclavo(DIRECCION_ESCLAVO_13, horaActEnv, minutosActEnv, segundosActEnv, diaActEnv, mesActEnv, anoActEnv);
  enviarMensajeEsclavo(DIRECCION_ESCLAVO_14, horaActEnv, minutosActEnv, segundosActEnv, diaActEnv, mesActEnv, anoActEnv);
  enviarMensajeEsclavo(DIRECCION_ESCLAVO_15, horaActEnv, minutosActEnv, segundosActEnv, diaActEnv, mesActEnv, anoActEnv);
  // ENVIAR DATOS A ESCLAVOS TORNO 3
  enviarMensajeEsclavo(DIRECCION_ESCLAVO_16, horaActEnv, minutosActEnv, segundosActEnv, diaActEnv, mesActEnv, anoActEnv);
  enviarMensajeEsclavo(DIRECCION_ESCLAVO_17, horaActEnv, minutosActEnv, segundosActEnv, diaActEnv, mesActEnv, anoActEnv);
  enviarMensajeEsclavo(DIRECCION_ESCLAVO_18, horaActEnv, minutosActEnv, segundosActEnv, diaActEnv, mesActEnv, anoActEnv);
  enviarMensajeEsclavo(DIRECCION_ESCLAVO_19, horaActEnv, minutosActEnv, segundosActEnv, diaActEnv, mesActEnv, anoActEnv);
  // ENVIAR DATOS A ESCLAVOS TORNO 4
  enviarMensajeEsclavo(DIRECCION_ESCLAVO_20, horaActEnv, minutosActEnv, segundosActEnv, diaActEnv, mesActEnv, anoActEnv);
  enviarMensajeEsclavo(DIRECCION_ESCLAVO_21, horaActEnv, minutosActEnv, segundosActEnv, diaActEnv, mesActEnv, anoActEnv);
  enviarMensajeEsclavo(DIRECCION_ESCLAVO_22, horaActEnv, minutosActEnv, segundosActEnv, diaActEnv, mesActEnv, anoActEnv);
  enviarMensajeEsclavo(DIRECCION_ESCLAVO_23, horaActEnv, minutosActEnv, segundosActEnv, diaActEnv, mesActEnv, anoActEnv);

  // IMPRIMIR DATOS ENVIADOS MONITOR SERIE
  Serial.print("Hora actual: ");
  Serial.println(formattedTime);
  Serial.print("Horas actuales: "); 
  Serial.println(horaActEnv);
  Serial.print("Minutos actuales: ");  
  Serial.println(minutosActEnv);
  Serial.print("Segundos actuales: ");  
  Serial.println(segundosActEnv);

  Serial.print("Fecha actual: ");
  Serial.print(diaActEnv);
  Serial.print("/");
  Serial.print(mesActEnv);
  Serial.print("/20");
  Serial.println(anoActEnv);
  Serial.print("Día de la semana: ");
  Serial.println(diaActSem);
  Serial.print("Dia actual: ");  
  Serial.println(diaActEnv);
  Serial.print("Mes actual: ");  
  Serial.println(mesActEnv);
  Serial.print("Año actual: ");  
  Serial.print("20");
  Serial.println(anoActEnv);
  Serial.println("---------------------");

  // EVITAR REBOTES
  delay(1000);
}