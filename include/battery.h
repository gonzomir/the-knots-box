#include <cmath>
#include <Arduino.h>
#include "driver/adc.h"
#include "esp_adc_cal.h"
#include "soc/adc_channel.h"

#undef ADC_WIDTH_BIT_DEFAULT
#define ADC_WIDTH_BIT_DEFAULT   ((adc_bits_width_t) ((int)ADC_WIDTH_MAX-1))

static esp_adc_cal_characteristics_t adc_chars;

void calibrate_adc();
float get_battery_voltage();
int get_battery_percents(float voltage);
