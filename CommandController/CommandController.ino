#include "src/Finger.h"
#include "HandsController.h"

HandsController controller;

void setup() {
  Serial.begin(9600);

  initHandsController(&controller);

  for(uint8_t i = 0; i < FINGER_COUNT; i++) {
    pinMode(fingers[i].pin, INPUT);
  }
}

void loop() {
  updateAllFingers(false, false);

  // Update local hand
  updateLocalHand(&controller, fingers);

  // Read remote hand if available
  if (Serial.available() > 0) {
    uint8_t remote = Serial.read();
    updateRemoteHand(&controller, remote);
  }

  // Build command
  uint16_t command = getCommand(&controller);

  // Debug
  Serial.print("Command: ");
  Serial.println(command, BIN);

  delay(100);
}