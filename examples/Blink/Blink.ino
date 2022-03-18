#include "AtomBabies.h"

using namespace M5Stack_AtomBabies;

AtomBabies babies;

void setup(void) {
    babies.begin();
    babies.setOrientation(OrientationUpsideDown);
}

void loop(void) {
    babies.update();
    if (M5.Btn.wasPressed()) {
        babies.toggleBlink();
    }
}
