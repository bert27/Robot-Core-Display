#include "ESPNowManager.h"
#include "secrets.h"
#include <esp_wifi.h>
#include <esp_now.h>
#include <WiFi.h>
#include "remote_protocol.h"

// Static callback for ESP-NOW
static void onDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
    if (status == ESP_NOW_SEND_SUCCESS) {
        printf("[ESP-NOW] Delivery Success! (ACK received)\n");
    } else {
        printf("[ESP-NOW] Delivery Fail (No receiver found on this channel)\n");
    }
}

bool ESPNowManager::begin() {
    // Set device as a Wi-Fi Station
    WiFi.mode(WIFI_STA);
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

    esp_now_register_send_cb(onDataSent);
    
    printf("[ESP-NOW] Peer Broadcast Added OK\n");
    return true;
}

void ESPNowManager::sendDrinkSelection(const String& drinkName) {
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

int32_t ESPNowManager::getWiFiChannel(const char *ssid) {
    if (int32_t n = WiFi.scanNetworks()) {
        for (uint8_t i = 0; i < n; i++) {
            if (!strcmp(ssid, WiFi.SSID(i).c_str())) {
                return WiFi.channel(i);
            }
        }
    }
    return 0;
}
