#include "AtomBabies.h"

using namespace M5Stack_AtomBabies;

AtomBabies babies;

const FacePosition FACES[] = {FaceUp,   FaceTop,    FaceUp,    FaceNormal,
                              FaceDown, FaceBottom, FaceDown,  FaceNormal,
                              FaceLeft, FaceNormal, FaceRight, FaceNormal};
const size_t MAX_FACES = sizeof(FACES) / sizeof(FACES[0]);

const FaceOrientation ORIENTATIONS[] = {
    OrientationNormal,
    OrientationRight,
    OrientationUpsideDown,
    OrientationLeft,
};
const size_t MAX_ORIENTATIONS = sizeof(ORIENTATIONS) / sizeof(FaceOrientation);

const CRGB FILL_COLOR(0x00, 0x64, 0x00);

void showFaces(AtomBabies& babies, uint16_t ms) {
    for (size_t i = 0; i < MAX_FACES; ++i) {
        delay(ms);
        babies.setFace(FACES[i]).display();
    }
    delay(ms);
}

void toggleOrientation(AtomBabies& babies) {
    static size_t pos = 0;
    pos = (pos + 1) % MAX_ORIENTATIONS;
    babies.setOrientation(ORIENTATIONS[pos]).display();
}

void setup(void) {
    babies.begin();
    babies.display();
    babies.startBlink();
}

void loop(void) {
    babies.update();
    if (M5.Btn.wasPressed()) {
        babies.setFace(FaceNormal).display();
        babies.startBlink();
        showFaces(babies, 1000);
        toggleOrientation(babies);
        babies.stopBlink();
        babies.fill(FILL_COLOR);
    }
}