#include "SPI.h"                              // Comunicación SPI, ESP32 - TFT
#include <Adafruit_ILI9341.h>                 // Pantalla ILI9341 TFT
#include "dragon.h"                           // Logo Repsol
#include <Wire.h>                             // Comunicación I2C
#include <Fonts/FreeMono9pt7b.h>              // Fuente Mono9pt7b
#include <Fonts/FreeMonoBold24pt7b.h>         // Fuente MonoBold24pt7b
#include <Fonts/FreeMonoBold18pt7b.h>         // Fuente MonoBold18pt7b
#include <Fonts/FreeMonoBold12pt7b.h>         // Fuente MonoBold12pt7b

// DECLARAR VARIABLES, ENTRADAS LNL-1200
bool IN_1, IN_2, IN_3, IN_4, IN_5, IN_6;

// DEFINIR PINES, PANTALLA TFT
#define TFT_CS 5
#define TFT_RST -1
#define TFT_DC 6
#define TFT_MOSI 7
#define TFT_SCK 15
#define TFT_MISO 16
#define TFT_BL 17

// DEFINIR PINES, ENTRADAS LNL-1200
#define PIN_IN_1 42  //Acceso concedido
#define PIN_IN_2 41  //Acceso denegado
#define PIN_IN_3 40  //Matrícula no reconocida. Consulte a seguridad
#define PIN_IN_4 39  //Fuera de servicio
#define PIN_IN_5 38  //Error. Imparidad fichaje
#define PIN_IN_6 45  //Error. Consulte a seguridad

// DEFINIR DIRECCIÓN I2C ESCLAVO
#define DIRECCION_ESCLAVO 11
const int I2C_SDA = 8;
const int I2C_SCL = 9;

// DECLARAR VARIABLES, I2C
uint32_t i = 0;
int wireAvailable;

// VARIABLES FECHA Y HORA
uint8_t horaActRec;
uint8_t horaAnt;
uint8_t minutosActRec;
uint8_t minutosAnt;
uint8_t segundosActRec;
uint8_t segundosAnt;
uint8_t diaActRec;
uint8_t diaAnt;
uint8_t mesActRec;
uint8_t mesAnt;
uint8_t anoActRec;
uint8_t anoAnt;
uint8_t segundosAntI2C;

// INCIALIZAR PANTALLA 
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCK, TFT_RST, TFT_MISO);

void setup() {
  // INICIALIZAR MONITOR SERIE
  Serial.begin(115200);
  Serial.setDebugOutput(true);

  // INICIALIZAR DE ENTRADAS DIGITALES
  pinMode(PIN_IN_1, INPUT);
  pinMode(PIN_IN_2, INPUT);
  pinMode(PIN_IN_3, INPUT);
  pinMode(PIN_IN_4, INPUT);
  pinMode(PIN_IN_5, INPUT);
  pinMode(PIN_IN_6, INPUT);

  // INICIALIZAR COMUNICACIÓN I2C
  Wire.onReceive(onReceive);
  Wire.begin((uint8_t)DIRECCION_ESCLAVO);

  // INICIALIZAR PANTALLA TFT
  tft.begin();
  tft.setRotation(3);
  tft.fillScreen(ILI9341_WHITE);

  // INICIALIZAR VARIABLES FECHA Y HORA
  horaActRec = 0;
  minutosActRec = 0;
  segundosActRec = 0;
  diaActRec = 0;
  mesActRec = 0;
  anoActRec = 0;
  segundosAntI2C = 0;
}

