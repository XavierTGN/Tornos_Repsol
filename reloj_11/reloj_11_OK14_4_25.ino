#define WIDTH 240
#define HEIGHT 320
#define SMOOTH_FONT
#define DEBUG(a) Serial.println(a);

#include <TFT_eSPI.h>
#include "repsol.h"
#include "logo.h"
#include "logo_NTP.h"
#include "Alert.h"
#include "logo_REPSOL1.h"

#include "Free_Fonts.h"  // Include the header file attached to this sketch
#include <TimeLib.h>
#include <WiFi.h>
#include "time.h"
#include <Wire.h>  // Comunicación I2C


const char *ssid = "MOVISTAR-WIFI6-6810";
const char *password = "N9HEPszqVUT93Js79xqs";

const char *ntpServer = "ntp.lonelybinary.com";
const long gmtOffset_sec = 3600L * 1;
const int daylightOffset_sec = 1;


struct tm timeinfo;

//  conexion comunicaciones   NO TOCAR *********************
#define TFT_MISO 16
#define TFT_MOSI 7
#define TFT_SCLK 15
#define TFT_CS 5  // Chip select control pin
#define TFT_DC 6  // Data Command control pin
//#define TFT_RST   4   // Reset pin (could connect to RST pin)
#define TFT_RST -1  // Set TFT_RST to -

// DECLARAR VARIABLES, ENTRADAS LNL-1200
bool IN_1, IN_2, IN_3, IN_4, IN_5, IN_6;

// DEFINIR PINES, ENTRADAS LNL-1200
#define PIN_IN_1 42  //Acceso concedido
#define PIN_IN_2 41  //Acceso denegado
#define PIN_IN_3 40  //Matrícula no reconocida. Consulte a seguridad
#define PIN_IN_4 39  //Fuera de servicio
#define PIN_IN_5 38  //Error. Imparidad fichaje
#define PIN_IN_6 45  //Error. Consulte a seguridad

// DECLARAR VARIABLES, I2C
uint32_t i = 0;
int wireAvailable;

// DEFINIR DIRECCIÓN I2C ESCLAVO
#define DIRECCION_ESCLAVO 11
const int I2C_SDA = 8;
const int I2C_SCL = 9;

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

TFT_eSPI tft = TFT_eSPI();
//TFT_eSprite win_mensaje = TFT_eSprite(&tft);
TFT_eSprite win_reloj = TFT_eSprite(&tft);
TFT_eSprite logo = TFT_eSprite(&tft);
TFT_eSprite logo_REPSOLpetit = TFT_eSprite(&tft);
TFT_eSprite win_total = TFT_eSprite(&tft);


// parametros de la pantalla
int off_orig_x = 0;    //  valor negativo x a restar por si la esquina origen no coincide con el hueco de la pantalla
int off_orig_y = 0;    //  valor negativo y a restar por si la esquina origen no coincide con el hueco de la pantalla
int tft_width = 320;   //  amplada de la pantalla
int tft_height = 240;  //  altura de la pantalla

//  Ventana  logo
int logo_ini_x = 160;
int logo_ini_y = 100;
// Temps
uint32_t targetTime = 0;  // for next 1 second timeout
uint8_t hh = 11;
uint8_t mm = 55;
uint8_t ss = 56;
unsigned char lletra_gran = 8;
unsigned char lletra_petit = 4;
byte omm = 99, oss = 99;
byte xcolon = 0, xsecs = 0;
unsigned int colour = 0;
char dt[16];
char tm[16];
char sm[16];

int data = 0;
long temps_out = 0;
int angle = 10;

bool horari_estiu = false;   //he contat una hora mes al horario estiu? true=si
bool horari_hivern = false;  //he tret una hora menys al horario hivern? true=si

bool es_horari_seguent = false;

