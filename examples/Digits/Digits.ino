#include "AtomBabies.h"

using namespace M5Stack_AtomBabies;

AtomBabies babies;
uint16_t count = 0;
const CRGB DIGIT_COLOR(CRGB::Green);

#define DISPLAY_SCROLL

#if defined(DISPLAY_SCROLL)
#define DISPLAY scrollDigits
const uint16_t DISPLAY_DIGITS_INTERVAL = 300;
#else
#define DISPLAY displayDigits
const uint16_t DISPLAY_DIGITS_INTERVAL = 1000;
#endif

void setup(void) {
    babies.begin();
    babies.setOrientation(OrientationUpsideDown).display();
    babies.startBlink();
}

void loop(void) {
    babies.update();
    if (babies.wasPressed()) {
        babies.stopBlink();
        babies.DISPLAY(DIGIT_COLOR, count, DISPLAY_DIGITS_INTERVAL);
        ++count;
        babies.display();
        babies.startBlink();
    }
    delay(100);
}