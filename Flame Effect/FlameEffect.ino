#include <Adafruit_CircuitPlayground.h>

/*** CONSTANTS ****/

// The default average brightness of our flame effect when no effect is applied
const uint16_t RESTING_BRIGHTNESS = 10;
// The peak average brightness of our flame effect when it flares up
const uint16_t FLARE_BRIGHTNESS = 255;
// How much the average brightness of our flame effect gets reduced with each fade step after flaring up
const uint16_t FLARE_STEP = 10;
// How much total absolute acceleration must be measured before it registers as a shake, already accounted for gravity
const uint8_t SHAKE_THRESHOLD = 20;
// The intensity of randomness of our flame effect. Ranges from 0 to 255.
const uint16_t FLAME_FLICKER_INTENSITY = 100;
// The temperature threshold in °C under which it is considered cold temperature. (For blue flame effect)
const uint8_t COLD_TEMP_THRESHOLD = 20;
// For color simulation, colors are only simulated every SIMULATE_COLOR_ITERATION_OFFSET loop iterations.
const uint8_t SIMULATE_COLOR_ITERATION_OFFSET = 3;

/*** GLOBALS ***/

// The current average brightness of our flame effect
uint16_t brightness = RESTING_BRIGHTNESS;
// For color simulation, the current step in the waiting loop before we simulate the color.
uint8_t currentColorSimIteration = 0;

/*** FLAGS ***/

// Flag to track whether we're currently fading or not
bool isFadingFlare = false;
// Flag to track whether we're currently simulating the Light Spell or not
bool isInLightSpellMode = false;
// Flag to track the previous state of the Left Button
bool lastLeftButtonState = false;

void setup() {
  Serial.begin(9600);
  randomSeed(CircuitPlayground.soundSensor());

  CircuitPlayground.begin();
}

// returns true if the temperature sensor reads a value below the temperatureThreshold (in °C)
bool isCold(float temperatureThreshold = 10.0) {
  return CircuitPlayground.temperature() < temperatureThreshold;
}

// Reads motion values and checks whether the absolute motion (adjusted for gravity) exceeds the SHAKE_THRESHOLD.
bool DetectShake(float SHAKE_THRESHOLD = 5.0) {
  float absMotionX = abs(CircuitPlayground.motionX());
  float absMotionY = abs(CircuitPlayground.motionY());
  float absMotionZ = abs(CircuitPlayground.motionZ());

  return absMotionX + absMotionY + absMotionZ - 9.81 > SHAKE_THRESHOLD;
}

// Colors all 10 neopixels in a random orange (warm) or blue (cold) color value at a random brightness around the brightness parameter. Magnitude of randomness is determined by the intensity parameter which can range from 0 to 255.
void SimulateFireEffect(uint16_t intensity = 100, uint16_t brightness = 30) {
  // Cap intensity at 255
  if(intensity > 255) {
    intensity = 255;
  }
  
  // Map intensity on a scale from 0 to 1 for brightness deviation.
  float intensityRatio = intensity / 255;

  // Make sure we only simulate color every SIMULATE_COLOR_ITERATION_OFFSET loop iteration.
  bool simulateColors = false;
  if(currentColorSimIteration == SIMULATE_COLOR_ITERATION_OFFSET) {
    simulateColors = true;
    currentColorSimIteration = 0;
  }
  currentColorSimIteration++;

  // Color all neo pixels
  for(uint8_t pixeln = 0; pixeln <= 10; pixeln++) {
    if(simulateColors) {
      if(isCold(COLD_TEMP_THRESHOLD)) {
        // Set randomized blue color
        CircuitPlayground.setPixelColor(pixeln, 0, random(0, intensity), 255 - random(0, intensity));
      }
      else {
        // Set randomized orange color
        CircuitPlayground.setPixelColor(pixeln, 255 - random(0, intensity), random(0, intensity), 0);
      }
    }

    // Update and randomize brightness
    CircuitPlayground.setBrightness(brightness + random(brightness * -1 * intensityRatio, brightness * intensityRatio));
  }
}

bool SimulateLightSpell() {
  bool leftButtonState = CircuitPlayground.leftButton();

  // Only on button down, not while still pressed.
  if(!lastLeftButtonState && leftButtonState) {
    // Toggle Light Spell simulation flag
    isInLightSpellMode = !isInLightSpellMode;
  }

  lastLeftButtonState = leftButtonState;

  return isInLightSpellMode;
}

void loop() {
  if(!CircuitPlayground.slideSwitch()) {
    CircuitPlayground.clearPixels();
    brightness = RESTING_BRIGHTNESS;
    isFadingFlare = false;
    return;
  }

  if(SimulateLightSpell()) {
    isFadingFlare = true; // Make sure we fade the light out if the spell ends.
    brightness = FLARE_BRIGHTNESS;
  }
  else if(DetectShake(SHAKE_THRESHOLD)) {
    isFadingFlare = true;
    brightness = FLARE_BRIGHTNESS;
  }
  else if(isFadingFlare) {
    // Reduce the current brightness by a FLARE_STEP but not below RESTING_BRIGHTNESS
    brightness = max(RESTING_BRIGHTNESS, brightness - FLARE_STEP);
    // We're done fading if we reach the RESTING_BRIGHTNESS
    if(brightness == RESTING_BRIGHTNESS) {
      isFadingFlare = false;
    }
  }

  SimulateFireEffect(FLAME_FLICKER_INTENSITY, brightness);
  delay(20);
}