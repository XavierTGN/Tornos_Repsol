#define WIDTH 240
#define HEIGHT 320
#define SMOOTH_FONT
#define DEBUG(a) Serial.println(a);

#include <TFT_eSPI.h>
#include "repsol.h"
#include "Free_Fonts.h"  // Include the header file attached to this sketch
#include <TimeLib.h>

//  conexion comunicaciones
#define TFT_MISO 16
#define TFT_MOSI 7
#define TFT_SCLK 15
#define TFT_CS 5  // Chip select control pin
#define TFT_DC 6  // Data Command control pin
//#define TFT_RST   4   // Reset pin (could connect to RST pin)
#define TFT_RST -1  // Set TFT_RST to -

TFT_eSPI tft = TFT_eSPI();
//TFT_eSprite win_mensaje = TFT_eSprite(&tft);
TFT_eSprite win_reloj = TFT_eSprite(&tft);
TFT_eSprite logo = TFT_eSprite(&tft);
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
unsigned char lletra_petit = 6;
byte omm = 99, oss = 99;
byte xcolon = 0, xsecs = 0;
unsigned int colour = 0;
char dt[16];
char tm[16];

int data =0;


bool horari_estiu = false;   //he contat una hora mes al horario estiu? true=si
bool horari_hivern = false;  //he tret una hora menys al horario hivern? true=si

bool es_horari_seguent = false;

void setup() {
  Serial.begin(9600);

  setTime(00, 59, 40, 30, 3, 2025);  // 2h matinada 30 març, es hivern i que que restar 1 hora
  //setTime(2, 59, 55, 26, 10, 2025);  //3h matinada  26 oct 2025  començar el hivern

  carregar_hora();

  //time_t t = now(); // Store the current time in time
  targetTime = millis() + 1000;
  tft.init();
  tft.setRotation(1);

  tft.fillScreen(TFT_BLACK);
  /*
  //crear ventana logo
  logo.setColorDepth(16);
  logo.setSwapBytes(true);
  logo.createSprite(110, 80);
  logo.setTextWrap(true);
  //logo.setCursor(0, 0);
  //logo.setTextFont(4);
  //logo.fillScreen(TFT_TRANSPARENT);
  //logo.print("logo logo sunt in culpa qui officia deserunt mollit anim id est laborum");
  logo.pushSprite(190, 0);
  logo.pushImage(0, 0, 110, 80, repsol);
  //Bit_repsol.pushToSprite(&logo,10,0,TFT_BLACK);
*/
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
  ////////win_reloj.pushToSprite(&logo,100,10,TFT_BLACK);
  ///////////logo.pushImage(0, 0, 110, 80, repsol);
  win_reloj.pushImage(200, 0, 110, 80, repsol);
  delay(2000);
}

