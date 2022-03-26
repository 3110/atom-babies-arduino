#include "plugins/PIR.h"

#include "AtomBabies.h"

namespace M5Stack_AtomBabies {

PIR::PIR(void) : _wasDetected(false) {
}

PIR::~PIR(void) {
}

bool PIR::begin(AtomBabies& babies) {
    pinMode(INPUT_PIN, INPUT_PULLUP);
    return true;
}

bool PIR::update(AtomBabies& babies) {
    if (isDetected()) {
        if (!this->_wasDetected) {
            this->_wasDetected = true;
            doDetected(babies, this->_wasDetected);
        }
    } else {
        if (this->_wasDetected) {
            this->_wasDetected = false;
            doDetected(babies, this->_wasDetected);
        }
    }
    return true;
}

const char* PIR::getName(void) const {
    return "PIR";
}

bool PIR::isDetected(void) const {
    return digitalRead(INPUT_PIN) == 1;
}

void PIR::doDetected(AtomBabies& babies, bool detected) {
    SERIAL_PRINTF_LN("%s: %s", getName(), detected ? "Detected" : "Got Away");
}

}  // namespace M5Stack_AtomBabies