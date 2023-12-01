#define USE_HSPI_FOR_EPD
#define ENABLE_GxEPD2_GFX 0

#include <cmath>

#include <GxEPD2_BW.h>
#include <GxEPD2_3C.h>

#include "fonts/Roboto/roboto-bold-90.h"

#define GxEPD2_DISPLAY_CLASS GxEPD2_BW
#define GxEPD2_DRIVER_CLASS GxEPD2_420_M01

#define GxEPD2_BW_IS_GxEPD2_BW true
#define GxEPD2_3C_IS_GxEPD2_3C true
#define GxEPD2_7C_IS_GxEPD2_7C true
#define GxEPD2_1248_IS_GxEPD2_1248 true
#define IS_GxEPD(c, x) (c##x)
#define IS_GxEPD2_BW(x) IS_GxEPD(GxEPD2_BW_IS_, x)
#define IS_GxEPD2_3C(x) IS_GxEPD(GxEPD2_3C_IS_, x)
#define IS_GxEPD2_7C(x) IS_GxEPD(GxEPD2_7C_IS_, x)
#define IS_GxEPD2_1248(x) IS_GxEPD(GxEPD2_1248_IS_, x)

#if defined(ESP32)
#define MAX_DISPLAY_BUFFER_SIZE 65536ul // e.g.
#if IS_GxEPD2_BW(GxEPD2_DISPLAY_CLASS)
#define MAX_HEIGHT(EPD) (EPD::HEIGHT <= MAX_DISPLAY_BUFFER_SIZE / (EPD::WIDTH / 8) ? EPD::HEIGHT : MAX_DISPLAY_BUFFER_SIZE / (EPD::WIDTH / 8))
#elif IS_GxEPD2_3C(GxEPD2_DISPLAY_CLASS)
#define MAX_HEIGHT(EPD) (EPD::HEIGHT <= (MAX_DISPLAY_BUFFER_SIZE / 2) / (EPD::WIDTH / 8) ? EPD::HEIGHT : (MAX_DISPLAY_BUFFER_SIZE / 2) / (EPD::WIDTH / 8))
#elif IS_GxEPD2_7C(GxEPD2_DISPLAY_CLASS)
#define MAX_HEIGHT(EPD) (EPD::HEIGHT <= (MAX_DISPLAY_BUFFER_SIZE) / (EPD::WIDTH / 2) ? EPD::HEIGHT : (MAX_DISPLAY_BUFFER_SIZE) / (EPD::WIDTH / 2))
#endif
GxEPD2_DISPLAY_CLASS<GxEPD2_DRIVER_CLASS, MAX_HEIGHT(GxEPD2_DRIVER_CLASS)> display(GxEPD2_DRIVER_CLASS(/*CS=*/ 15, /*DC=*/ 27, /*RST=*/ 26, /*BUSY=*/ 25));
#endif

#if defined(ESP32) && defined(USE_HSPI_FOR_EPD)
SPIClass hspi(HSPI);
#endif

void drawSpeed(float speed);
void showBox(uint16_t x, uint16_t y, uint16_t w, uint16_t h, bool partial);
void showPartialUpdate();

void setup() {
  Serial.begin(115200);
  Serial.println();
  Serial.println("setup");

  // *** special handling for Waveshare ESP32 Driver board *** //
  // ********************************************************* //
#if defined(ESP32) && defined(USE_HSPI_FOR_EPD)
  hspi.begin(13, 12, 14, 15); // remap hspi for EPD (swap pins)
  display.epd2.selectSPI(hspi, SPISettings(4000000, MSBFIRST, SPI_MODE0));
#endif
  // *** end of special handling for Waveshare ESP32 Driver board *** //
  // **************************************************************** //

  display.init(115200);

  delay(1000);
  Serial.println("Draw top bar.");
  display.setRotation(0);
  showBox(0, 0, display.width(), 24, true);

  Serial.println("Draw speed 4.5.");
  drawSpeed(4.4681);
  delay(1000);

  Serial.println("Draw speed 4.8.");
  drawSpeed(4.8281);
  delay(1000);

  Serial.println("Draw speed 5.2.");
  drawSpeed(5.2);
  delay(1000);

  Serial.println("Draw speed 5.2.");
  drawSpeed(12.2);
  delay(1000);

//  display.powerOff();
  Serial.println("setup done");
}

void loop() {
  // put your main code here, to run repeatedly:
}

void drawSpeed(float speed) {
  Serial.println("Drawing speed...");

  char speed_r[4];
  std::sprintf(speed_r, "%0.1f", std::round(speed * 10) / 10);
  Serial.println(speed_r);

  display.setRotation(0);
  display.setFont(&Roboto_Bold90pt7b);
  display.setTextColor(GxEPD_BLACK);

  display.setPartialWindow(30, 30, display.width() - 30, display.height() - 30);

  int16_t tbx, tby; uint16_t tbw, tbh;
  display.getTextBounds(speed_r, 0, 0, &tbx, &tby, &tbw, &tbh);

  int16_t x = display.width() - tbw - 30;
  int16_t y = 250;

  Serial.printf("%d %d %d %d", display.width(), tbx, tbw, x);
  Serial.println("");

  do {
    display.fillRect(30, 30, display.width() - 30, display.height() - 30, GxEPD_WHITE);
    display.setCursor(x, y);
    display.print(speed_r);
  } while (display.nextPage());

  Serial.println("Drew speed.");
}

void showBox(uint16_t x, uint16_t y, uint16_t w, uint16_t h, bool partial) {
  Serial.println("showBox");
  display.setRotation(0);
  if (partial) {
    display.setPartialWindow(x, y, w, h);
  }
  else {
    display.setFullWindow();
  }
  display.firstPage();
  do {
    display.fillRect(x, y, w, h, GxEPD_BLACK);
  }
  while (display.nextPage());
  Serial.println("showBox done");
}