void loop(void) {
  //time_t t = now();//Declaramos la variable time_t t
  if (targetTime < millis()) {
    carregar_hora();
    if (Serial.available() > 0) {
      data = Serial.read();
      Serial.println(data, DEC);
       if (data== 'A'){
          setTime(00, 59, 40, 30, 3, 2025);  // 2h matinada 30 març, es hivern i que que restar 1 hora
          //time_t t=now();
          delay(200);
       }
       if (data== 'B'){
          setTime(2, 59, 55, 26, 10, 2025);  //En aquest moment es estiu.Que que suma 1 hora3h matinada  26 oct 2025  començar el hivern
          //time_t t=now();
          delay(200);
       }
       if (data== 'C'){
          setTime(22, 59, 55, 26, 10, 2025);  //En aquest moment es estiu.Que que suma 1 hora3h matinada  26 oct 2025  començar el hivern
          //time_t t=now();
        }
      if (data== '+'){
          hh = hh + 1;
          setTime(hh, mm, ss, day(), month(), year());  // Another way to set
          //time_t t=now();
      }
      if (data== '-'){
          hh = hh - 1;
          setTime(hh, mm, ss, day(), month(), year());  // Another way to set
          //time_t t=now();
      }
      if (data== 'Q'){
          mm = mm + 1;
          setTime(hh, mm, ss, day(), month(), year());  // Another way to set
          //time_t t=now();
      }
      if (data== 'W'){
          mm = mm - 1;
          setTime(hh, mm, ss, day(), month(), year());  // Another way to set
          //time_t t=now();
       }
      DEBUG((char)data);
    }
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
    /////////////carregar_hora();

    //tft.setCursor(0, 0);
    //tft.setTextColor(ILI9341_WHITE);
    /*
      tft.setTextSize(4);
      tft.println("Hello World!");
      tft.setTextSize(1);
    */
    ////background.pushSprite(0, 0);
    ////RepsolSprite.pushImage(0, 0, 110, 80, repsol);
    ////RepsolSprite.pushToSprite(&background, 100, 0, TFT_BLACK);


    /*
    // Update digital time
    int xpos = 0;
    int ypos = 110;  // Top left corner ot clock text, about half way down
    int ysecs = ypos + 24;

    if (omm != mm) {  // Redraw hours and minutes time every minute
      omm = mm;
      // posar tipos de lletra i amplada
      win_reloj.setCursor(10, 100);
      win_reloj.setTextFont(7);
      win_reloj.setTextSize(1);
      
      // Draw hours and minutes
      if (hh < 10) xpos += win_reloj.drawChar('0', xpos, ypos, lletra_gran);  // Add hours leading zero for 24 hr clock
      xpos += win_reloj.drawNumber(hh, xpos, ypos, lletra_gran);              // Draw hours
      xcolon = xpos;                                                          // Save colon coord for later to flash on/off later
      xpos += win_reloj.drawChar(':', xpos, ypos - 8, lletra_gran);
      if (mm < 10) xpos += win_reloj.drawChar('0', xpos, ypos, lletra_gran);  // Add minutes leading zero
      xpos += win_reloj.drawNumber(mm, xpos, ypos, lletra_gran);              // Draw minutes
      xsecs = xpos;                                                           // Sae seconds 'x' position for later display updates
    }
    if (oss != ss) {  // Redraw seconds time every second
      oss = ss;
      xpos = xsecs;

      if (ss % 2) {                                                   // Flash the colons on/off
        win_reloj.setTextColor(0x39C4, TFT_BLACK);                    // Set colour to grey to dim colon
        win_reloj.drawChar(':', xcolon, ypos - 8, lletra_gran);       // Hour:minute colon
        xpos += win_reloj.drawChar(':', xsecs, ysecs, lletra_petit);  // Seconds colon
        win_reloj.setTextColor(TFT_YELLOW, TFT_BLACK);                // Set colour back to yellow
      } else {
        win_reloj.drawChar(':', xcolon, ypos - 8, lletra_gran);  // Hour:minute colon
        xpos += tft.drawChar(':', xsecs, ysecs, lletra_petit);   // Seconds colon
      }

      //Draw seconds
      if (ss < 10) xpos += win_reloj.drawChar('0', xpos, ysecs, lletra_petit);  // Add leading zero
      win_reloj.drawNumber(ss, xpos, ysecs, lletra_petit);                      // Draw seconds
    }
    */
  }
  // Hora
    // Set text padding to 10 pixels
  win_reloj.setTextPadding(10);
  win_reloj.setCursor(1, 50);
  win_reloj.setTextFont(7);
  win_reloj.setTextSize(1);
  //win_reloj.drawNumber(i, 50, 50, 7);
  win_reloj.drawString(tm,10,50,6);
  //////////////////win_reloj.print(tm);
//  date
  win_reloj.setCursor(1, 1);
  win_reloj.setTextFont(2);
  win_reloj.setTextSize(2);
  //win_reloj.print(dt);
  win_reloj.drawString(dt,1,5,2);

  win_reloj.pushSprite(0, 0);
}

//win_reloj.fillSprite(0);
/*
  delay(2000);
  mensaje("    ACCESO","    PERMITIDO",TFT_GREEN,TFT_BLACK);
  delay(2000);
  mensaje("     ACCESO","    DENEGADO",TFT_RED,TFT_WHITE);
  delay(2000);

}
  */
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
}

void carregar_hora() {
  Serial.print(dt);
  Serial.print(" ");
  Serial.print(tm);
  Serial.print("->>>");
  Serial.print(hh);
  Serial.print(":");
  Serial.print(mm);
  Serial.print(":");
  Serial.println(ss);

  hh = hour();
  mm = minute();
  ss = second();
  sprintf(dt, "%02d/%02d/%02d", day(),month(),year());
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
//=======================================================
// ¿Estamos en horario de verano hoy?-->SI   retorna true
//=======================================================

bool isDst(int dia, int mes, int dia_semana, int hora) {
  //Enero, febrero, noviembre y diciembre están fuera.
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