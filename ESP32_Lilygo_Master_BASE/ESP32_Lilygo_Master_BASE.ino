/*
-----------------------------------------------------------------------
PARA CARGAR EN LILYGO T-ETH
PULSAR BOTÓN DE BOOT
PULSAR BOTÓN DE RESET
SOLTAR BOTÓN DE RESET
SOLTAR BOTÓN DE BOOT
*/

#include <ETH.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <Wire.h>

// DATOS DIRECCIÓN IP 
IPAddress ip(10, 82, 103, 217);         // IP                           
IPAddress gateway(10, 82, 103, 209);    // Puerta de enlace             
IPAddress subnet(255, 255, 255, 240);   // Máscara de subred        

// DATOS SERVIDOR NTP
const char* ntpServer = "172.24.147.1";
const long  gmtOffset_sec = 3600;
const int   daylightOffset_sec = 3600;

// DIRECCIONES I2C
const int I2C_SDA = 19;
const int I2C_SCL = 20;
#define DIRECCION_ESCLAVO_8 8
#define DIRECCION_ESCLAVO_9 9
#define DIRECCION_ESCLAVO_10 10
#define DIRECCION_ESCLAVO_11 11

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
    // ENVIAR DATOS A ESCLAVOS
    enviarMensajeEsclavo(DIRECCION_ESCLAVO_8, horaActEnv, minutosActEnv, segundosActEnv, diaActEnv, mesActEnv, anoActEnv);
    enviarMensajeEsclavo(DIRECCION_ESCLAVO_9, horaActEnv, minutosActEnv, segundosActEnv, diaActEnv, mesActEnv, anoActEnv);
    enviarMensajeEsclavo(DIRECCION_ESCLAVO_10, horaActEnv, minutosActEnv, segundosActEnv, diaActEnv, mesActEnv, anoActEnv);
    enviarMensajeEsclavo(DIRECCION_ESCLAVO_11, horaActEnv, minutosActEnv, segundosActEnv, diaActEnv, mesActEnv, anoActEnv);
    
    // IMPRIMIR VALORES
    Serial.println("Error NTP");
    Serial.print("Valor hora: ");
    Serial.println(horaActEnv);    
    Serial.println("---------------------");

    timeClient.forceUpdate();
  }

  // ENVIAR DATOS A CADA ESCLAVO
  enviarMensajeEsclavo(DIRECCION_ESCLAVO_8, horaActEnv, minutosActEnv, segundosActEnv, diaActEnv, mesActEnv, anoActEnv);
  enviarMensajeEsclavo(DIRECCION_ESCLAVO_9, horaActEnv, minutosActEnv, segundosActEnv, diaActEnv, mesActEnv, anoActEnv);
  enviarMensajeEsclavo(DIRECCION_ESCLAVO_10, horaActEnv, minutosActEnv, segundosActEnv, diaActEnv, mesActEnv, anoActEnv);
  enviarMensajeEsclavo(DIRECCION_ESCLAVO_11, horaActEnv, minutosActEnv, segundosActEnv, diaActEnv, mesActEnv, anoActEnv);

  // IMPRIMIR DATOS ENVIADOS
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