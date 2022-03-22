#include "AtomBabies.h"

using namespace M5Stack_AtomBabies;

AtomBabies babies;
uint16_t count = 0;
const CRGB DIGIT_COLOR(CRGB::Green);
const uint16_t DISPLAY_DIGITS_INTERVAL = 1000;

void setup(void) {
    babies.begin();
    babies.setOrientation(OrientationUpsideDown).display();
    babies.startBlink();
}

void loop(void) {
    babies.update();
    if (babies.wasPressed()) {
        babies.stopBlink();
        babies.displayDigits(count, DIGIT_COLOR, DISPLAY_DIGITS_INTERVAL);
        ++count;
        babies.display();
        babies.startBlink();
    }
    delay(100);
}