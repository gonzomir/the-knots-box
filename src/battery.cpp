#include <cmath>
#include <Arduino.h>

float get_battery_voltage() {
	int raw = 0;
	for (int i = 0; i < 5; i++) {
		raw += analogRead(35);
	}
	raw = raw / 5;
	float voltage = ((float)raw / 4096) * 3.2;
	Serial.println(voltage);
	// The voltage devider is 33k to 100k, so the multiplier is (100 + 33) / 100 = 1.33.
	// Actual readings show multiplier is 1.15625.
	return voltage * 1.33;
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
