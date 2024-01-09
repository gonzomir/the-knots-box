
#include <cstdio>
#include <nmeaparser.h>

#include "draw.h"

NMEAParser parser;
bool should_sleep = false;

void IRAM_ATTR button_pressed() {
  should_sleep = true;
}

void go_to_sleep() {
  detachInterrupt(digitalPinToInterrupt(33));

  clearDisplay();
  powerOffDisplay();

  Serial.println("Going to sleep.");
  esp_sleep_enable_ext0_wakeup(GPIO_NUM_33, 1);
  esp_deep_sleep_start();
}

void setup() {
  should_sleep = false;
  esp_sleep_disable_wakeup_source(ESP_SLEEP_WAKEUP_ALL);
  pinMode(33, INPUT_PULLDOWN);

  Serial.begin(115200);

  setupDisplay();
  clearDisplay();

  drawStatus("Waiting for GPS...");

  Serial2.begin(9600, SERIAL_8N1, 16, 17);

  attachInterrupt(digitalPinToInterrupt(33), button_pressed, FALLING);

  Serial.println("Setup complete.");
}

void loop() {
  if (should_sleep) {
    go_to_sleep();
    return;
  }

  char status[200];

  String data = "";

  while (Serial2.available()) {
    data = Serial2.readStringUntil('\n');
    // Remove trailing newline.
    data.remove(data.length() -1);
    Serial.println(data);

    const char * str = data.c_str();
    if (parser.dispatch(str)) {
      // Get the last parsed sentence type.
      switch(parser.getLastProcessedType()) {
        // Is it a GPRMC sentence?
        case NMEAParser::TYPE_GPRMC:
          // Clear display every 5 minutes.
          if (atoi(parser.last_gprmc.utc_time) % 300 == 0 ) {
            clearDisplay();
          }
          // Show speed.
          drawSpeed(parser.last_gprmc.speed_over_ground);
          break;
        case NMEAParser::TYPE_GPGGA:
          // Update status.
          sprintf(status, "Sats: %d; Acc: %d m", parser.last_gpgga.satellites_used, parser.last_gpgga.hdop);
          drawStatus(status);
          break;
        case NMEAParser::UNKNOWN:
        case NMEAParser::TYPE_PLSR2451:
        case NMEAParser::TYPE_PLSR2452:
        case NMEAParser::TYPE_PLSR2457:
        case NMEAParser::TYPE_GPGSV:
        case NMEAParser::TYPE_GPGSA:
        case NMEAParser::TYPE_HCHDG:
        case NMEAParser::TYPE_GPGLL:
        case NMEAParser::TYPE_GPVTG:
        case NMEAParser::TYPE_GPTXT:
          break;
      }

    }
  }

  delay(500);
}
