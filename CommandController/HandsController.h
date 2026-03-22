#ifndef HANDS_CONTROLLER_H
#define HANDS_CONTROLLER_H

#include <Arduino.h>
#include "src/Finger.h"

struct HandsController {
    uint8_t localMask;
    uint8_t remoteMask;
};

// Initialize controller
void initHandsController(HandsController* controller);

// Update masks
void updateLocalHand(HandsController* controller);
void updateRemoteHand(HandsController* controller, uint8_t remoteMask);

// Combine into 16-bit command
uint16_t getCommand(const HandsController* controller);

#endif