void loop(void) {
  // VARIABLES LEEN ESTADO DE LAS ENTRADAS IN_X
  IN_1 = digitalRead(PIN_IN_1);
  IN_2 = digitalRead(PIN_IN_2);
  IN_3 = digitalRead(PIN_IN_3);
  IN_4 = digitalRead(PIN_IN_4);
  IN_5 = digitalRead(PIN_IN_5);
  IN_6 = digitalRead(PIN_IN_6);

  // IMPRIMIR FECHA Y HORA
  Serial.print("Valor I2C: ");
  Serial.println(wireAvailable);
  Serial.print("Hora actual: ");
  Serial.println(horaActRec);
  Serial.print("Minutos actual: ");
  Serial.println(minutosActRec);
  Serial.print("Segundos actual: ");
  Serial.println(segundosActRec);
  Serial.print("Dia actual: ");
  Serial.println(diaActRec);
  Serial.print("Mes actual: ");
  Serial.println(mesActRec);
  Serial.print("Año actual: ");
  Serial.println(anoActRec);
  Serial.println("------------------");

  // LIMPIAR PANTALLA CUANDO CAMBIE EL DÍA
  if ((diaActRec != diaAnt) || (mesActRec != mesAnt) || (anoActRec != anoAnt) ){
    tft.fillScreen(ILI9341_WHITE); 

    diaAnt = diaActRec;
    mesAnt = mesActRec;
    anoAnt = anoActRec;
  }

  // LIMPIAR PANTALLA A LAS XX:17
  if ( (minutosActRec == 17 && segundosActRec == 0) ){
    tft.fillScreen(ILI9341_WHITE); 
  }
  // LIMPIAR PANTALLA A LAS XX:27
  if ( (minutosActRec == 27 && segundosActRec == 0) ){
    tft.fillScreen(ILI9341_WHITE); 
  }
  // LIMPIAR PANTALLA A LAS XX:37
  if ( (minutosActRec == 37 && segundosActRec == 0) ){
    tft.fillScreen(ILI9341_WHITE); 
  }
  // LIMPIAR PANTALLA A LAS XX:47
  if ( (minutosActRec == 47 && segundosActRec == 0) ){
    tft.fillScreen(ILI9341_WHITE); 
  }

  // ERROR NTP
  while (horaActRec == 255 || diaActRec == 0){
    mensajeErrorNTP();
    delay(20000);
    tft.fillScreen(ILI9341_WHITE);
  }
  
  // ERROR BUS I2C
  if ( Wire.available() == 0 ){
    mensajeHora();
    segundosAntI2C = segundosActRec;
    delay(3000);
    while ( (Wire.available() == 0) && (segundosAntI2C == segundosActRec) ){
      mensajeErrorI2C();
      wireAvailable = Wire.available();
      Serial.print("Valor I2C: ");
      Serial.println(wireAvailable);
      delay(10000);
      tft.fillScreen(ILI9341_WHITE);
    }
  }

  // MOSTRAR PANTALLA_0: MOSTRAR HORA
  mensajeHora();
  
  // MOSTRAR PANTALLA_1: ACCESO CONCEDIDO
  if (IN_1 == HIGH) {
    tft.fillScreen(ILI9341_WHITE);
    mensajeAccesoConcedido();
    delay(3000);
    tft.fillScreen(ILI9341_WHITE);
  }
  // MOSTRAR PANTALLA_2: ACCESO DENEGADO
  if (IN_2 == HIGH) {
    tft.fillScreen(ILI9341_WHITE);
    mensajeAccesoDenegado();
    delay(3000);
    tft.fillScreen(ILI9341_WHITE);
  }
  // MOSTRAR PANTALLA_3: TORNO BLOQUEADO. APROXÍMESE DE NUEVO
  if (IN_3 == HIGH) {
    tft.fillScreen(ILI9341_WHITE);
    mensajeTornoBloqueado();
    delay(3000);
    tft.fillScreen(ILI9341_WHITE);
  }
  // MOSTRAR PANTALLA_4: FUERA DE SERVICIO
  if (IN_4 == HIGH) {
    tft.fillScreen(ILI9341_WHITE);
    mensajeFueraDeServicio();
    delay(3000);
    tft.fillScreen(ILI9341_WHITE);
  }
  // MOSTRAR PANTALLA_5: ERROR. IMPARIDAD FICHAJE
  if (IN_5 == HIGH) {
    tft.fillScreen(ILI9341_WHITE);
    mensajeErrorImparidadFichaje();
    delay(3000);
    tft.fillScreen(ILI9341_WHITE);
  }
  // MOSTRAR PANTALLA_6: ERROR. CONSULTE A SEGURIDAD
  if (IN_6 == HIGH) {
    tft.fillScreen(ILI9341_WHITE);
    mensajeErrorConsulteASeguridad();
    delay(3000);
    tft.fillScreen(ILI9341_WHITE);
  }
  
  // ACTUALIZAR HORA EN VARIABLES
  if ( (horaActRec != horaAnt) || (minutosActRec != minutosAnt) ){
    tft.fillRect(20, 65, 300, 65, ILI9341_WHITE);
    
    horaAnt = horaActRec;
    minutosAnt = minutosActRec;

  }

  // EVITAR REBOTES
  delay(1000);
}

