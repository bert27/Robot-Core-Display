#pragma once

#include <Arduino.h>

class ESPNowManager {
public:
    static ESPNowManager& getInstance() {
        static ESPNowManager instance;
        return instance;
    }

    bool begin();
    void sendDrinkSelection(const String& drinkName);

private:
    ESPNowManager() {}
    int32_t getWiFiChannel(const char *ssid);
    
    uint8_t broadcastAddress[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
};
