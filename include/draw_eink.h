#include <cmath>

#include <GxEPD2_BW.h>
#include <GxEPD2_3C.h>

#define USE_HSPI_FOR_EPD
#define ENABLE_GxEPD2_GFX 0

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

/**
 * Setup the display.
 */
void setup_display_eink();

/**
 * Power off the display.
 */
void power_off_display_eink();

/**
 * Clear the display.
 */
void clear_display_eink();

/**
 * Draw speed in knots.
 *
 * @param speed
 */
void draw_speed_eink(float speed);

/**
 * Draw top status bar.
 */
void draw_top_bar_eink();

/**
 * Draw bottom status bar.
 */
void draw_bottom_bar_eink();

/**
 * Draw status text.
 *
 * @param text
 */
void draw_status_eink(String text);

/**
 * Clear status text area.
 */
void clear_status_eink();

/**
 * Draw battery capacity indicator.
 *
 * @param percentage Battery capacity.
 */
void draw_battery_status_eink(int percentage);

/**
 * Draw time.
 *
 * @param hours
 * @param minutes
 * @param seconds
 */
void draw_time_eink(int hours, int minutes, int seconds);

/**
 * Draw units.
 *
 * @param text
 */
void draw_units_eink(String text);

/**
 * Draw rectangle on the screen.
 *
 * @param x Start point X coordinate.
 * @param y Start point Y coordinate.
 * @param w Width of the rectangle.
 * @param h Height of the rectangle.
 * @param partial Partial or full screen draw.
 */
void draw_box_eink(uint16_t x, uint16_t y, uint16_t w, uint16_t h, bool partial);
