#include "Happy.h"

namespace M5Stack_AtomBabies {

Happy::Happy(void) : EspNow() {
}

Happy::~Happy(void) {
}

bool Happy::begin(AtomBabies& babies) {
    bool result = EspNow::begin(babies);

    return result;
}

bool Happy::update(AtomBabies& babies) {
    bool result = EspNow::update(babies);

    return result;
}

const char* Happy::getName(void) const {
    return "Happy";
}

void Happy::onReceived(const uint8_t* addr, const uint8_t* data, int len) {
    EspNow::onReceived(addr, data, len);
}

void Happy::onSent(const uint8_t* addr, esp_now_send_status_t status) {
    EspNow::onSent(addr, status);
}

}  // namespace M5Stack_AtomBabies