#include <stdio.h>
#include <string.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_log.h>

#include <Ticker.h>

#include "lv_port.h"

static lv_disp_draw_buf_t disp_buf; // contains internal graphic buffer(s) called draw buffer(s)
static lv_disp_drv_t disp_drv;      // contains callback functions

static const char *TAG = "LvglPort";

Ticker lvgl_tick;

/**
 * Increment LVGL tick.
 */
void IRAM_ATTR tick_handler() {
	lv_tick_inc(LVGL_TICK_PERIOD_MS);
}

void lvgl_portinit(int width, int height, DispFlushCb flush_cb) {
	lv_init();
	lv_color_t *buffer1 = (lv_color_t *)heap_caps_malloc(width * height * sizeof(lv_color_t), MALLOC_CAP_SPIRAM);
	assert(buffer1);
	lv_color_t *buffer2 = (lv_color_t *)heap_caps_malloc(width * height * sizeof(lv_color_t), MALLOC_CAP_SPIRAM);
	assert(buffer2);

	lv_disp_draw_buf_init(&disp_buf, buffer1, buffer2, width * height);
	ESP_LOGI(TAG, "Register display driver to LVGL");

	lv_disp_drv_init(&disp_drv);
	disp_drv.hor_res = width;
	disp_drv.ver_res = height;
	disp_drv.flush_cb = flush_cb;
	disp_drv.full_refresh = 1;
	disp_drv.draw_buf = &disp_buf;
	lv_disp_drv_register(&disp_drv);

	lvgl_tick.attach_ms(LVGL_TICK_PERIOD_MS, tick_handler);
}

/**
 * Call LVGL timr handler and flush display.
 */
void timer_handler() {
	lv_timer_handler();
}
