#include "Finger.h"

Finger fingers[] = {
  {"Thumb",  0, A0, false},
  {"Index",  0, A1, false},
  {"Middle", 0, A2, false},
  {"Ring",   0, A3, false},
  {"Pinky",  0, A4, false}
};

void printFinger(struct Finger *finger) {
  Serial.print(finger->name);
  Serial.print(" [");
  Serial.print(finger->pin);
  Serial.print("]: ");
  Serial.print(finger->value);
  Serial.print(" | Flexed: ");
  Serial.println(finger->isFlexed ? "YES" : "NO");
};

void updateAllFingers(bool printGesture, bool printFingers) {
  for(uint8_t i = 0; i < FINGER_COUNT; i++) {
    if(updateFinger(&fingers[i], printFingers) && printGesture) {
  
      const Gesture* g = detectGesture(fingers);
      
      Serial.print("Gesture: ");
      if (g != nullptr) {
        Serial.println(g->name);
      }
      else {
        Serial.println("NONE");
      }
    }
  }
}

bool updateFinger(struct Finger *finger, bool printIfUpdate) {
  int newValue = analogRead(finger->pin);

  if(newValue != finger->value) {
    finger->value = newValue;

    // --- Flex logic ---
    if (finger->isFlexed && newValue > STRAIGHT_THRESHOLD) {
      finger->isFlexed = false;
    }
    else if (!finger->isFlexed && newValue < FLEXED_THRESHOLD) {
      finger->isFlexed = true;
    }

    if(printIfUpdate) {
      printFinger(finger);
    }

    return true;
  }

  return false;
}

// GESTURES

Gesture gestures[] = {
  { "OPEN_HAND", 0b00000 },
  { "FIST",      0b11111 },
  { "POINT",     0b11101 },
  { "PEACE",     0b11001 },
  { "THUMBS_UP", 0b11110 }
};

const uint8_t GESTURE_COUNT = sizeof(gestures) / sizeof(gestures[0]);

const Gesture* detectGesture(Finger fingers[FINGER_COUNT]) {
  uint8_t currentMask = getFingerBitmask(fingers);

  for (uint8_t g = 0; g < GESTURE_COUNT; g++) {
    if (gestures[g].mask == currentMask) {
      return &gestures[g];
    }
  }

  return nullptr;
}

void applyGesture(const Gesture* gesture, Finger fingers[FINGER_COUNT]) {
  if (!gesture) return;

  applyFingerBitmask(gesture->mask, fingers);
}

// COMMUNICATION

uint8_t getFingerBitmask(Finger fingers[FINGER_COUNT]) {
  uint8_t mask = 0;

  for (uint8_t i = 0; i < FINGER_COUNT; i++) {
    if (fingers[i].isFlexed) {
      mask |= (1 << i);
    }
  }

  return mask;
}

void applyFingerBitmask(uint8_t mask, Finger fingers[FINGER_COUNT]) {
  for (uint8_t i = 0; i < FINGER_COUNT; i++) {
    fingers[i].isFlexed = (mask & (1 << i)) != 0;
  }
}