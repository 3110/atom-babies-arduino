#pragma once

#include <Arduino.h>

namespace M5Stack_AtomBabies {

class AtomBabies;

class AbstractAtomBabiesPlugin {
public:
    AbstractAtomBabiesPlugin(AtomBabies& babies) : _babies(babies) {
    }
    virtual ~AbstractAtomBabiesPlugin(void) {
    }
    virtual bool begin(void) = 0;
    virtual bool update(void) = 0;
    virtual const char* getName(void) const = 0;

protected:
    virtual AtomBabies& getBabies(void) {
        return this->_babies;
    }

private:
    AtomBabies& _babies;
};

}  // namespace M5Stack_AtomBabies
