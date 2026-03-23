#ifndef FINGER_H
#define FINGER_H

#include <Arduino.h>

#define FLEXED_THRESHOLD 1000
#define STRAIGHT_THRESHOLD 700
#define FINGER_COUNT 5

struct Finger {
    char name[7];
    int value;
    int pin;
    bool isFlexed;
};

extern Finger fingers[FINGER_COUNT];

void printFinger(struct Finger *finger);
void updateAllFingers(bool printGesture, bool printFinger);
bool updateFinger(struct Finger *finger);

// GESTURES

struct Gesture {
    const char* name;
    uint8_t mask;
};

extern Gesture gestures[];
extern const uint8_t GESTURE_COUNT;

const Gesture* detectGesture(Finger fingers[FINGER_COUNT]);
void applyGesture(const Gesture* gesture, Finger fingers[FINGER_COUNT]);

// COMMUNICATION

uint8_t getFingerBitmask(Finger fingers[FINGER_COUNT]);
void applyFingerBitmask(uint8_t mask, Finger fingers[FINGER_COUNT]);

#endif