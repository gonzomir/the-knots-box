#include <sys/time.h>
#include <cstdio>
#include <string>
#include <nmeaparser.h>

#include <Ticker.h>

#include "esp_bt.h"
#include "esp_wifi.h"

#include "battery.h"
#include "draw.h"

NMEAParser parser;

bool should_sleep = false;

tkb_mode display_mode = tkb_mode::speed;

bool gps_is_ready = false;
bool do_read_gnss = false;
bool do_start_timer = false;
bool do_update_start_timer = false;

int last_fix = 0;
String last_status = "";
float last_speed = 0.0;
bool battery_read = true;
int last_battery_percents = 0;

int start_timer_time = 300;
int start_timer_start = 0;
bool start_timer_started = false;

hw_timer_t *battery_timer = NULL;

Ticker battery_ticker;
Ticker start_timer_ticker;

/**
 * On/off button unterrupt handler.
 */
void IRAM_ATTR button_pressed() {
	should_sleep = true;
}

/**
 * Put the chip in deep sleep and power off the display.
 */
void go_to_sleep() {
	detachInterrupt(digitalPinToInterrupt(POWER_BTN));
	detachInterrupt(digitalPinToInterrupt(GNSS_PPS));

	power_off_display();

	// GNSS off.
	digitalWrite(GNSS_EN, LOW);

	// Hold output pin states.
	gpio_hold_en(GNSS_EN_GPIO);
	gpio_deep_sleep_hold_en();

	ets_printf("Going to sleep.\n");
	esp_sleep_enable_ext0_wakeup(POWER_BTN_GPIO, 1);
	esp_deep_sleep_start();
}

void IRAM_ATTR read_gnss() {
	ets_printf("PPS triggered\n");
	do_read_gnss = !do_start_timer;
	last_fix = millis();
}

void IRAM_ATTR read_battery() {
	ets_printf("Battery timer triggered\n");
	battery_read = true;
}

void IRAM_ATTR update_start_timer() {
	ets_printf("Start timer triggered, passed %d\n", millis() / 1000 - start_timer_start);
	start_timer_time = 300 - (millis() / 1000 - start_timer_start);
	do_update_start_timer = true;
}

void do_speed() {
	if (do_read_gnss) {
		do_read_gnss = false;

		String data = "";

		while (Serial2.available()) {
			data = Serial2.readStringUntil('\n');
			// Remove trailing newline.
			data.remove(data.length() -1);
			//ets_printf("%s\n", data);

			const char * str = data.c_str();
			if (parser.dispatch(str)) {
				// Get the last parsed sentence type.
				switch(parser.getLastProcessedType()) {
					case NMEAParser::TYPE_GPRMC:
						if (gps_is_ready) {
							std::string gps_time = parser.lastGPRMC.utc_time;
							int hours = atoi(gps_time.substr(0, 2).c_str());
							int minutes = atoi(gps_time.substr(2, 2).c_str());
							int seconds = atoi(gps_time.substr(4, 2).c_str());

							// Clear eInk display every 5 minutes.
							#ifdef EINK
							if (minutes % 5 == 0 && seconds == 0) {
								clear_display();
								draw_top_bar();
								draw_bottom_bar();
								draw_units("SOG, Kn");
								battery_read = true;
								last_battery_percents = 0;
							}
							#endif

							draw_time(hours, minutes, seconds);
						}
						break;
					case NMEAParser::TYPE_GPVTG:
						if (gps_is_ready) {
							float speed = 0;
							if (parser.lastGPVTG.ground_speed_unit_1 == 'N') {
								speed = parser.lastGPVTG.ground_speed_1;
							} else if (parser.lastGPVTG.ground_speed_unit_2 == 'N') {
								speed = parser.lastGPVTG.ground_speed_2;
							}

							if (speed != last_speed) {
								// Show speed.
								draw_speed(speed);
								last_speed = speed;
							}
						}
						break;
					case NMEAParser::TYPE_GPGGA:
						gps_is_ready = (parser.lastGPGGA.satellites_used > 3);
						if (gps_is_ready) {
							char s[22] = "";
							// Update status.
							sprintf(s, "Sats: %d; Acc: %.2f m", parser.lastGPGGA.satellites_used, parser.lastGPGGA.hdop);
							String status = String(s);
							if (status != last_status) {
								draw_status(status);
								last_status = status;
							}
						}
						break;
					case NMEAParser::UNKNOWN:
					case NMEAParser::TYPE_PLSR2451:
					case NMEAParser::TYPE_PLSR2452:
					case NMEAParser::TYPE_PLSR2457:
					case NMEAParser::TYPE_GPGSV:
					case NMEAParser::TYPE_GPGSA:
					case NMEAParser::TYPE_HCHDG:
					case NMEAParser::TYPE_GPGLL:
					case NMEAParser::TYPE_GPTXT:
						break;
				}

			} else {
				ets_printf("Failed parsing NMEA sentence.\n");
			}
		}
	} else {
		int now = millis();
		if (now > last_fix + 30 * 1000) {
			String status = "Waiting for GPS...";
			if (status != last_status) {
				draw_status(status);
				last_status = status;
			}
			if (last_speed != 0.0) {
				draw_speed(0.0);
				last_speed = 0.0;
			}
		}
	}

	if (start_timer_ticker.active()) {
		ets_printf("Stop start timer in speed mode.\n");
		start_timer_ticker.detach();
		do_start_timer = false;
		start_timer_started = false;
	}
}

