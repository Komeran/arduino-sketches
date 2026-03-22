#include "src/Finger.h"
#include "HandsController.h"

HandsController controller;

void readRemoteHand() {
  if (Serial.available() >= 3) {
    if (Serial.read() == 0xAA) {
      uint8_t mask = Serial.read();
      uint8_t checksum = Serial.read();

      if (checksum == (mask ^ 0xFF)) {
        updateRemoteHand(&controller, mask);
      }
    }
  }
}

void setup() {
  Serial.begin(9600);

  initHandsController(&controller);

  for(uint8_t i = 0; i < FINGER_COUNT; i++) {
    pinMode(fingers[i].pin, INPUT);
  }
}

void loop() {
  updateLocalHand(&controller);
  readRemoteHand();

  uint16_t command = getCommand(&controller);

  delay(100);
}