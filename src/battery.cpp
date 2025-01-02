#include <cmath>
#include <Arduino.h>
#include "driver/adc.h"
#include "esp_adc_cal.h"
#include "soc/adc_channel.h"

#include "battery.h"

// This fixes compile error, see https://github.com/esphome/issues/issues/2982.
#undef ADC_WIDTH_BIT_DEFAULT
#define ADC_WIDTH_BIT_DEFAULT   ((adc_bits_width_t) ((int)ADC_WIDTH_MAX-1))

/**
 * Calibrate the ADC.
 */
void calibrate_adc() {
	esp_err_t ret;

	has_calibration = false;

	ret = esp_adc_cal_check_efuse(ESP_ADC_CAL_VAL_EFUSE_VREF);
	if (ret == ESP_OK) {
		has_calibration = true;
		esp_adc_cal_characterize(ADC_UNIT_1, ADC_ATTEN_DB_12, ADC_WIDTH_BIT_DEFAULT, 0, &adc_chars);
	}
}

/**
 * Get the battery voltage.
 *
 * @return float Battery voltage.
 */
float get_battery_voltage() {
	adc1_config_channel_atten(BATTERY_STATUS_ADC_CHANNEL, ADC_ATTEN_DB_12);

	uint32_t raw = adc1_get_raw(BATTERY_STATUS_ADC_CHANNEL);
	ets_printf("Battery raw reading: %d\n", raw);

	float voltage;
	if (has_calibration) {
		voltage = esp_adc_cal_raw_to_voltage(raw, &adc_chars) / 1000.0;
	}
	else {
		voltage = raw * 3.3/4096.0;
	}

	/**
	 * The voltage devider is 100k to 100k, so the multiplier is (100 + 100) / 100 = 2.
	 * If we take into account ESP32's GPIO impedance (1.3Mohms), the resulting
	 * multiplier is (93 + 100) / 93 = 2.08, but that gives results higher than the real voltage.
	 * Empirically 2.04 gives results closer to real life (or maybe the GPIO impedance is not 1.3Mohms).
	 */
	#ifdef EINK
	return voltage * 2.04;
	#endif

	/**
	 * The voltage devider is 33k to 100k, so the multiplier is (100 + 33) / 100 = 1.33.
	 * If we take into account ESP32's GPIO impedance (1.3Mohms), the resulting
	 * multiplier is (93 + 33) / 93 = 1.36. But comparing ADC reading to measured
	 * battery voltage gives 1.76 as a multiplier.
	 */
	#ifdef TFT
	return voltage * 1.76;
	#endif
}

/**
 * Convert battery voltage to battery capacity percentage.
 *
 * @param voltage Battery voltage.
 * @return int Capacity percantege.
 */
int get_battery_percents(float voltage) {
	// This formula was calculated using samples collected from a lipo battery.
	double percents = 2808.3808 * pow(voltage, 4) - 43560.9157 * pow(voltage, 3) + 252848.5888 * pow(voltage, 2) - 650767.4615 * voltage + 626532.5703;

	// Enforce bounds, 0-100.
	percents = max(percents, 0.0);
	percents = min(percents, 100.0);

	percents = round(percents);
	return static_cast<int>(percents);
}
