#include "AtomBabies.h"

using namespace M5Stack_AtomBabies;

const uint8_t HEART_POS[] = {2,  4,  6,  7,  8,  9,  10, 11,
                             12, 13, 14, 15, 17, 18, 19, 23};
const size_t HEART_POS_SIZE = sizeof(HEART_POS) / sizeof(HEART_POS[0]);
const CRGB HEART_COLOR(0xFF, 0x00, 0x00);

AtomBabies babies;
uint16_t count = 0;

void beforeDisplay(AtomBabies& babies) {
    babies.stopBlink();
    delay(100);
    babies.clear();
}

void afterDisplay(AtomBabies& babies) {
    babies.clear();
    babies.display();
    babies.startBlink();
}

void setup(void) {
    babies.begin();
    babies.display();
    babies.startBlink();
}

void loop(void) {
    babies.update();
    if (babies.wasPressed()) {
        beforeDisplay(babies);
        babies.scrollDigits(CRGB::Green, count, 500);
        afterDisplay(babies);
    } else {
        if (babies.isTouched()) {
            ++count;
            beforeDisplay(babies);
            babies.displayData(HEART_COLOR, HEART_POS, HEART_POS_SIZE);
            delay(500);
            afterDisplay(babies);
        }
    }
}