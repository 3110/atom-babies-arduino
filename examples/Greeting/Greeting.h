#pragma once

#include "AtomBabies.h"
#include "plugins/PIR.h"

namespace M5Stack_AtomBabies {

class Greeting : public PIR {
public:
    static const CRGB FILL_COLOR;

    Greeting(void);
    virtual ~Greeting(void);

    virtual bool begin(AtomBabies& babies);

protected:
    virtual void doDetected(AtomBabies& babies, bool detected);
};

}  // namespace M5Stack_AtomBabies
