#pragma once

#include <Arduino.h>

#include "Debug.h"

namespace M5Stack_AtomBabies {

class AtomBabies;

class AbstractAtomBabiesPlugin {
public:
    virtual ~AbstractAtomBabiesPlugin(void) {
    }
    virtual bool begin(AtomBabies& babies) = 0;
    virtual bool update(AtomBabies& babies) = 0;
    virtual const char* getName(void) const = 0;
};

}  // namespace M5Stack_AtomBabies
