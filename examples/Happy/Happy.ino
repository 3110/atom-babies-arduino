#include "Happy.h"

using namespace M5Stack_AtomBabies;

const uint8_t HEART_POS[] = {2,  4,  6,  7,  8,  9,  10, 11,
                             12, 13, 14, 15, 17, 18, 19, 23};
const size_t HEART_POS_SIZE = sizeof(HEART_POS) / sizeof(HEART_POS[0]);
const CRGB HEART_COLOR(0xFF, 0x00, 0x00);

const uint8_t* peer = EspNow::BROADCAST_ADDRESS;

AtomBabies babies;
Happy happy;
uint16_t count = 0;

void onDataReceived(const uint8_t* addr, const uint8_t* data, int len) {
    happy.onReceived(addr, data, len);
    doTouched(babies);
}

void onDataSent(const uint8_t* addr, esp_now_send_status_t status) {
    happy.onSent(addr, status);
}

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

void doPressed(AtomBabies& babies) {
    beforeDisplay(babies);
    babies.scrollDigits(CRGB::Green, count, 500);
    afterDisplay(babies);
}

void doTouched(AtomBabies& babies) {
    ++count;
    beforeDisplay(babies);
    babies.displayData(HEART_COLOR, HEART_POS, HEART_POS_SIZE);
    delay(500);
    afterDisplay(babies);
}

void setup(void) {
    babies.addPlugin(happy);
    babies.begin();
    happy.setRecvCallback(onDataReceived);
    happy.setSendCallback(onDataSent);
    happy.registerPeer(peer);
    babies.display();
    babies.startBlink();
}

void loop(void) {
    babies.update();
    if (babies.wasPressed()) {
        doPressed(babies);
    } else {
        if (babies.isTouched()) {
            happy.send("ATOM Babies: Hello", strlen("ATOM Babies: Hello"));
            doTouched(babies);
        }
    }
}