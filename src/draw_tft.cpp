#include <cmath>
#include <Arduino.h>

#include "lvgl.h"

#include "display.h"
#include "esp_bsp.h"
#include "lv_port.h"
#include "lv_conf.h"

#include "draw_tft.h"

LV_FONT_DECLARE(lvgl_rethinksans_bold_16)
LV_FONT_DECLARE(lvgl_rethinksans_bold_200)

lv_obj_t *speed_screen = NULL;
lv_obj_t *timer_screen = NULL;

lv_obj_t *speed_label = NULL;
lv_obj_t *timer_label = NULL;

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

	static lv_style_t style_screen;
	lv_style_init(&style_screen);

	lv_style_set_bg_color(&style_screen, lv_color_hex(0xFFFFFF));
	lv_style_set_text_color(&style_screen, lv_color_hex(0x000000));
	lv_style_set_text_font(&style_screen, &lvgl_rethinksans_bold_16);
	lv_style_set_pad_left(&style_screen, 0);
	lv_style_set_pad_right(&style_screen, 0);
	lv_style_set_pad_top(&style_screen, 0);
	lv_style_set_pad_bottom(&style_screen, 0);

	static lv_style_t style_big_label;
	lv_style_init(&style_big_label);

	lv_style_set_text_font(&style_big_label, &lvgl_rethinksans_bold_200);
	lv_style_set_text_color(&style_big_label, lv_color_hex(0x000000));
	lv_style_set_text_line_space(&style_big_label, 0);
	lv_style_set_pad_left(&style_big_label, 0);
	lv_style_set_pad_right(&style_big_label, 0);
	lv_style_set_pad_top(&style_big_label, 0);
	lv_style_set_pad_bottom(&style_big_label, 0);

	speed_screen = lv_obj_create(NULL);
	lv_obj_add_style(speed_screen, &style_screen, LV_PART_MAIN);
	lv_obj_set_scrollbar_mode(speed_screen, LV_SCROLLBAR_MODE_OFF);

	lv_obj_set_flex_flow(speed_screen, LV_FLEX_FLOW_COLUMN);
	lv_obj_set_flex_align(speed_screen, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

	speed_label = lv_label_create(speed_screen);
	lv_obj_align(speed_label, LV_ALIGN_CENTER, 0, 0);
	lv_obj_add_style(speed_label, &style_big_label, LV_PART_MAIN);

	lv_scr_load(speed_screen);

	timer_screen = lv_obj_create(NULL);
	lv_obj_add_style(timer_screen, &style_screen, LV_PART_MAIN);
	lv_obj_set_scrollbar_mode(timer_screen, LV_SCROLLBAR_MODE_OFF);

	lv_obj_set_flex_flow(timer_screen, LV_FLEX_FLOW_COLUMN);
	lv_obj_set_flex_align(timer_screen, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

	timer_label = lv_label_create(timer_screen);
	lv_obj_align(timer_label, LV_ALIGN_CENTER, 0, 0);
	lv_obj_add_style(timer_label, &style_big_label, LV_PART_MAIN);
	lv_obj_add_flag(timer_label, LV_OBJ_FLAG_CLICKABLE);
	lv_obj_add_flag(timer_label, LV_OBJ_FLAG_EVENT_BUBBLE);

	draw_start_timer_tft(300);

	lv_obj_add_event_cb(speed_screen, screen_event_cb, LV_EVENT_ALL, NULL);
	lv_obj_add_event_cb(timer_screen, screen_event_cb, LV_EVENT_ALL, NULL);

	lv_obj_t * top_bar = lv_obj_create(lv_layer_top());
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

	lv_obj_t * bottom_bar = lv_obj_create(lv_layer_top());
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
	lvgl_port_deinit();
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

/**
 * Draw start timer.
 *
 * @param seconds
 */
void draw_start_timer_tft(int seconds) {
	int minutes;
	minutes = seconds / 60;
	seconds = seconds % 60;

	bsp_display_lock(0);
	lv_label_set_text_fmt(timer_label, "%d:%02d", minutes, seconds);
	bsp_display_unlock();
}

/**
 * Guesture event handler.
 *
 * @param lv_event_t e Event object.
 */
void screen_event_cb(lv_event_t * e) {
	if (lv_event_get_code(e) == LV_EVENT_SHORT_CLICKED) {
		LV_LOG_USER("GOT CLICK EVENT");
		lv_obj_t * target = lv_event_get_target(e);
		if (target == timer_label) {
			LV_LOG_USER("GOT CLICK ON TIMER LABEL");
			do_start_timer = true;
			return;
		}
	}

	if (lv_event_get_code(e) != LV_EVENT_GESTURE) {
		return;
	}

	lv_dir_t direction = lv_indev_get_gesture_dir(lv_indev_get_act());

	switch (direction) {
		case LV_DIR_LEFT:
			if (lv_scr_act() == speed_screen) {
				lv_scr_load_anim(timer_screen, LV_SCR_LOAD_ANIM_MOVE_LEFT, 200, 0, false);
				display_mode = tkb_mode::start;
				draw_units_tft("Time to start");
				draw_start_timer_tft(300);
			} else {
				lv_scr_load_anim(speed_screen, LV_SCR_LOAD_ANIM_MOVE_LEFT, 200, 0, false);
				display_mode = tkb_mode::speed;
				draw_units_tft("SOG, Kn");
			}
			do_start_timer = false;
			start_timer_started = false;
			break;

		case LV_DIR_RIGHT:
			if (lv_scr_act() == speed_screen) {
				lv_scr_load_anim(timer_screen, LV_SCR_LOAD_ANIM_MOVE_RIGHT, 200, 0, false);
				display_mode = tkb_mode::start;
				draw_units_tft("Time to start");
				draw_start_timer_tft(300);
			} else {
				lv_scr_load_anim(speed_screen, LV_SCR_LOAD_ANIM_MOVE_RIGHT, 200, 0, false);
				display_mode = tkb_mode::speed;
				draw_units_tft("SOG, Kn");
			}
			do_start_timer = false;
			start_timer_started = false;
			break;

	}
}
