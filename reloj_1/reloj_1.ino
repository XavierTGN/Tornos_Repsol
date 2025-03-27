#define WIDTH 240
#define HEIGHT 320
#define SMOOTH_FONT
/  test3
// test 2
// test para github
#include <TFT_eSPI.h>
#include "repsol.h"
#include "Free_Fonts.h"  // Include the header file attached to this sketch
#include <TimeLib.h>
#include <Wire.h>
#include "SPI.h"  


//  conexion comunicaciones
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

// DEFINIR DIRECCIÓN I2C ESCLAVO
#define DIRECCION_ESCLAVO 10
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


TFT_eSPI tft = TFT_eSPI();
//TFT_eSprite win_mensaje = TFT_eSprite(&tft);
TFT_eSprite win_reloj = TFT_eSprite(&tft);
TFT_eSprite logo = TFT_eSprite(&tft);
TFT_eSprite win_total = TFT_eSprite(&tft);
// parametros de la pantalla
int mens_orig_x = 0;    //  valor negativo x a restar por si la esquina origen no coincide con el hueco de la pantalla
int mens_orig_y = 0;    //  valor negativo y a restar por si la esquina origen no coincide con el hueco de la pantalla
int mens_width = 320;   //  amplada de la pantalla
int mens_height = 240;  //  altura de la pantalla

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

void setup() {
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
  
  
  Serial.begin(9600);
  setTime(9, 55, 0, 27, 3, 2025);  // Another way to set
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
  win_reloj.fillSprite(TFT_BLACK);
  win_reloj.setCursor(0, 0);
  win_reloj.pushSprite(0, 0);
  ////////win_reloj.pushToSprite(&logo,100,10,TFT_BLACK);
  ///////////logo.pushImage(0, 0, 110, 80, repsol);
  win_reloj.pushImage(200, 0, 110, 80, repsol);
  delay(2000);
}

void loop(void) {



  if (targetTime < millis()) {


    // Set next update for 1 second later
    targetTime = millis() + 1000;
    carregar_hora();

    // Update digital time
    int xpos = 0;
    int ypos = 110;  // Top left corner ot clock text, about half way down
    int ysecs = ypos + 24;

    if (omm != mm) {  // Redraw hours and minutes time every minute
      omm = mm;
      // Draw hours and minutes
      if (hh < 10) xpos += win_reloj.drawChar('0', xpos, ypos, lletra_gran);  // Add hours leading zero for 24 hr clock
      xpos += win_reloj.drawNumber(hh, xpos, ypos, lletra_gran);              // Draw hours
      xcolon = xpos;                                                          // Save colon coord for later to flash on/off later
      xpos += win_reloj.drawChar(':', xpos, ypos - 8, lletra_gran);
      if (mm < 10) xpos += win_reloj.drawChar('0', xpos, ypos, lletra_gran);  // Add minutes leading zero
      xpos += win_reloj.drawNumber(mm, xpos, ypos, lletra_gran);              // Draw minutes
      xsecs = xpos;                                                           // Sae seconds 'x' position for later display updates
    }
    if (ss==10){
      mensaje("ACCESO", "DENEGADO",TFT_RED,TFT_WHITE);
      delay(3000);
    }
    if (ss==25){
      mensaje("ACCESO", "PERMITIDO",TFT_GREEN,TFT_BLACK);
      delay(3000);
    }
    if (oss != ss) {  // Redraw seconds time every second
      oss = ss;
      xpos = xsecs;

      if (ss % 2) {                                                   // Flash the colons on/off
        //win_reloj.setTextColor(0x39C4, TFT_BLACK);                    // Set colour to grey to dim colon
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
  }

  win_reloj.setCursor(10, 100);
  win_reloj.setTextFont(7);
  win_reloj.setTextSize(1);
  //win_reloj.drawNumber(i, 50, 50, 7);
  //win_reloj.drawString("00:33:22",10,100,4);
  win_reloj.print(tm);
  win_reloj.pushSprite(0, 0);
}

void mensaje(String text_1, String text_2, int col_fons, int col_lletra) {
  // muestra un mensaje a pantalla completa
  // Parametros, 
  //    Texto primera linea
  //  Texto segunfa linea
  //  Color de la letra
  //  Color del fondo
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
  win_total.drawString(text_1, 120, 150,8);
  win_total.setFreeFont(FF19);  // Select Free Serif 24 point font
  win_total.println(text_2);    // Print the font name onto the TFT screen
  win_total.pushSprite(0, 0);
}
void carregar_hora() {
  hh = hour();
  mm = minute();
  ss = second();
  //printf(dt, "%02d/%02d/%02d", now.year(),now.month(),now.day());
  sprintf(tm, "%02d:%02d:%02d", hour(),minute(),second());
}
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

// PARTE SUPERIOR PANTALA
void parteSuperiorTFT(){
  // IMPRIMIR FECHA
    win_total.setFreeFont(FF19);  // Select Free Serif 24 point font
  //win_total.setFont(7);
  win_total.setTextColor(ILI9341_BLACK);
  win_total.setCursor(10, 40);
  win_total.printf("%d/%d/20%d", diaActRec, mesActRec, anoActRec);
  win_total.setCursor(145, 40);
  win_total.printf("C.I. TARRAGONA");

  // IMPRIMIR SEPARADORES NARANJAS
  win_total.drawLine(0, 43, 320, 43, TFT_ORANGE);
  win_total.drawLine(0, 44, 320, 44, TFT_ORANGE);
  
  // REINICIAR TAMAÑO FUENTES
  win_total.setTextSize(0);
}