void setup() {
  // INICIALIZAR DE ENTRADAS DIGITALES
  pinMode(PIN_IN_1, INPUT);
  pinMode(PIN_IN_2, INPUT);
  pinMode(PIN_IN_3, INPUT);
  pinMode(PIN_IN_4, INPUT);
  pinMode(PIN_IN_5, INPUT);
  pinMode(PIN_IN_6, INPUT);

  // inicialitza la pantalla
  tft.init();
  tft.setRotation(3);
  tft.fillScreen(TFT_BLACK);
  tft.setSwapBytes(true);
 // tft.pushImage(40, 100, 64, 64, logo_NTP);
  tft.pushImage(100, 40, 110, 80, repsol);
  //tft.pushImage(140, 100, 60, 41, logo_REPSOL1);
  //tft.fillSmoothCircle(100, 100, 40, TFT_RED, TFT_GREEN);
  //tft.drawSmoothCircle(10, 10, 4, TFT_RED, TFT_GREEN);

  // INICIALIZAR COMUNICACIÓN I2C
  Wire.onReceive(onReceive);
  Wire.begin((uint8_t)DIRECCION_ESCLAVO);

  Serial.begin(9600);

  setTime(03, 59, 40, 30, 3, 2025);  // 2h matinada 30 març, es hivern i que que restar 1 hora

  const char *ssid = "MI-9";
  const char *password = "viscaTarracoII";

  Serial.print("Connecting to WiFi network ");
  /*
  WiFi.begin(ssid, password);
  temps_out = millis() + 5000;
  while ((WiFi.status() != WL_CONNECTED) && (temps_out >= millis())) {
    delay(400);
    barra();
    Serial.print(".");
  }

  Serial.print("Syncing time with NTP server ");
*/


  sincro_NTP();
  /*
  //while ((!getLocalTime(&timeinfo)) && (temps_out >= millis())) {
  while (!getLocalTime(&timeinfo)) {
    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
    delay(500);
    Serial.print("-");
    barra();
  }
  */
  Serial.println("");

  // disconnect WiFi
  WiFi.disconnect(true);
  WiFi.mode(WIFI_OFF);
/*
  //struct tm timeinfo;
  getLocalTime(&timeinfo);
  Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
  //setTime(00, 59, 40, 30, 3, 2025);
  setTime(timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec, timeinfo.tm_mday, timeinfo.tm_mon + 1, timeinfo.tm_year + 1900);
  */
  Serial.print("timeinfo.tm_hour--->");
  Serial.println(timeinfo.tm_hour);
  Serial.print("timeinfo.tm_min--->");
  Serial.println(timeinfo.tm_min);
  Serial.print("timeinfo.tm_mday--->");
  Serial.println(timeinfo.tm_mday);
  Serial.print("timeinfo.tm_mon--->");
  Serial.println(timeinfo.tm_mon + 1);
  Serial.print("timeinfo.tm_year--->");
  Serial.println(timeinfo.tm_year + 1900);

  sprintf(dt, "%02d/%02d/%02d", day(), month(), year());
  sprintf(tm, "%02d:%02d:%02d", hour(), minute(), second());
  sprintf(sm, "%02d", second());


  Serial.print("dt-->");
  Serial.println(dt);
  Serial.print("tm--->");
  Serial.println(tm);
  /*
  // inicialitza la pantalla
  tft.init();
  tft.setRotation(3);
  tft.fillScreen(TFT_BLACK);
  tft.pushImage(200, 0, 110, 80, repsol);
  */
  tft.fillScreen(TFT_BLACK);
  tft.pushImage(140, 100, alertWidth, alertHeight, alert);
  tft.pushImage(40, 100, 64, 64, logo_NTP);
  tft.pushImage(140, 130, 60, 41, logo_REPSOL1);
 
  //crear ventana total
  win_total.createSprite(340, 240);
  win_total.setSwapBytes(true);
  win_total.setTextWrap(true);
  win_total.setTextFont(4);
  win_total.setCursor(0, 0);
  //win_total.print("ACCESO  CONCEDIDO");
  win_total.pushSprite(0, 0);
  //////////////////////win_total.pushToSprite(&logo,100,10,TFT_BLACK);

  // crear ventana reloj
  win_reloj.createSprite(320, 240);
  win_reloj.setSwapBytes(true);
  win_reloj.fillSprite(TFT_OLIVE);
  win_reloj.setCursor(0, 0);
  win_reloj.pushSprite(0, 0);
  win_reloj.createSprite(320, 240);
  tft.setSwapBytes(true);
  targetTime = millis() + 1000;
  capcalera();
}

void loop(void) {
  llegir_DI();
  //time_t t = now();//Declaramos la variable time_t t
  opcions_teclat();
  //date_hora_guio();
  //Date_hora();
      carregar_hora();
  rellotge();
  /*
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("WIFI conectada");
  }
  */
}

