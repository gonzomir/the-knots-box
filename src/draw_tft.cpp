#include <cmath>
#include <Arduino.h>

#include "config.h"

#include "lvgl.h"

#include "display.h"
#include "esp_bsp.h"
#include "lv_port.h"
#include "lv_conf.h"

#include "draw_tft.h"

LV_FONT_DECLARE(lvgl_rethinksans_bold_16)
LV_FONT_DECLARE(lvgl_rethinksans_bold_200)

lv_obj_t *speed_label = NULL;
lv_obj_t *status_label = NULL;
lv_obj_t *time_label = NULL;
lv_obj_t *battery_label = NULL;
lv_obj_t *units_label = NULL;

/**
 * Setup the display.
 */
void setup_display_tft() {
	bsp_display_cfg_t cfg = {
			.lvgl_port_cfg = ESP_LVGL_PORT_INIT_CONFIG(),
			.buffer_size = LCD_QSPI_H_RES * LCD_QSPI_V_RES,
			.rotate = LV_DISP_ROT_90,
	};

	bsp_display_start_with_config(&cfg);
	bsp_display_backlight_on();

	bsp_display_lock(0);

	lv_obj_set_style_bg_color(lv_scr_act(), lv_color_hex(0xFFFFFF), LV_PART_MAIN);
	lv_obj_set_style_text_color(lv_scr_act(), lv_color_hex(0x000000), LV_PART_MAIN);
	lv_obj_set_style_text_font(lv_scr_act(), &lvgl_rethinksans_bold_16, LV_PART_MAIN);
	lv_obj_set_style_pad_left(lv_scr_act(), 0, LV_PART_MAIN);
	lv_obj_set_style_pad_right(lv_scr_act(), 0, LV_PART_MAIN);
	lv_obj_set_style_pad_top(lv_scr_act(), 0, LV_PART_MAIN);
	lv_obj_set_style_pad_bottom(lv_scr_act(), 0, LV_PART_MAIN);

	lv_obj_set_scrollbar_mode(lv_scr_act(), LV_SCROLLBAR_MODE_OFF);

	lv_obj_t * container = lv_obj_create(lv_scr_act());
	lv_obj_set_size(container, LCD_QSPI_V_RES, LCD_QSPI_H_RES);
	lv_obj_align_to(container, lv_scr_act(), LV_ALIGN_TOP_LEFT, 0, 0);
	lv_obj_set_flex_flow(container, LV_FLEX_FLOW_COLUMN);
	lv_obj_set_flex_align(container, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
	lv_obj_set_style_pad_left(container, 0, LV_PART_MAIN);
	lv_obj_set_style_pad_right(container, 0, LV_PART_MAIN);
	lv_obj_set_style_pad_top(container, 0, LV_PART_MAIN);
	lv_obj_set_style_pad_bottom(container, 0, LV_PART_MAIN);
	lv_obj_set_scrollbar_mode(container, LV_SCROLLBAR_MODE_OFF);

	lv_obj_t * top_bar = lv_obj_create(container);
	lv_obj_set_size(top_bar, LCD_QSPI_V_RES, 40);
	lv_obj_align(top_bar, LV_ALIGN_TOP_MID, 0, 5);
	lv_obj_set_flex_flow(top_bar, LV_FLEX_FLOW_ROW);
	lv_obj_set_flex_align(top_bar, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
	lv_obj_set_style_border_width(top_bar, 1, LV_PART_MAIN);
	lv_obj_set_style_border_color(top_bar, lv_color_black(), LV_PART_MAIN);
	lv_obj_set_style_border_side(top_bar, LV_BORDER_SIDE_BOTTOM, LV_PART_MAIN);
	lv_obj_set_style_pad_left(top_bar, 20, LV_PART_MAIN);
	lv_obj_set_style_pad_right(top_bar, 20, LV_PART_MAIN);
	lv_obj_set_scrollbar_mode(top_bar, LV_SCROLLBAR_MODE_OFF);

	time_label = lv_label_create(top_bar);
	lv_obj_align(time_label, LV_ALIGN_OUT_LEFT_MID, 0, 0);
	battery_label = lv_label_create(top_bar);
	lv_obj_align(battery_label, LV_ALIGN_OUT_RIGHT_MID, 0, 0);

	speed_label = lv_label_create(container);
	lv_obj_align(speed_label, LV_ALIGN_CENTER, 0, 0);
	lv_obj_set_style_text_font(speed_label, &lvgl_rethinksans_bold_200, LV_PART_MAIN);
	lv_obj_set_style_text_color(speed_label, lv_color_hex(0x000000), LV_PART_MAIN);

	lv_obj_t * bottom_bar = lv_obj_create(container);
	lv_obj_set_size(bottom_bar, LCD_QSPI_V_RES, 40);
	lv_obj_align(bottom_bar, LV_ALIGN_BOTTOM_MID, 0, 5);
	lv_obj_set_flex_flow(bottom_bar, LV_FLEX_FLOW_ROW);
	lv_obj_set_flex_align(bottom_bar, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
	lv_obj_set_style_border_width(bottom_bar, 1, LV_PART_MAIN);
	lv_obj_set_style_border_color(bottom_bar, lv_color_black(), LV_PART_MAIN);
	lv_obj_set_style_border_side(bottom_bar, LV_BORDER_SIDE_TOP, LV_PART_MAIN);
	lv_obj_set_style_pad_left(bottom_bar, 20, LV_PART_MAIN);
	lv_obj_set_style_pad_right(bottom_bar, 20, LV_PART_MAIN);
	lv_obj_set_scrollbar_mode(bottom_bar, LV_SCROLLBAR_MODE_OFF);

	status_label = lv_label_create(bottom_bar);
	lv_obj_align(status_label, LV_ALIGN_OUT_LEFT_MID, 0, 0);
	units_label = lv_label_create(bottom_bar);
	lv_obj_align(units_label, LV_ALIGN_OUT_RIGHT_MID, 0, 0);

	bsp_display_unlock();
}

/**
 * Power off the display.
 */
void power_off_display_tft() {
	bsp_display_backlight_off();
}

/**
 * Clear the display.
 */
void clear_display_tft() {
	return;
}

/**
 * Draw speed in knots.
 *
 * @param speed
 */
void draw_speed_tft(float speed) {
	bsp_display_lock(0);
	lv_label_set_text_fmt(speed_label, "%0.1f", speed);
	bsp_display_unlock();
}

/**
 * Draw top status bar.
 *
 * @param text
 */
void draw_top_bar_tft() {
	return;
}

/**
 * Draw bottom status bar.
 *
 * @param text
 */
void draw_bottom_bar_tft() {
	return;
}

/**
 * Draw status text.
 *
 * @param text
 */
void draw_status_tft(String text) {
	bsp_display_lock(0);
	lv_label_set_text(status_label, text.c_str());
	bsp_display_unlock();
}

/**
 * Clear status text area.
 */
void clear_status_tft() {
	bsp_display_lock(0);
	lv_label_set_text(status_label, "");
	bsp_display_unlock();
}

/**
 * Draw battery capacity indicator.
 *
 * @param percentage Battery capacity.
 */
void draw_battery_status_tft(int percentage) {
	bsp_display_lock(0);
	lv_label_set_text_fmt(battery_label, "%d%%", percentage);
	bsp_display_unlock();
}

/**
 * Draw time.
 *
 * @param hours
 * @param minutes
 * @param seconds
 */
void draw_time_tft(int hours, int minutes, int seconds) {
	bsp_display_lock(0);
	lv_label_set_text_fmt(time_label, "%02d:%02d", hours, minutes);
	bsp_display_unlock();
}

/**
 * Draw units.
 *
 * @param text
 */
void draw_units_tft(String text) {
	bsp_display_lock(0);
	lv_label_set_text(units_label, text.c_str());
	bsp_display_unlock();
}
