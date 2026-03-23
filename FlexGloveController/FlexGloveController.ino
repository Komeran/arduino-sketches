#include "Finger.h"

uint8_t mask = 0b11111;

void setup() {
  Serial.begin(9600);
  for(uint8_t i = 0; i < 5; i++) {
    pinMode(fingers[i].pin, INPUT);
  }

  updateAllFingers(false, true);
}

void loop() {
  updateAllFingers(false, true);
  uint8_t newMask = getFingerBitmask(fingers);
  
  if(newMask != mask) {
    mask = newMask;
    /*uint8_t checksum = mask ^ 0xFF;

    Serial.write(0xAA);
    Serial.write(mask);
    Serial.write(checksum);*/
  }

  delay(100);
}