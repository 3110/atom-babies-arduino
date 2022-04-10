#pragma once

#include "plugins/AbstractAtomBabiesPlugin.h"

namespace M5Stack_AtomBabies {

class PIR : public AbstractAtomBabiesPlugin {
public:
    static constexpr uint8_t INPUT_PIN = 33;  // via ATOM Mate

    PIR(void);
    virtual ~PIR(void);

    virtual bool begin(AtomBabies& babies);
    virtual bool update(AtomBabies& babies);

    virtual const char* getName(void) const;

    virtual bool isDetected(void) const;

protected:
    virtual void doDetected(AtomBabies& babies, bool detected);

private:
    bool _wasDetected;
};

}  // namespace M5Stack_AtomBabies