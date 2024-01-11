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

void setup_display();
void power_off_display();
void clear_display();
void draw_speed(float speed);
void draw_status(String text);
void clear_status();
void draw_battery_status(int percentage);
void draw_box(uint16_t x, uint16_t y, uint16_t w, uint16_t h, bool partial);