/* FUNCIONES MOSTRAR MENSAJE POR PANTALLA:
    0- mensajeHora                    -> Se muestra la hora
    1- mensajeAccesoConcedido         -> Se muestra el mensaje de: Acceso concedido 
    2- mensajeAccesoDenegado          -> Se muestra el mensaje de: Acceso denegado
    3- mensajeTornoBloqueado          -> Se muestra el mensaje de: Torno bloqueado. Aproxímese de nuevo
    4- mensajeFueraDeServicio         -> Se muestra el mensaje de: Fuera de servicio
    5- mensajeErrorImparidadFichaje   -> Se muestra el mensaje de: Error, imparidad de fichaje
    6- mensajeErrorConsulteASeguridad -> Se muestra el mensaje de: Error, consulte a seguridad
*/
void mensajeHora() {
  //TFT PARTE SUPERIOR
  parteSuperiorTFT();

  //TFT PARTE CENTRAL
  tft.setTextSize(5);
  tft.setTextColor(ILI9341_BLACK);
  tft.setCursor(15, 105);

  // IMPRIMIR HORA PARA: HORAS > 9 Y MINUTOS > 9
  if ( (horaActRec > 9) && (minutosActRec > 9) ){
    // LIMPIAR PANTALLA CUANDO CAMBIE LA HORA
    if ( (horaActRec != horaAnt) || (minutosActRec != minutosAnt) ){
      tft.fillRect(20, 45, 265, 65, ILI9341_WHITE);
    
      horaAnt = horaActRec;
      minutosAnt = minutosActRec;

    }
    tft.printf("%d:%d", horaActRec, minutosActRec);
  }
  // IMPRIMIR HORA PARA: HORAS > 9 Y MINUTOS <= 9 
  if ( (horaActRec > 9) && (minutosActRec <= 9) ){
    // LIMPIAR PANTALLA CUANDO CAMBIE LA HORA
    if ( (horaActRec != horaAnt) || (minutosActRec != minutosAnt) ){
      tft.fillRect(20, 45, 265, 65, ILI9341_WHITE);
    
      horaAnt = horaActRec;
      minutosAnt = minutosActRec;

    }
    tft.printf("%d:0%d", horaActRec, minutosActRec);
  }
  // IMPRIMIR HORA PARA: HORAS <= 9 Y MINUTOS <= 9 
  if ( (horaActRec <= 9) && (minutosActRec <= 9) ){
    // LIMPIAR PANTALLA CUANDO CAMBIE LA HORA
    if ( (horaActRec != horaAnt) || (minutosActRec != minutosAnt) ){
      tft.fillRect(20, 45, 265, 65, ILI9341_WHITE);
    
      horaAnt = horaActRec;
      minutosAnt = minutosActRec;

    }
    tft.printf("0%d:0%d", horaActRec, minutosActRec);
  }
  // IMPRIMIR HORA PARA: HORAS <= 9 Y MINUTOS > 9 
  if ( (horaActRec <= 9) && (minutosActRec > 9) ){
    // LIMPIAR PANTALLA CUANDO CAMBIE LA HORA
    if ( (horaActRec != horaAnt) || (minutosActRec != minutosAnt) ){
      tft.fillRect(20, 45, 265, 65, ILI9341_WHITE);
    
      horaAnt = horaActRec;
      minutosAnt = minutosActRec;

    }
    tft.printf("0%d:%d", horaActRec, minutosActRec);
  }

  //TFT PARTE INFERIOR
  tft.setTextSize(0);
  parteInferiorTFT();
}
void mensajeAccesoConcedido() {
  //TFT LIMPIAR PANTALLA ANTERIOR
  tft.fillScreen(ILI9341_WHITE);
  
  //TFT PARTE SUPERIOR
  parteSuperiorTFT();

  //TFT PARTE CENTRAL
  tft.setFont(&FreeMonoBold24pt7b);
  tft.setTextColor(ILI9341_BLACK);
  tft.setCursor(75, 80);
  tft.printf("ACCESO");

  tft.setFont(&FreeMonoBold24pt7b);
  tft.setTextColor(ILI9341_BLACK);
  tft.setCursor(35, 110);
  tft.printf("CONCEDIDO");
}
void mensajeAccesoDenegado() {
  //TFT LIMPIAR PANTALLA ANTERIOR
  tft.fillScreen(ILI9341_WHITE);
  
  //TFT PARTE SUPERIOR
  parteSuperiorTFT();

  //TFT PARTE CENTRAL
  tft.setFont(&FreeMonoBold24pt7b);
  tft.setTextColor(ILI9341_BLACK);
  tft.setCursor(75, 80);
  tft.printf("ACCESO");

  tft.setFont(&FreeMonoBold24pt7b);
  tft.setTextColor(ILI9341_BLACK);
  tft.setCursor(45, 110);
  tft.printf("DENEGADO");
}
void mensajeTornoBloqueado() {
  //TFT LIMPIAR PANTALLA ANTERIOR
  tft.fillScreen(ILI9341_WHITE);
  
  //TFT PARTE SUPERIOR
  parteSuperiorTFT();

  //TFT PARTE CENTRAL
  tft.setFont(&FreeMonoBold12pt7b);
  tft.setTextColor(ILI9341_BLACK);
  tft.setCursor(50, 80);
  tft.printf("TORNO BLOQUEADO");

  tft.setFont(&FreeMonoBold12pt7b);
  tft.setTextColor(ILI9341_BLACK);
  tft.setCursor(30, 110);
  tft.printf("APROXIMESE DE NUEVO");
}
void mensajeFueraDeServicio() {
  //TFT LIMPIAR PANTALLA ANTERIOR
  tft.fillScreen(ILI9341_WHITE);
  
  //TFT PARTE SUPERIOR
  parteSuperiorTFT();

  //TFT PARTE CENTRAL
  tft.setFont(&FreeMonoBold18pt7b);
  tft.setTextColor(ILI9341_BLACK);
  tft.setCursor(105, 80);
  tft.printf("FUERA");

  tft.setFont(&FreeMonoBold18pt7b);
  tft.setTextColor(ILI9341_BLACK);
  tft.setCursor(40, 110);
  tft.printf("DE SERVICIO");
}
void mensajeErrorImparidadFichaje() {
  //TFT LIMPIAR PANTALLA ANTERIOR
  tft.fillScreen(ILI9341_WHITE);
  
  //TFT PARTE SUPERIOR
  parteSuperiorTFT();

  //TFT PARTE CENTRAL
  tft.setFont(&FreeMonoBold18pt7b);
  tft.setTextColor(ILI9341_BLACK);
  tft.setCursor(110, 70);
  tft.printf("ERROR");

  tft.setFont(&FreeMonoBold18pt7b);
  tft.setTextColor(ILI9341_BLACK);
  tft.setCursor(63, 100);
  tft.printf("IMPARIDAD");

  tft.setFont(&FreeMonoBold18pt7b);
  tft.setTextColor(ILI9341_BLACK);
  tft.setCursor(53, 130);
  tft.printf("DE FICHAJE");
}
void mensajeErrorConsulteASeguridad() {
  //TFT LIMPIAR PANTALLA ANTERIOR
  tft.fillScreen(ILI9341_WHITE);
  
  //TFT PARTE SUPERIOR
  parteSuperiorTFT();

  //TFT PARTE CENTRAL
  tft.setFont(&FreeMonoBold18pt7b);
  tft.setTextColor(ILI9341_BLACK);
  tft.setCursor(110, 70);
  tft.printf("ERROR");

  tft.setFont(&FreeMonoBold18pt7b);
  tft.setTextColor(ILI9341_BLACK);
  tft.setCursor(80, 100);
  tft.printf("CONSULTE");

  tft.setFont(&FreeMonoBold18pt7b);
  tft.setTextColor(ILI9341_BLACK);
  tft.setCursor(50, 130);
  tft.printf("A SEGURIDAD");
}
// MENSAJE ERROR NTP
void mensajeErrorNTP() { 
  //TFT LIMPIAR PANTALLA ANTERIOR
  tft.fillScreen(ILI9341_WHITE);

  //TFT PARTE CENTRAL
  tft.setFont(&FreeMonoBold18pt7b);
  tft.setTextColor(ILI9341_BLACK);
  tft.setCursor(70, 70);
  tft.printf("AVISAR A");

  tft.setFont(&FreeMonoBold18pt7b);
  tft.setTextColor(ILI9341_BLACK);
  tft.setCursor(40, 100);
  tft.printf("ELECTRONICA");

  tft.setFont(&FreeMono9pt7b);
  tft.setTextColor(ILI9341_BLACK);
  tft.setCursor(55, 125);
  tft.printf("ERROR SERVIDOR NTP");

  /*
  // PARTE INFERIOR
  // IMPRIMIR LOGO REPSOL
  //mostrarLogoRepsol();

  // IMPRIMIR SEPARADORES NARANJAS
  tft.drawLine(0, 224, 320, 224, ILI9341_ORANGE);
  tft.drawLine(0, 225, 320, 225, ILI9341_ORANGE);

  // IMPRIMIR "CIT"
  tft.setFont(&FreeMono9pt7b);
  tft.setTextColor(ILI9341_BLACK);
  tft.setCursor(16, 237);
  tft.printf("CIT");

  // IMPRIMIR "TALLER ELECTRÓNICA"
  tft.setCursor(150, 237);
  tft.setTextColor(ILI9341_BLACK);
  tft.printf("T. ELECTRONICA");
  */
}
// MENSAJE ERROR BUS I2C
void mensajeErrorI2C() {
  //TFT LIMPIAR PANTALLA ANTERIOR
  tft.fillScreen(ILI9341_WHITE);

  //TFT PARTE CENTRAL
  tft.setFont(&FreeMonoBold18pt7b);
  tft.setTextColor(ILI9341_BLACK);
  tft.setCursor(70, 70);
  tft.printf("AVISAR A");

  tft.setFont(&FreeMonoBold18pt7b);
  tft.setTextColor(ILI9341_BLACK);
  tft.setCursor(40, 100);
  tft.printf("ELECTRONICA");

  tft.setFont(&FreeMono9pt7b);
  tft.setTextColor(ILI9341_BLACK);
  tft.setCursor(80, 130);
  tft.printf("ERROR BUS I2C");

  /*
  // PARTE INFERIOR
  // IMPRIMIR LOGO REPSOL
  mostrarLogoRepsol();

  // IMPRIMIR SEPARADORES NARANJAS
  tft.drawLine(0, 224, 320, 224, ILI9341_ORANGE);
  tft.drawLine(0, 225, 320, 225, ILI9341_ORANGE);

  // IMPRIMIR "CIT"
  tft.setFont(&FreeMono9pt7b);
  tft.setTextColor(ILI9341_BLACK);
  tft.setCursor(16, 237);
  tft.printf("CIT");

  // IMPRIMIR "TALLER ELECTRÓNICA"
  tft.setCursor(150, 237);
  tft.setTextColor(ILI9341_BLACK);
  tft.printf("T. ELECTRONICA");
  */
}
// PARTE SUPERIOR PANTALA
void parteSuperiorTFT(){
  // IMPRIMIR FECHA
  tft.setFont(&FreeMono9pt7b);
  tft.setTextColor(ILI9341_BLACK);
  tft.setCursor(10, 40);
  tft.printf("%d/%d/20%d", diaActRec, mesActRec, anoActRec);
  tft.setCursor(145, 40);
  tft.printf("C.I. TARRAGONA");

  // IMPRIMIR SEPARADORES NARANJAS
  tft.drawLine(0, 43, 320, 43, ILI9341_ORANGE);
  tft.drawLine(0, 44, 320, 44, ILI9341_ORANGE);
  
  // REINICIAR TAMAÑO FUENTES
  tft.setTextSize(0);
}
// PARTE INFERIOR PANTALLA
void parteInferiorTFT(){
  // IMPRIMIR LOGO REPSOL
  mostrarLogoRepsol();

  // IMPRIMIR SEPARADORES NARANJAS
  tft.drawLine(0, 187, 320, 187, ILI9341_ORANGE);
  tft.drawLine(0, 188, 320, 188, ILI9341_ORANGE);

  // IMPRIMIR "TALLER ELECTRÓNICA"
  tft.setCursor(45, 200);
  tft.setTextColor(ILI9341_BLACK);
  tft.printf("TALLER DE ELECTRONICA");

  // REINICIAR TAMAÑO FUENTES
  tft.setTextSize(0);
}
// MOSTRAR LOGO REPSOL
void mostrarLogoRepsol(){
  tft.drawRGBBitmap(100, 110, repsolBitMap, 110, 80);
}
// FUNCIÓN RECIBIR DATOS I2C
void onReceive(int len) {
  horaActRec = Wire.read();
  wireAvailable = Wire.available();
  minutosActRec = Wire.read();
  segundosActRec = Wire.read();
  diaActRec = Wire.read();
  mesActRec = Wire.read();
  anoActRec = Wire.read();

  Serial.print("Valor I2C: ");
  Serial.println(wireAvailable);
  Serial.print("Hora actual: ");
  Serial.println(horaActRec);
  Serial.print("Minutos actual: ");
  Serial.println(minutosActRec);
  Serial.print("Segundos actual: ");
  Serial.println(segundosActRec);
  Serial.print("Dia actual: ");
  Serial.println(diaActRec);
  Serial.print("Mes actual: ");
  Serial.println(mesActRec);
  Serial.print("Año actual: ");
  Serial.println(anoActRec);
  Serial.println("------------------");
}
