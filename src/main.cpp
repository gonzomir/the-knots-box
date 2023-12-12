
#include "draw.h"

void setup() {
  Serial.begin(115200);
  Serial.println();
  Serial.println("setup");

  setupDisplay();

  Serial.println("Draw speed 4.5.");
  drawSpeed(4.4681);
  delay(1000);

  Serial.println("Draw speed 4.8.");
  drawSpeed(4.8281);
  delay(1000);

  Serial.println("Draw speed 5.2.");
  drawSpeed(5.2);
  delay(1000);

  Serial.println("Draw speed 5.2.");
  drawSpeed(12.2);
  delay(1000);

//  display.powerOff();
  Serial.println("setup done");
}

void loop() {
  // put your main code here, to run repeatedly:
}
