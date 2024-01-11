#include <cmath>
#include <Arduino.h>
#include "driver/adc.h"
#include "esp_adc_cal.h"
#include "soc/adc_channel.h"

#undef ADC_WIDTH_BIT_DEFAULT
#define ADC_WIDTH_BIT_DEFAULT   ((adc_bits_width_t) ((int)ADC_WIDTH_MAX-1))

static esp_adc_cal_characteristics_t adc_chars;

void calibrate_adc() {
	esp_err_t ret;

	ret = esp_adc_cal_check_efuse(ESP_ADC_CAL_VAL_EFUSE_VREF);
	if (ret == ESP_OK) {
		esp_adc_cal_characterize(ADC_UNIT_1, ADC_ATTEN_DB_11, ADC_WIDTH_BIT_DEFAULT, 0, &adc_chars);
	}
}


float get_battery_voltage() {
	adc1_config_channel_atten(ADC1_GPIO35_CHANNEL, ADC_ATTEN_DB_11);

	uint32_t raw = adc1_get_raw(ADC1_GPIO35_CHANNEL);
	float voltage = esp_adc_cal_raw_to_voltage(raw, &adc_chars) / 1000.0;

	/**
	 * The voltage devider is 100k to 100k, so the multiplier is (100 + 100) / 100 = 2.
	 * If we take into account ESP32's GPIO impedance (1.3Mohms), the resulting
	 * multiplier is (93 + 100) / 93 = 2.08, but that gives results higher than the real voltage.
	 * Empirically 2.04 gives results closer to real life (or maybe the GPIO impedance is not 1.3Mohms).
	 */
	return voltage * 2.04;
}

int get_battery_percents(float voltage) {
	// This formula was calculated using samples collected from a lipo battery.
	double percents = 2808.3808 * pow(voltage, 4) - 43560.9157 * pow(voltage, 3) + 252848.5888 * pow(voltage, 2) - 650767.4615 * voltage + 626532.5703;

	// Enforce bounds, 0-100.
	percents = max(percents, 0.0);
	percents = min(percents, 100.0);

	percents = round(percents);
	return static_cast<int>(percents);
}
