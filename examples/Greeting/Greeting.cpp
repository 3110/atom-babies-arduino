#include "Greeting.h"

namespace M5Stack_AtomBabies {

const CRGB Greeting::FILL_COLOR(0x00, 0x64, 0x00);

Greeting::Greeting(void) : PIR() {
}

Greeting::~Greeting(void) {
}

bool Greeting::begin(AtomBabies& babies) {
    bool result = PIR::begin(babies);
    babies.setOrientation(OrientationUpsideDown);
    babies.fill(FILL_COLOR);
    return result;
}

void Greeting::doDetected(AtomBabies& babies, bool detected) {
    PIR::doDetected(babies, detected);
    if (detected) {
        babies.bow();
    } else {
        babies.fill(FILL_COLOR);
    }
}

}  // namespace M5Stack_AtomBabies
