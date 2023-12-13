
#include <nmeaparser.h>
#include "HardwareSerial.h"

#include "draw.h"

NMEAParser parser;
HardwareSerial SerialGPS(1);

void setup() {
  Serial.begin(9600);

  setupDisplay();

  drawStatus("Waiting for GPS...");

  SerialGPS.begin(9600);
}

void loop() {
  char buffer[200];

  String data = "";
  char character;

  while (SerialGPS.available()) {
    Serial.println("SerialGPS available.");
    character = SerialGPS.read();
    data.concat(character);
  }

  Serial.println(data);

  if (parser.dispatch(data)) {
    // Guess the last parsed sentence type.
    switch(parser.getLastProcessedType()) {
        // Is it a GPRMC sentence?
      case NMEAParser::TYPE_GPRMC:
        // Show speed.

        drawSpeed(parser.last_gprmc.speed_over_ground);

        break;
      case NMEAParser::TYPE_GPGGA:
        sprintf(buffer, "Accuracy: %d m; Satellites: %s", parser.last_gpgga.hdop, parser.last_gpgga.satellites_used);
        drawStatus(buffer);
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

  delay(500);
}
