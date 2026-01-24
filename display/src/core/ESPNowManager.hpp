#ifndef ESPNOW_MANAGER_HPP
#define ESPNOW_MANAGER_HPP

#include <Arduino.h>
#include "../../secrets.h"
#include <esp_wifi.h>
#include <esp_now.h>
#include <WiFi.h>
#include "remote_protocol.hpp"

// Static callback for ESP-NOW (defined as inline to allow header-only usage)
inline void onDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
    if (status == ESP_NOW_SEND_SUCCESS) {
        printf("[ESP-NOW] Delivery Success! (ACK received)\n");
    } else {
        printf("[ESP-NOW] Delivery Fail (No receiver found on this channel)\n");
    }
}

// Global state for sync (declared as inline for header-only)
inline struct_message last_sync_data;
inline unsigned long last_sync_time = 0;
inline bool is_server_connected = false;

inline void onDataRecv(const esp_now_recv_info_t *recv_info, const uint8_t *data, int len) {
    if (len == sizeof(struct_message)) {
        struct_message msg;
        memcpy(&msg, data, sizeof(msg));
        
        if (msg.id == 101) { // Sync Response
            last_sync_data = msg;
            last_sync_time = millis();
            is_server_connected = true;
            printf("[ESP-NOW] Pump data received from server!\n");
        }
    }
}

class ESPNowManager {
public:
    static ESPNowManager& getInstance() {
        static ESPNowManager instance;
        return instance;
    }

    bool begin() {
        // Set device as a Wi-Fi Station
        WiFi.mode(WIFI_STA);
        esp_wifi_set_ps(WIFI_PS_NONE); // Disable power save to prevent display flickering
        WiFi.disconnect();

        // Sync channel with the target network defined in secrets.h
        printf("[ESP-NOW] Searching for network: %s\n", TARGET_WIFI_SSID);
        
        delay(500); 
        int32_t channel = getWiFiChannel(TARGET_WIFI_SSID);
        
        if (channel > 0) {
            printf("[ESP-NOW] Found network on channel %d. Switching radio...\n", (int)channel);
            
            esp_wifi_set_promiscuous(true);
            esp_wifi_set_channel(channel, WIFI_SECOND_CHAN_NONE);
            esp_wifi_set_promiscuous(false);
        } else {
            printf("[ESP-NOW] Network not found. Defaulting to channel 1.\n");
            channel = 1;
        }

        // Init ESP-NOW
        if (esp_now_init() != ESP_OK) {
            printf("[ESP-NOW] Error initializing ESP-NOW\n");
            return false;
        }
        
        esp_now_register_send_cb(onDataSent);
        esp_now_register_recv_cb(onDataRecv);

        printf("[ESP-NOW] Init OK. MAC: %s\n", WiFi.macAddress().c_str());
        printf("[ESP-NOW] Radio Sintonized - Channel: %d (Target: %d)\n", (int)WiFi.channel(), (int)channel);

        // Register peer
        esp_now_peer_info_t peerInfo;
        memset(&peerInfo, 0, sizeof(peerInfo));
        memcpy(peerInfo.peer_addr, broadcastAddress, 6);
        peerInfo.channel = channel; 
        peerInfo.encrypt = false;
        
        if (esp_now_add_peer(&peerInfo) != ESP_OK) {
            printf("[ESP-NOW] Failed to add peer\n");
            return false;
        }

        printf("[ESP-NOW] Peer Broadcast Added OK\n");
        return true;
    }

    void requestPumpSync() {
        struct_message msg;
        memset(&msg, 0, sizeof(msg));
        msg.id = 100; // Request Sync
        esp_now_send(broadcastAddress, (uint8_t *) &msg, sizeof(msg));
    }

    bool isConnected() {
        if (millis() - last_sync_time > 8000) is_server_connected = false;
        return is_server_connected;
    }

    struct_message& getSyncData() { return last_sync_data; }

    void sendDrinkSelection(const String& drinkName) {
        struct_message msg;
        memset(&msg, 0, sizeof(msg));
        
        msg.id = 99; 
        strncpy(msg.choose, drinkName.c_str(), sizeof(msg.choose) - 1);
        
        esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &msg, sizeof(msg));
        
        if (result == ESP_OK) {
            printf("[ESP-NOW] Command sent: %s\n", drinkName.c_str());
        } else {
            printf("[ESP-NOW] Send error!\n");
        }
    }

private:
    ESPNowManager() {}

    int32_t getWiFiChannel(const char *ssid) {
        if (int32_t n = WiFi.scanNetworks()) {
            for (uint8_t i = 0; i < n; i++) {
                if (!strcmp(ssid, WiFi.SSID(i).c_str())) {
                    return WiFi.channel(i);
                }
            }
        }
        return 0;
    }
    
    inline static uint8_t broadcastAddress[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
};

#endif // ESPNOW_MANAGER_HPP
