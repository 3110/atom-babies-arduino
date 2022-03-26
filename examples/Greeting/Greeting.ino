#include "AtomBabies.h"
#include "Greeting.h"

using namespace M5Stack_AtomBabies;

AtomBabies babies;
Greeting greeting;

void setup(void) {
    babies.addPlugin(greeting);
    babies.begin();
}

void loop(void) {
    babies.update();
}