void do_start() {
	if (do_start_timer) {
		if (!start_timer_started) {
			ets_printf("Initiate start timer...\n");
			start_timer_time = 300;
			start_timer_start = millis() / 1000;
			start_timer_ticker.attach(1, update_start_timer);
			start_timer_started = true;
		}

		if (do_update_start_timer) {
			ets_printf("Update start timer, time is %d sec.\n", start_timer_time);
			draw_start_timer(start_timer_time);
			do_update_start_timer = false;
		}

		if (start_timer_time <= 0) {
			ets_printf("End start timer at %d sec.\n", start_timer_time);
			start_timer_ticker.detach();
			do_start_timer = false;
			start_timer_started = false;
		}
	}
}

/**
 * Setup routine.
 */
void setup() {
	should_sleep = false;
	esp_sleep_disable_wakeup_source(ESP_SLEEP_WAKEUP_ALL);

	// Disable WiFi and Bluetooth.
	esp_bt_controller_disable();
	esp_wifi_stop();

	// Disable the hold on output pin states.
	gpio_deep_sleep_hold_dis();
	gpio_hold_dis(GNSS_EN_GPIO);

	// On/off button.
	pinMode(POWER_BTN, INPUT_PULLDOWN);

	// Battery voltage.
	pinMode(BATTERY_STATUS, INPUT);
	calibrate_adc();

	// GNSS PPS
	pinMode(GNSS_PPS, INPUT_PULLDOWN);

	// GNSS On.
	pinMode(GNSS_EN, OUTPUT);
	digitalWrite(GNSS_EN, HIGH);

	Serial.begin(115200);

	setup_display();
	clear_display();
	draw_top_bar();
	draw_bottom_bar();
	draw_units("SOG, Kn");
	draw_speed(0.0);
	draw_time(0, 0 ,0);

	draw_status("Waiting for GPS...");

	Serial2.begin(38400, SERIAL_8N1, GNSS_RX, GNSS_TX);

	attachInterrupt(digitalPinToInterrupt(POWER_BTN), button_pressed, FALLING);
	attachInterrupt(digitalPinToInterrupt(GNSS_PPS), read_gnss, RISING);

	#ifdef EINK
	battery_timer = timerBegin(1);
	timerAttachInterrupt(battery_timer, &read_battery);
	timerAlarm(battery_timer, 1000000 * 60, true, 0);
	#endif

	#ifdef TFT
	battery_ticker.attach(60, read_battery);
	#endif

	// Print the XTAL crystal frequency
	ets_printf("XTAL Crystal Frequency: %d MHz\n", getXtalFrequencyMhz());

	// Print the CPU frequency
	ets_printf("CPU Frequency: %d MHz\n", getCpuFrequencyMhz());

	// Print the APB bus frequency
	ets_printf("APB Bus Frequency: %d MHz\n", getApbFrequency());

	ets_printf("Setup complete.\n");
}

/**
 * Main loop.
 */
void loop() {
	if (should_sleep) {
		go_to_sleep();
		return;
	}

	switch (display_mode) {
		case tkb_mode::speed:
			do_speed();
			break;

		case tkb_mode::start:
			do_start();
			break;
	}

	if (battery_read) {
		battery_read = false;

		float voltage = get_battery_voltage();
		int battery_percents = get_battery_percents(voltage);

		char buffer[4];
		sprintf(buffer, "%0.2f", voltage);
		ets_printf("Battery voltage: %s V\n", buffer);
		ets_printf("Battery capacity: %d%%\n", battery_percents);

		if (battery_percents < 2.0) {
			go_to_sleep();
			return;
		}

		if (battery_percents != last_battery_percents) {
			draw_battery_status(battery_percents);
			last_battery_percents = battery_percents;
		}
	}
}
