#include <cmath>
#include <Arduino.h>

#include "config.h"

#include "lvgl.h"

#include "display.h"
#include "esp_bsp.h"
#include "lv_port.h"
#include "lv_conf.h"

/**
 * Setup the display.
 */
void setup_display_tft();

/**
 * Power off the display.
 */
void power_off_display_tft();

/**
 * Clear the display.
 */
void clear_display_tft();

/**
 * Draw speed in knots.
 *
 * @param speed
 */
void draw_speed_tft(float speed);

/**
 * Draw top status bar.
 */
void draw_top_bar_tft();

/**
 * Draw bottom status bar.
 */
void draw_bottom_bar_tft();

/**
 * Draw status text.
 *
 * @param text
 */
void draw_status_tft(String text);

/**
 * Clear status text area.
 */
void clear_status_tft();

/**
 * Draw battery capacity indicator.
 *
 * @param percentage Battery capacity.
 */
void draw_battery_status_tft(int percentage);

/**
 * Draw time.
 *
 * @param hours
 * @param minutes
 * @param seconds
 */
void draw_time_tft(int hours, int minutes, int seconds);

/**
 * Draw units.
 *
 * @param text
 */
void draw_units_tft(String text);

/**
 * Draw rectangle on the screen.
 *
 * @param x Start point X coordinate.
 * @param y Start point Y coordinate.
 * @param w Width of the rectangle.
 * @param h Height of the rectangle.
 * @param partial Partial or full screen draw.
 */
void draw_box_tft(uint16_t x, uint16_t y, uint16_t w, uint16_t h, bool partial);
