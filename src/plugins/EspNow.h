#pragma once

#include <WiFi.h>
#include <esp_now.h>

#include "plugins/AbstractAtomBabiesPlugin.h"

namespace M5Stack_AtomBabies {

class EspNow : public AbstractAtomBabiesPlugin {
public:
    static const uint8_t BROADCAST_ADDRESS[ESP_NOW_ETH_ALEN];

    EspNow(void);
    virtual ~EspNow(void);

    virtual bool begin(AtomBabies& babies);
    virtual bool update(AtomBabies& babies);
    virtual const char* getName(void) const = 0;

    virtual void init(void);
    virtual bool setChannel(const uint8_t channel);

    virtual void initPeer(const uint8_t* addr, esp_now_peer_info_t& peer);
    virtual bool registerPeer(const uint8_t* addr);
    virtual bool unregisterPeer(const uint8_t* addr);

    virtual bool setRecvCallback(esp_now_recv_cb_t cb);
    virtual bool setSendCallback(esp_now_send_cb_t cb);

    virtual size_t send(const char* data, size_t len);

    virtual const uint8_t* const getAddress(void) const;

    virtual void onReceived(const uint8_t* addr, const uint8_t* data,
                            int len) = 0;
    virtual void onSent(const uint8_t* addr, esp_now_send_status_t status) = 0;

    virtual void printMacAddress(const uint8_t* macAddr);
    virtual void dump(const uint8_t* data, size_t len, uint8_t indent,
                      uint8_t width);

private:
    uint8_t _channel;
    uint8_t _address[ESP_NOW_ETH_ALEN];
    char _buf[ESP_NOW_MAX_DATA_LEN];
};

}  // namespace M5Stack_AtomBabies