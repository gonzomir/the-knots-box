#pragma once

#include "lvgl.h"

#define LVGL_TICK_PERIOD_MS    5
#define LVGL_TASK_MAX_DELAY_MS 500
#define LVGL_TASK_MIN_DELAY_MS 50

typedef void (*DispFlushCb)(struct _lv_disp_drv_t * disp_drv, const lv_area_t * area, lv_color_t * color_p);

void lvgl_portinit(int width, int height, DispFlushCb flush_cb);

/**
 * Call LVGL timr handler and flush display.
 */
void timer_handler();
