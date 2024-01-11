#include <cmath>
#include <Arduino.h>
#include "driver/adc.h"
#include "esp_adc_cal.h"
#include "soc/adc_channel.h"

// This fixes compile error, see https://github.com/esphome/issues/issues/2982.
#undef ADC_WIDTH_BIT_DEFAULT
#define ADC_WIDTH_BIT_DEFAULT   ((adc_bits_width_t) ((int)ADC_WIDTH_MAX-1))

static esp_adc_cal_characteristics_t adc_chars;

/**
 * Calibrate the ADC.
 */
void calibrate_adc();

/**
 * Get the battery voltage.
 *
 * @return float Battery voltage.
 */
float get_battery_voltage();

/**
 * Convert battery voltage to battery capacity percentage.
 *
 * @param voltage Battery voltage.
 * @return int Capacity percantege.
 */
int get_battery_percents(float voltage);