void barra() {

  tft.fillRect(50, 130, angle, 35, TFT_ORANGE);
  angle = angle + 10;
}
void capcalera() {

  ///////////////tft.setCursor(10, 10, 2);
  // Set the font colour to be white with a black background, set text size multiplier to 1
  ///////////////tft.setTextColor(TFT_WHITE, TFT_BLACK);
  ///////////////tft.setTextSize(1);
  // We can now plot text on screen using the "print" class
  // començar a la linea y 30, del 0 al 30 no es veu
  tft.pushImage(254, 110, 60, 41, logo_REPSOL1);

  //tft.pushImage(random(tft.width() - alertWidth), random(tft.height() - alertHeight), alertWidth, alertHeight, alert);
  // tft.pushImage(random(tft.width() - alertWidth), random(tft.height() - alertHeight), alertWidth, alertHeight, logo_NTP);

  tft.setCursor(200, 37, 2);
  tft.println(" C.I.  TARRAGONA");

  tft.drawRect(1, 31, 319, 28, TFT_RED);
  tft.drawRect(2, 32, 318, 26, TFT_RED);

  //tft.fillRect(0, 0, 319, 24,TFT_GREEN);
  tft.drawRect(1, 60, 319, 100, TFT_ORANGE);
  tft.drawRect(2, 61, 317, 98, TFT_ORANGE);

  tft.drawRect(1, 161, 319, 40, TFT_ORANGE);
  tft.drawRect(2, 162, 316, 38, TFT_ORANGE);
}
void sincro_NTP() {
  horari_estiu = false;
  horari_hivern = false;

  
  WiFi.begin(ssid, password);
  temps_out = millis() + 5000;
  struct tm timeinfo;
  while ((WiFi.status() != WL_CONNECTED) && (temps_out >= millis())) {
    delay(400);
    Serial.print(".");
  }
  Serial.print("Syncing time with NTP server ");
  while (!getLocalTime(&timeinfo)) {
    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
    delay(500);
    Serial.print("-");
  }
  delay(1000);
  //carregar_hora();
  // disconnect WiFi
  WiFi.disconnect(true);
  WiFi.mode(WIFI_OFF);
    //struct tm timeinfo;
  getLocalTime(&timeinfo);
  Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
  //setTime(00, 59, 40, 30, 3, 2025);
  setTime(timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec, timeinfo.tm_mday, timeinfo.tm_mon + 1, timeinfo.tm_year + 1900);
  carregar_hora();
}
void rellotge() {
  hh = hour();
  mm = minute();
  ss = second();
  if (targetTime < millis()) {
    // Set next update for 1 second later
    targetTime = millis() + 1000;
    // Update digital time
    int xpos = 5;
    int ypos = 72;  // Top left corner ot clock text, about half way down
    int ysecs = ypos + 1;

    tft.setCursor(4, 37, 2);
    tft.print(dt);

    if (omm != mm) {  // Redraw hours and minutes time every minute

      omm = mm;
      // Draw hours and minutes
      if (hh < 10) xpos += tft.drawChar('0', xpos, ypos, lletra_gran);  // Add hours leading zero for 24 hr clock
      xpos += tft.drawNumber(hh, xpos, ypos, lletra_gran);              // Draw hours
      xcolon = xpos;                                                    // Save colon coord for later to flash on/off later
      xpos += tft.drawChar(':', xpos, ypos - 8, lletra_gran);
      if (mm < 10) xpos += tft.drawChar('0', xpos, ypos, lletra_gran);  // Add minutes leading zero
      xpos += tft.drawNumber(mm, xpos, ypos, lletra_gran);              // Draw minutes
      xsecs = xpos;                                                     // Sae seconds 'x' position for later display updates
    }
    if (oss != ss) {  // Redraw seconds time every second
      oss = ss;
      xpos = xsecs;

      if (ss % 2) {                                             // Flash the colons on/off
        tft.setTextColor(TFT_RED, TFT_BLACK);                   // Set colour to grey to dim colon
        tft.drawChar(':', xcolon, ypos - 8, lletra_gran);       // Hour:minute colon
        xpos += tft.drawChar(':', xsecs, ysecs, lletra_petit);  // Seconds colon
        tft.setTextColor(TFT_YELLOW, TFT_BLACK);                // Set colour back to yellow
      } else {
        tft.drawChar(':', xcolon, ypos - 8, lletra_gran);       // Hour:minute colon
        xpos += tft.drawChar(':', xsecs, ysecs, lletra_petit);  // Seconds colon
      }

      //Draw seconds
      if (ss < 10) xpos += tft.drawChar('0', xpos, ysecs, lletra_petit);  // Add leading zero
      tft.drawNumber(ss, xpos, ysecs, lletra_petit);                      // Draw seconds
    }
  }
}
//win_reloj.fillSprite(0);
void llegir_DI() {
  /*
  // VARIABLES LEEN ESTADO DE LAS ENTRADAS IN_X
  if (digitalRead(PIN_IN_1)==HIGH){
    mensaje("ACCESO","CONCEDIDO", TFT_GREEN, TFT_WHITE);
  }
  if (digitalRead(PIN_IN_2)==HIGH){
    mensaje("ACCESO","DENEGADO", TFT_RED, TFT_WHITE);
  }
  if (digitalRead(PIN_IN_3)==HIGH){
    mensaje("MATRICULA","NO RECONOCIDA", TFT_RED, TFT_WHITE);
  }
  if (digitalRead(PIN_IN_4)==HIGH){
    mensaje("FUERA","DE SERVICIO", TFT_RED, TFT_WHITE);
  }
  if (digitalRead(PIN_IN_5)==HIGH){
    mensaje("ERROR","IMPARIDAD FICHAJE", TFT_RED, TFT_WHITE);
  }
  if (digitalRead(PIN_IN_6)==HIGH){
    mensaje("ERROR","CONSULTE SEGURIDAD", TFT_RED, TFT_WHITE);
  }
  */
}
void Date_hora() {
  // Hora
  carregar_hora();
  //Set text padding to 10 pixels

  //tft.setSwapBytes(true);
  //tft.pushImage(200, 0, 110, 80, repsol);
  //tft.pushImage(1, 170, 87, 60, logo_REPSOL);

  tft.setTextPadding(10);
  tft.setCursor(1, 100);
  tft.setTextFont(8);
  tft.setTextSize(1);
  //tft.drawNumber(i, 50, 50, 7);
  tft.drawString(tm, 1, 80, 8);
  //////////////////tft.print(tm);
  //  date
  tft.setCursor(1, 10);
  tft.setTextFont(2);
  tft.setTextSize(2);
  tft.print(dt);
  tft.drawString(dt, 1, 5, 2);

  //win_reloj.pushSprite(0, 0);

  //Serial.println("imprimir a tft--->");
  win_reloj.fillSprite(0);
}
void mensaje(String text_1, String text_2, int col_fons, int col_lletra) {
  win_total.setCursor(10, 10);
  win_total.pushSprite(0, 0);
  win_total.fillSprite(col_fons);
  win_total.setTextColor(col_lletra, col_fons);
  win_total.setFreeFont(FSS18);  // Select Free Serif 24 point font
  win_total.setTextDatum(4);
  win_total.println();  // Move cursor down a line
  win_total.setTextDatum(BC_DATUM);
  //win_total.drawString("Texto centrado", 30, 10);
  win_total.println(text_1);    // Print the font name onto the TFT screen
  win_total.setFreeFont(FF19);  // Select Free Serif 24 point font
  win_total.println(text_2);    // Print the font name onto the TFT screen
  win_total.pushSprite(0, 0);
  delay(2000);
  //tft.init();
  tft.setRotation(3);

  tft.fillScreen(TFT_BLACK);

  //win_total.fillSprite(TFT_BLACK);
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
void carregar_hora() {
  hh = hour();
  mm = minute();
  ss = second();

  Serial.print(dt);
  Serial.print(" ");
  Serial.print(tm);
  Serial.print("->>>");
  Serial.print(hh);
  Serial.print(":");
  Serial.print(mm);
  Serial.print(":");
  Serial.println(ss);


  sprintf(dt, "%02d/%02d/%02d", day(), month(), year());
  sprintf(tm, "%02d:%02d:%02d", hour(), minute(), second());

  Serial.print("es_horari_seguent=");
  Serial.print(es_horari_seguent);

  if ((es_horari_seguent == true) && (hh > 5)) {
    es_horari_seguent = false;
  }
  if (es_horari_seguent == false) {
    if (isDst(day(), month(), weekday(), hour()) == true) {
      //  es horari de estiu
      Serial.println("Es horari estiu+1!!!!");
      // es la primera deteccio de horari de estiu??
      if (horari_estiu == false) {
        Serial.println("sumar 1 hora!!!!");
        hh = hh + 1;
        es_horari_seguent = true;
        setTime(hh, mm, ss, day(), month(), year());  // Another way to set
        horari_estiu = true;
        horari_hivern = false;
      }
    } else {
      /// Horari de hivern detectat
      Serial.println("Es horari hivern---1hora!!!!");
      // es la primera deteccio de horari de estiu??
      if (horari_hivern == false) {
        hh = hh - 1;
        es_horari_seguent = true;
        Serial.println("restar 1 hora!!!!");
        setTime(hh, mm, ss, day(), month(), year());  // Another way to set
        // Per no tarnar a pasar fins despres de molt temps
        horari_estiu = false;
        horari_hivern = true;
      } else {
        Serial.println("esta bloqueixat per una 5 horas!!!!");
      }
    }
  }
}

void Grids() {
  //SMALLEST_TOP bars_left
  // Draw tick -12th degree
  ////////////tft.init();
  //////////////tft.setRotation(1);
  tft.setTextSize(1);
  tft.fillScreen(TFT_BLACK);
  for (int x = 0; x <= 320; x = x + 20) {
    tft.drawLine(x, 1, x, 240, TFT_RED);
    tft.drawNumber(x, x, 20, 1);
  }
  for (int y = 0; y <= 240; y = y + 20) {
    tft.drawLine(1, y, 340, y, TFT_WHITE);
    tft.drawNumber(y, 100, y, 1);
  }
  delay(10000);
}
void opcions_teclat() {
  if (Serial.available() > 0) {
    data = Serial.read();
    Serial.println(data, DEC);
    if (data == 'S') {
      sincro_NTP();
      delay(200);
    }
    if (data == 'A') {
      setTime(00, 59, 40, 30, 3, 2025);  // 2h matinada 30 març, es hivern i que que restar 1 hora
      //time_t t=now();
      delay(200);
    }
    if (data == 'B') {
      setTime(2, 59, 55, 26, 10, 2025);  //En aquest moment es estiu.Que que suma 1 hora3h matinada  26 oct 2025  començar el hivern
      //time_t t=now();
      delay(200);
    }
    if (data == 'C') {
      setTime(22, 59, 55, 26, 10, 2025);  //En aquest moment es estiu.Que que suma 1 hora3h matinada  26 oct 2025  començar el hivern
      //time_t t=now();
    }
    if (data == '+') {
      hh = hh + 1;
      setTime(hh, mm, ss, day(), month(), year());  // Another way to set
                                                    //time_t t=now();
    }
    if (data == '-') {
      hh = hh - 1;
      setTime(hh, mm, ss, day(), month(), year());  // Another way to set
                                                    //time_t t=now();
    }
    if (data == 'Q') {
      mm = mm + 1;
      setTime(hh, mm, ss, day(), month(), year());  // Another way to set
                                                    //time_t t=now();
    }
    if (data == 'W') {
      mm = mm - 1;
      setTime(hh, mm, ss, day(), month(), year());  // Another way to set
                                                    //time_t t=now();
    }
    if (data == 'G') {
      Grids();
    }
    if (data == 'N') {
      date_hora_guio();
    }
    if (data == 'M') {
      mensaje("    ACCESO", "    PERMITIDO", TFT_GREEN, TFT_BLACK);
      delay(2000);
      win_total.fillSprite(TFT_BLACK);
      mensaje("    ACCESO", "    DENEGADO", TFT_RED, TFT_WHITE);
      delay(2000);
      win_total.fillSprite(TFT_WHITE);
    }
    DEBUG((char)data);
  }
}

void date_hora_guio() {
  if (targetTime < millis()) {
    carregar_hora();

    //txtSprite.pushToSprite(&background,2,3);
    //txtSprite.drawString(String(ss),0,0,6);
    //tft.setTextFont(1);
    /*
    for (int i = 0; i <= 255; i++) {
      tft.drawNumber(i, i, 200, 6);
      delay(80);
    }*/
    //tft.setFreeFont(FSB18);                              // Select the font
    //tft.drawString("Serif Bold 9pt", 10, 10, 8);  // Draw the text string in the selected GFX free font
    //background.pushSprite(0,0);
    //tft.drawNumber(6666,200,10,6);
    //tft.setTextSize(1);


    // Set next update for 1 second later
    targetTime = millis() + 1000;
    ////////////////////carregar_hora();


    tft.setCursor(10, 10, 2);
    // Set the font colour to be white with a black background, set text size multiplier to 1
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.setTextSize(1);
    // We can now plot text on screen using the "print" class
    tft.drawRect(1, 1, 320, 30, TFT_RED);
    //tft.fillRect(0, 0, 319, 24,TFT_GREEN);
    tft.drawRect(1, 31, 320, 200, TFT_RED);

    tft.print(dt);
    tft.setCursor(200, 10, 1);
    tft.println(" C.I. TARRAGONA");

    // Set the font colour to be yellow with no background, set to font 7
    tft.setTextColor(TFT_YELLOW);
    tft.setTextFont(6);



    /*

      tft.setCursor(10, 10,3);
      //tft.setTextFont(3);
      tft.setTextSize(1);
      // Draw hours and minutes
      tft.setTextColor(TFT_RED, TFT_BLACK);  
      tft.drawChar('REPSOL:', 10, 10, lletra_petit); 
      tft.println("Hello World!");
    */

    //tft.setCursor(0, 0);
    //tft.setTextColor(ILI9341_WHITE);
    ////background.pushSprite(0, 0);
    ////RepsolSprite.pushImage(0, 0, 110, 80, repsol);
    ////RepsolSprite.pushToSprite(&background, 100, 0, TFT_BLACK);

    // Update digital time
    int xpos = 0;
    int ypos = 110;  // Top left corner ot clock text, about half way down
    int ysecs = ypos + 24;

    if (omm != mm) {  // Redraw hours and minutes time every minute
      omm = mm;
      // posar tipos de lletra i amplada
      tft.setCursor(10, 100);
      tft.setTextFont(7);
      tft.setTextSize(1);
      // Draw hours and minutes
      tft.setTextColor(TFT_YELLOW, TFT_BLACK);                          // Set colour to grey to dim colon
      if (hh < 10) xpos += tft.drawChar('0', xpos, ypos, lletra_gran);  // Add hours leading zero for 24 hr clock
      xpos += tft.drawNumber(hh, xpos, ypos, lletra_gran);              // Draw hours
      xcolon = xpos;                                                    // Save colon coord for later to flash on/off later
      xpos += tft.drawChar(':', xpos, ypos - 8, lletra_gran);
      if (mm < 10) xpos += tft.drawChar('0', xpos, ypos, lletra_gran);  // Add minutes leading zero
      xpos += tft.drawNumber(mm, xpos, ypos, lletra_gran);              // Draw minutes
      xsecs = xpos;                                                     // Sae seconds 'x' position for later display updates
    }
    if (oss != ss) {  // Redraw seconds time every second
      oss = ss;
      xpos = xsecs;

      if (ss % 2) {                                             // Flash the colons on/off
        tft.setTextColor(TFT_RED, TFT_BLACK);                   // Set colour to grey to dim colon
        tft.drawChar(':', xcolon, ypos - 8, lletra_gran);       // Hour:minute colon
        xpos += tft.drawChar(':', xsecs, ysecs, lletra_petit);  // Seconds colon
        tft.setTextColor(TFT_YELLOW, TFT_BLACK);                // Set colour back to yellow
      } else {
        tft.drawChar(':', xcolon, ypos - 8, lletra_gran);       // Hour:minute colon
        xpos += tft.drawChar(':', xsecs, ysecs, lletra_petit);  // Seconds colon
      }

      //Draw seconds
      if (ss < 10) xpos += tft.drawChar('0', xpos, ysecs, lletra_petit);  // Add leading zero
      Serial.print("-->");
      Serial.print(xpos);
      Serial.print("<-->");
      Serial.println(tft.drawChar('0', xpos, ysecs, lletra_petit));
      tft.drawNumber(ss, xpos, ysecs, lletra_petit);  // Draw seconds
    }
  }
}

bool isDst(int dia, int mes, int dia_semana, int hora) {
  //Enero, febrero, noviembre y diciembre están fuera.
  //=======================================================
  // ¿Estamos en horario de verano hoy?-->SI   retorna true
  //=======================================================
  if (mes < 3 || mes > 10) { return false; }

  //Abril a septiembre están dentro
  if (mes > 3 && mes < 10) { return true; }

  int domingo_anterior = dia - dia_semana + 1;
  //En marzo, estamos en DST si es después del último domingo.
  if (mes == 3 && domingo_anterior >= 25 && dia_semana == 1) { return hora >= 2; }
  if (mes == 3) { return domingo_anterior >= 25; }

  //En octubre, estamos en DST si es antes del último domingo
  if (domingo_anterior < 25) { return true; }
  if (domingo_anterior >= 25 && dia_semana == 1) { return hora < 3; }
  return false;
}
// Hora Central Europea
// De CET a CEST:
//    último domingo de marzo a las 02:00 hora local -> 03:00
// De CEST a CET:
//    último domingo de octubre a las 03:00 hora local -> 02:00
//
// día    1 .. 31
// mes  1 .. 12
// dia_semana    1 .. 7   (día de la semana, 1=domingo)
// hora   0 .. 23 hora local
//