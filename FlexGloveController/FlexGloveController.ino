#include "Finger.h"

uint8_t mask = 0b11111;

void setup() {
  Serial.begin(9600);
  for(uint8_t i = 0; i < 5; i++) {
    pinMode(fingers[i].pin, INPUT);
  }

  updateAllFingers(false, false);
}

void loop() {
  updateAllFingers(false, false);
  uint8_t newMask = getFingerBitmask(fingers);
  
  if(newMask != mask) {
    mask = newMask;
    Serial.write(mask);
  }

  delay(100);
}