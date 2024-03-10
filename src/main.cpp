#include <sys/time.h>
#include <cstdio>
#include <string>
#include <nmeaparser.h>

#include "battery.h"
#include "draw.h"

NMEAParser parser;
bool should_sleep = false;
bool gps_is_ready = false;
suseconds_t last_battery_read = 0;

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
  detachInterrupt(digitalPinToInterrupt(33));

  clear_display();
  power_off_display();

  // GNSS off.
  digitalWrite(18, LOW);

  // Hold output pin states.
  gpio_hold_en(GPIO_NUM_18);
  gpio_deep_sleep_hold_en();

  Serial.println("Going to sleep.");
  esp_sleep_enable_ext0_wakeup(GPIO_NUM_33, 1);
  esp_deep_sleep_start();
}

/**
 * Setup routine.
 */
void setup() {
  should_sleep = false;
  esp_sleep_disable_wakeup_source(ESP_SLEEP_WAKEUP_ALL);

  // Disable the hold on output pin states.
  gpio_deep_sleep_hold_dis();
  gpio_hold_dis(GPIO_NUM_18);

  // On/off button.
  pinMode(33, INPUT_PULLDOWN);

  // Battery voltage.
  pinMode(35, INPUT);
  calibrate_adc();

  // GNSS On.
  pinMode(18, OUTPUT);
  digitalWrite(18, HIGH);

  Serial.begin(115200);

  setup_display();
  clear_display();
  draw_top_bar();
  draw_bottom_bar();

  draw_status("Waiting for GPS...");

  Serial2.begin(38400, SERIAL_8N1, 16, 17);

  attachInterrupt(digitalPinToInterrupt(33), button_pressed, FALLING);

  Serial.println("Setup complete.");
}

/**
 * Main loop.
 */
void loop() {
  if (should_sleep) {
    go_to_sleep();
    return;
  }

  char status[200] = "";

  String data = "";

  while (Serial2.available()) {
    data = Serial2.readStringUntil('\n');
    // Remove trailing newline.
    data.remove(data.length() -1);
    //Serial.println(data);

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

            // Clear display every 5 minutes.
            if (minutes % 5 == 0 && seconds == 0) {
              clear_display();
              draw_top_bar();
              draw_bottom_bar();
              last_battery_read = 0;
            }

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
            // Show speed.
            draw_speed(speed);
          }
          break;
        case NMEAParser::TYPE_GPGGA:
          gps_is_ready = (parser.lastGPGGA.satellites_used > 3);
          if (gps_is_ready) {
            // Update status.
            sprintf(status, "Sats: %d; Acc: %.2f m", parser.lastGPGGA.satellites_used, parser.lastGPGGA.hdop);
            draw_status(status);
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
      Serial.println("Failed parsing NMEA sentence.");
    }
  }

  struct timeval current_time;
  gettimeofday(&current_time, NULL);

  if (last_battery_read == 0 || current_time.tv_sec - last_battery_read > 59) {
    float voltage = get_battery_voltage();
    int battery_percents = get_battery_percents(voltage);
    //Serial.println(voltage);
    //Serial.println(battery_percents);

    if (battery_percents < 2.0) {
      go_to_sleep();
      return;
    }

    draw_battery_status(battery_percents);
    last_battery_read = current_time.tv_sec;
  }

  delay(500);
}
