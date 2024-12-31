#include <cmath>

#include <GxEPD2_BW.h>
#include <GxEPD2_3C.h>

#include "config.h"
#include "draw_eink.h"

#include "fonts/RethinkSans/rethinksans-bold-12.h"
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
GxEPD2_DISPLAY_CLASS<GxEPD2_DRIVER_CLASS, MAX_HEIGHT(GxEPD2_DRIVER_CLASS)> display(GxEPD2_DRIVER_CLASS(DISPLAY_CS, DISPLAY_DC, DISPLAY_RST, DISPLAY_BUSY));
#endif

#if defined(ESP32) && defined(USE_HSPI_FOR_EPD)
SPIClass hspi(HSPI);
#endif

int padding = 50;
int battery_status_w = 100;
int time_w = 90;

/**
 * Setup the display.
 */
void setup_display_eink() {
  // *** special handling for Waveshare ESP32 Driver board *** //
  // ********************************************************* //
#if defined(ESP32) && defined(USE_HSPI_FOR_EPD)
  hspi.begin(DISPLAY_CLK, 12, DISPLAY_DIN, DISPLAY_CS); // remap hspi for EPD (swap pins)
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
void power_off_display_eink() {
  display.setFullWindow();
  do {
    display.fillScreen(GxEPD_BLACK);
  } while (display.nextPage());
  do {
    display.fillScreen(GxEPD_WHITE);
  } while (display.nextPage());
  display.powerOff();
}

/**
 * Clear the display.
 */
void clear_display_eink() {
  display.setFullWindow();
  do {
    display.fillScreen(GxEPD_WHITE);
  } while (display.nextPage());
}

/**
 * Draw speed in knots.
 *
 * @param speed
 */
void draw_speed_eink(float speed) {
  char speed_r[4];
  sprintf(speed_r, "%0.1f", std::round(speed * 10) / 10);

  display.setRotation(0);
  display.setFont(&RethinkSans_Bold92pt7b);
  display.setTextColor(GxEPD_BLACK);

  display.setPartialWindow(padding / 2, padding, display.width() - padding, display.height() - 2 * padding);

  int16_t tbx, tby; uint16_t tbw, tbh;
  display.getTextBounds(speed_r, 0, 0, &tbx, &tby, &tbw, &tbh);

  int16_t x = (display.width() - tbw) / 2;
  int16_t y = (display.height() + tbh) / 2;

  do {
    display.fillRect(padding / 2, padding, display.width() - padding, display.height() - 2 * padding, GxEPD_WHITE);
    display.setCursor(x, y);
    display.print(speed_r);
  } while (display.nextPage());
}

/**
 * Draw top status bar.
 *
 * @param text
 */
void draw_top_bar_eink() {
  draw_box_eink(0, 41, display.width(), 1, true);
}

/**
 * Draw bottom status bar.
 *
 * @param text
 */
void draw_bottom_bar_eink() {
  draw_box_eink(0, display.height() - 41, display.width(), 1, true);
}

/**
 * Draw status text.
 *
 * @param text
 */
void draw_status_eink(String text) {
  display.setRotation(0);
  display.setFont(&RethinkSans_Bold12pt7b);
  display.setTextColor(GxEPD_BLACK);

  display.setPartialWindow(0, display.height() - 40, display.width() - 120, 40);

  int16_t x = 10;
  int16_t y = display.height() - 15;

  do {
    display.fillRect(0, display.height() - 40, display.width() - 120, 40, GxEPD_WHITE);
    display.setCursor(x, y);
    display.print(text);
  } while (display.nextPage());
}

/**
 * Clear status text area.
 */
void clear_status_eink() {
  display.setRotation(0);

  display.setPartialWindow(0, display.height() - 40, display.width() - 120, 40);

  do {
    display.fillRect(0, display.height() - 40, display.width() - 120, 40, GxEPD_WHITE);
  } while (display.nextPage());
}

/**
 * Draw battery capacity indicator.
 *
 * @param percentage Battery capacity.
 */
void draw_battery_status_eink(int percentage) {
  display.setRotation(0);
  display.setFont(&RethinkSans_Bold12pt7b);
  display.setTextColor(GxEPD_BLACK);

  display.setPartialWindow(display.width() - battery_status_w, 0, battery_status_w, 40);

  int16_t bat_x = display.width() - battery_status_w + 4;
  int16_t bat_y = 10;
  int16_t bat_w = 16;
  int16_t bat_h = 25;
  int16_t bat_b = 2;

  int16_t percentage_rounded = round(percentage / 5) * 5;
  int16_t bat_f = (bat_h - 2 * bat_b) * (1.00 - percentage_rounded / 100.00);

  int16_t x = display.width() - battery_status_w + 24;
  int16_t y = 30;
  char status[4];

  sprintf(status, "%d%%", percentage);

  do {
    display.fillRect(display.width() - battery_status_w, 0, battery_status_w, 40, GxEPD_WHITE);

    display.fillRect(bat_x, bat_y, bat_w, bat_h, GxEPD_BLACK);
    display.fillRect(bat_x + bat_b, bat_y - bat_b, bat_w - 2 * bat_b, bat_b, GxEPD_BLACK);

    display.fillRect(bat_x + bat_b, bat_y + bat_b, bat_w - 2 * bat_b, bat_f, GxEPD_WHITE);

    display.setCursor(x, y);
    display.print(status);
  } while (display.nextPage());
}

/**
 * Draw time.
 *
 * @param hours
 * @param minutes
 * @param seconds
 */
void draw_time_eink(int hours, int minutes, int seconds) {
  display.setRotation(0);
  display.setFont(&RethinkSans_Bold12pt7b);
  display.setTextColor(GxEPD_BLACK);

  display.setPartialWindow(0, 0, time_w, 40);

  int16_t x = 10;
  int16_t y = 30;

  char time[6];
  sprintf(time, "%02d:%02d", hours, minutes);

  do {
    display.fillRect(0, 0, time_w, 40, GxEPD_WHITE);
    display.setCursor(x, y);
    display.print(time);
  } while (display.nextPage());
}

/**
 * Draw units.
 *
 * @param text
 */
void draw_units_eink(String text) {
  display.setRotation(0);
  display.setFont(&RethinkSans_Bold12pt7b);
  display.setTextColor(GxEPD_BLACK);

  int16_t tbx, tby; uint16_t tbw, tbh;
  display.getTextBounds(text, 0, 0, &tbx, &tby, &tbw, &tbh);

  int16_t bx = display.width() - tbw - 20;
  int16_t bw = tbw + 20;
  int16_t by = display.height() - 40;
  int16_t bh = 40;

  int16_t tx = display.width() - tbw - 10;
  int16_t ty = display.height() - 15;

  display.setPartialWindow(bx, by, bw, bh);

  do {
    display.fillRect(bx, by, bw, bh, GxEPD_WHITE);
    display.setCursor(tx, ty);
    display.print(text);
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
void draw_box_eink(uint16_t x, uint16_t y, uint16_t w, uint16_t h, bool partial) {
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
