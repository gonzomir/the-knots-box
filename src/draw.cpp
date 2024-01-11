#include <cmath>

#include <GxEPD2_BW.h>
#include <GxEPD2_3C.h>

#include "draw.h"

#include "fonts/RethinkSans/rethinksans-bold-16.h"
#include "fonts/RethinkSans/rethinksans-bold-92.h"

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

int padding = 50;
int battery_status_w = 140;

/**
 * Setup the display.
 */
void setup_display() {
  // *** special handling for Waveshare ESP32 Driver board *** //
  // ********************************************************* //
#if defined(ESP32) && defined(USE_HSPI_FOR_EPD)
  hspi.begin(13, 12, 14, 15); // remap hspi for EPD (swap pins)
  display.epd2.selectSPI(hspi, SPISettings(4000000, MSBFIRST, SPI_MODE0));
#endif
  // *** end of special handling for Waveshare ESP32 Driver board *** //
  // **************************************************************** //

  display.init(115200);
  display.setFullWindow();
  display.firstPage(); // Use paged drawing mode, sets fillScreen(GxEPD_WHITE)
  display.fillScreen(GxEPD_WHITE);
}

/**
 * Power off the display.
 */
void power_off_display() {
  display.powerOff();
}

/**
 * Clear the display.
 */
void clear_display() {
  display.setFullWindow();
  do {
    display.fillRect(0, 0, display.width(), display.height(), GxEPD_WHITE);
  } while (display.nextPage());
}

/**
 * Draw speed in knots.
 *
 * @param speed
 */
void draw_speed(float speed) {
  char speed_r[4];
  std::sprintf(speed_r, "%0.1f", std::round(speed * 10) / 10);

  display.setRotation(0);
  display.setFont(&RethinkSans_Bold92pt7b);
  display.setTextColor(GxEPD_BLACK);

  display.setPartialWindow(padding, padding, display.width() - padding, display.height() - padding);

  int16_t tbx, tby; uint16_t tbw, tbh;
  display.getTextBounds(speed_r, 0, 0, &tbx, &tby, &tbw, &tbh);

  int16_t x = display.width() - tbw - 90;
  int16_t y = 250;

  if (speed > 10) {
    x = display.width() - tbw - 50;
  }

  // Serial.printf("%d %d %d %d", display.width(), tbx, tbw, x);
  // Serial.println("");

  do {
    display.fillRect(padding, padding, display.width() - padding, display.height() - padding, GxEPD_WHITE);
    display.setCursor(x, y);
    display.print(speed_r);
  } while (display.nextPage());
}

/**
 * Draw status text.
 *
 * @param text
 */
void draw_status(String text) {
  display.setRotation(0);
  display.setFont(&RethinkSans_Bold16pt7b);
  display.setTextColor(GxEPD_BLACK);

  display.setPartialWindow(0, 0, display.width() - battery_status_w, 40);

  int16_t x = 20;
  int16_t y = 30;

  do {
    display.fillRect(0, 0, display.width(), 40, GxEPD_WHITE);
    display.setCursor(x, y);
    display.print(text);
  } while (display.nextPage());
}

/**
 * Clear status text area.
 */
void clear_status() {
  display.setRotation(0);

  display.setPartialWindow(0, 0, display.width() - battery_status_w, 40);

  do {
    display.fillRect(0, 0, display.width(), 40, GxEPD_WHITE);
  } while (display.nextPage());
}

/**
 * Draw battery capacity indicator.
 *
 * @param percentage Battery capacity.
 */
void draw_battery_status(int percentage) {
  display.setRotation(0);
  display.setFont(&RethinkSans_Bold16pt7b);
  display.setTextColor(GxEPD_BLACK);

  display.setPartialWindow(display.width() - battery_status_w, 0, display.width() - battery_status_w, 40);

  int16_t x = display.width() - battery_status_w;
  int16_t y = 30;
  char status[10];

  sprintf(status, "Bat: %d%%", percentage);

  do {
    display.fillRect(display.width() - battery_status_w, 0, display.width() - battery_status_w, 40, GxEPD_WHITE);
    display.setCursor(x, y);
    display.print(status);
  } while (display.nextPage());
}

/**
 * Draw rectangle on the screen.
 *
 * @param x Start point X coordinate.
 * @param y Start point Y coordinate.
 * @param w Width of the rectangle.
 * @param h Height of the rectangle.
 * @param partial Partial or full screen draw.
 */
void draw_box(uint16_t x, uint16_t y, uint16_t w, uint16_t h, bool partial) {
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
  } while (display.nextPage());
}
