#include "plugins/EspNow.h"

namespace M5Stack_AtomBabies {

const uint8_t EspNow::BROADCAST_ADDRESS[ESP_NOW_ETH_ALEN] = {0xFF, 0xFF, 0xFF,
                                                             0xFF, 0xFF, 0xFF};

EspNow::EspNow(void) : _channel(0), _address{0}, _buf{0} {
    uint8_t addr[ESP_NOW_ETH_ALEN] = {0};
    esp_read_mac(addr, ESP_MAC_WIFI_STA);
    memcpy(this->_address, addr, ESP_NOW_ETH_ALEN);
}

EspNow::~EspNow(void) {
}

bool EspNow::begin(AtomBabies& babies) {
    if (!WiFi.mode(WIFI_STA)) {
        return false;
    }
    if (!WiFi.disconnect()) {
        return false;
    }
    init();
    return true;
}

bool EspNow::update(AtomBabies& babies) {
    return true;
}

void EspNow::init(void) {
    if (esp_now_init() != ESP_OK) {
        ESP.restart();
    }
}

bool EspNow::setChannel(const uint8_t channel) {
    this->_channel = channel;
    return true;
}

void EspNow::initPeer(const uint8_t* addr, esp_now_peer_info_t& peer) {
    memset(&peer, 0, sizeof(peer));
    memcpy(&(peer.peer_addr), addr, ESP_NOW_ETH_ALEN);
    peer.channel = this->_channel;
    peer.encrypt = 0;
}

bool EspNow::registerPeer(const uint8_t* addr) {
    esp_now_peer_info_t peerInfo;
    initPeer(addr, peerInfo);
    esp_err_t e = esp_now_add_peer(&peerInfo);
    return e == ESP_OK;
}

bool EspNow::unregisterPeer(const uint8_t* addr) {
    esp_err_t e = esp_now_del_peer(addr);
    return e == ESP_OK;
}

bool EspNow::setRecvCallback(esp_now_recv_cb_t cb) {
    esp_err_t e = esp_now_register_recv_cb(cb);
    return e == ESP_OK;
}

bool EspNow::setSendCallback(esp_now_send_cb_t cb) {
    esp_err_t e = esp_now_register_send_cb(cb);
    return e == ESP_OK;
}

size_t EspNow::send(const char* data, size_t len) {
    esp_err_t e = ESP_OK;
    esp_now_peer_info_t peer;
    size_t n = 0;
    for (size_t i = 0; i < len; i += n) {
        n = min(sizeof(this->_buf), len - i);
        memcpy(this->_buf, data + i, n);
        e = esp_now_fetch_peer(true, &peer);
        if (e == ESP_ERR_ESPNOW_NOT_FOUND) {  // Broadcast
            esp_now_send(BROADCAST_ADDRESS, (uint8_t*)this->_buf, n);
        } else {
            while (e == ESP_OK) {
                esp_now_send(peer.peer_addr, (uint8_t*)this->_buf, n);
                e = esp_now_fetch_peer(false, &peer);
            }
        }
    }
    return len;
}

const uint8_t* const EspNow::getAddress(void) const {
    return this->_address;
}

void EspNow::onReceived(const uint8_t* addr, const uint8_t* data, int len) {
    printMacAddress(addr);
    SERIAL_PRINT("> ");
    dump(data, len, 2, 16);
}

void EspNow::onSent(const uint8_t* addr, esp_now_send_status_t status) {
}

void EspNow::printMacAddress(const uint8_t* macAddr) {
    static char macStr[ESP_NOW_ETH_ALEN * 3 + 1] = {0};
    snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
             macAddr[0], macAddr[1], macAddr[2], macAddr[3], macAddr[4],
             macAddr[5]);
    SERIAL_PRINT(macStr);
}

void EspNow::dump(const uint8_t* data, size_t len, uint8_t indent,
                  uint8_t width) {
    SERIAL_PRINTLN();
    SERIAL_PRINTF_LN("Dump Length: %d", len);
    size_t p = 0;
    for (; p < len; ++p) {
        if (p % width == 0) {
            if (p != 0) {
                SERIAL_PRINT("  |");
                for (size_t cp = p - width; cp < p; ++cp) {
                    SERIAL_PRINTF("%c", isPrintable(data[cp]) ? data[cp] : '.');
                }
                SERIAL_PRINTLN("|");
            }
            for (size_t i = 0; i < indent; ++i) {
                SERIAL_PRINT(" ");
            }
        }
        SERIAL_PRINTF("%02x ", data[p]);
    }
    if (len % width != 0) {
        uint8_t remain = width - len % width;
        for (size_t cp = 0; cp < remain; ++cp) {
            SERIAL_PRINT("   ");
        }
        SERIAL_PRINT("  |");
        for (size_t cp = len - len % width; cp < len; ++cp) {
            SERIAL_PRINTF("%c", isPrintable(data[cp]) ? data[cp] : '.');
        }
        SERIAL_PRINTLN("|");
    }
}

}  // namespace M5Stack_AtomBabies