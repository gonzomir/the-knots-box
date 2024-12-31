#ifdef EINK
#include "draw_eink.h"
#endif

#ifdef TFT
#include "draw_tft.h"
#endif

/**
 * Setup the display.
 */
void setup_display() {
	#ifdef EINK
	Serial.println("Using eInk display routines.");
	setup_display_eink();
	#endif

	#ifdef TFT
	Serial.println("Using TFT display routines.");
	setup_display_tft();
	#endif
}

/**
 * Power off the display.
 */
void power_off_display() {
	#ifdef EINK
	power_off_display_eink();
	#endif

	#ifdef TFT
	power_off_display_tft();
	#endif
}

/**
 * Clear the display.
 */
void clear_display() {
	#ifdef EINK
	clear_display_eink();
	#endif
}

/**
 * Draw speed in knots.
 *
 * @param speed
 */
void draw_speed(float speed) {
	#ifdef EINK
	draw_speed_eink(speed);
	#endif

	#ifdef TFT
	draw_speed_tft(speed);
	#endif
}

/**
 * Draw top status bar.
 *
 * @param text
 */
void draw_top_bar() {
	#ifdef EINK
	draw_top_bar_eink();
	#endif
}

/**
 * Draw bottom status bar.
 *
 * @param text
 */
void draw_bottom_bar() {
	#ifdef EINK
	draw_bottom_bar_eink();
	#endif
}

/**
 * Draw status text.
 *
 * @param text
 */
void draw_status(String text) {
	#ifdef EINK
	draw_status_eink(text);
	#endif

	#ifdef TFT
	draw_status_tft(text);
	#endif
}

/**
 * Clear status text area.
 */
void clear_status() {
	#ifdef EINK
	clear_status_eink();
	#endif

	#ifdef TFT
	clear_status_tft();
	#endif
}

/**
 * Draw battery capacity indicator.
 *
 * @param percentage Battery capacity.
 */
void draw_battery_status(int percentage) {
	#ifdef EINK
	draw_battery_status_eink(percentage);
	#endif

	#ifdef TFT
	draw_battery_status_tft(percentage);
	#endif
}

/**
 * Draw time.
 *
 * @param hours
 * @param minutes
 * @param seconds
 */
void draw_time(int hours, int minutes, int seconds) {
	#ifdef EINK
	draw_time_eink(hours, minutes, seconds);
	#endif

	#ifdef TFT
	draw_time_tft(hours, minutes, seconds);
	#endif
}

/**
 * Draw units.
 *
 * @param text
 */
void draw_units(String text) {
	#ifdef EINK
	draw_units_eink(text);
	#endif

	#ifdef TFT
	draw_units_tft(text);
	#endif
}
