#include "HandsController.h"

// Initialize
void initHandsController(HandsController* controller) {
    controller->localMask = 0;
    controller->remoteMask = 0;
}

// Update local hand from sensors
void updateLocalHand(HandsController* controller, Finger fingers[FINGER_COUNT]) {
    controller->localMask = getFingerBitmask(fingers);
}

// Update remote hand from serial input
void updateRemoteHand(HandsController* controller, uint8_t remoteMask) {
    controller->remoteMask = remoteMask;
}

// Combine both into a 16-bit command
uint16_t getCommand(const HandsController* controller) {
    return ((uint16_t)controller->remoteMask << 8) | controller->localMask;
}