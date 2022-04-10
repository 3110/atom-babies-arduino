#pragma once

#include "AtomBabies.h"
#include "plugins/EspNow.h"

namespace M5Stack_AtomBabies {

class Happy : public EspNow {
public:
    Happy(void);
    virtual ~Happy(void);

    virtual bool begin(AtomBabies& babies);
    virtual bool update(AtomBabies& babies);

    virtual const char* getName(void) const;

    virtual void onReceived(const uint8_t* addr, const uint8_t* data, int len);
    virtual void onSent(const uint8_t* addr, esp_now_send_status_t status);
};

}  // namespace M5Stack_AtomBabies