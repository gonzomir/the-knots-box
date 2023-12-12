
#include "draw.h"

void setup() {
  Serial.begin(115200);
  Serial.println();
  Serial.println("setup");

  setupDisplay();

  drawStatus("Testing speed display...");
  delay(1000);

  clearStatus();

  Serial.println("Draw speed 2.1.");
  drawSpeed(2.12345);
  delay(1000);

  Serial.println("Draw speed 1.5.");
  drawSpeed(1.4681);
  delay(1000);

  Serial.println("Draw speed 4.5.");
  drawSpeed(4.4681);
  delay(1000);

  Serial.println("Draw speed 4.8.");
  drawSpeed(4.8281);
  delay(1000);

  Serial.println("Draw speed 5.2.");
  drawSpeed(5.2);
  delay(1000);

  Serial.println("Draw speed 12.2.");
  drawSpeed(12.2);
  delay(1000);

  drawStatus("Testing completed.");

//  display.powerOff();
  Serial.println("setup done");
}

void loop() {
  // put your main code here, to run repeatedly:
}
