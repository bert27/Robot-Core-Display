#ifndef ESPNOW_MANAGER_HPP
#define ESPNOW_MANAGER_HPP

#include <Arduino.h>
#include "../../secrets.h"
#include <esp_wifi.h>
#include <esp_now.h>
#include <WiFi.h>
#include "remote_protocol.hpp"
#include "DataManager.hpp"

// Static callback for ESP-NOW
static void onDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
    if (status == ESP_NOW_SEND_SUCCESS) {
        printf("[ESP-NOW] Delivery Success to %02X:%02X:%02X:%02X:%02X:%02X\n", 
               mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
    } else {
        printf("[ESP-NOW] Delivery Fail to %02X:%02X:%02X:%02X:%02X:%02X (No ACK received)\n", 
               mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
    }
}

// Global state for sync (static to prevent multiple definition errors)
static struct_message last_sync_data;
static unsigned long last_sync_time = 0;
static bool is_server_connected = false;
static std::function<void(const RecipeSyncData&)> on_recipe_recv_cb = nullptr;

// Conditional Signature for ESP-IDF 5.x / Arduino ESP32 v3.0+ vs Legacy
#if defined(ESP_ARDUINO_VERSION_MAJOR) && ESP_ARDUINO_VERSION_MAJOR >= 3
static void onDataRecv(const esp_now_recv_info_t *info, const uint8_t *data, int len) {
    const uint8_t *mac = info->src_addr; 
#else
static void onDataRecv(const uint8_t *mac, const uint8_t *data, int len) {
#endif
    printf("[ESP-NOW] Packet Recv from %02X:%02X:%02X:%02X:%02X:%02X | Len: %d\n", 
           mac[0], mac[1], mac[2], mac[3], mac[4], mac[5], len);
    
    if (len == sizeof(struct_message)) {
        struct_message msg;
        memcpy(&msg, data, sizeof(msg));
        printf("[ESP-NOW] Message ID: %d\n", msg.id);
        
        if (msg.id == REMOTE_CMD_SYNC_RESPONSE) { // Sync Response (Pumps)
            last_sync_data = msg;
            last_sync_time = millis();
            is_server_connected = true;
            DataManager::getInstance().updatePumps(msg.pumpValues);
            printf("[ESP-NOW] Pump data cached in DataManager.\n");
        }
        else if (msg.id == REMOTE_CMD_RECIPE_DATA) { // Recipe Part Received
            is_server_connected = true;
            last_sync_time = millis(); // Alive
            printf("[ESP-NOW] Recipe Part Recv: %s (%d/%d)\n", msg.recipeData.name, msg.recipeData.index, msg.recipeData.total);
            
            DataManager::getInstance().addRecipe(msg.recipeData);
            
            if (on_recipe_recv_cb) {
                on_recipe_recv_cb(msg.recipeData);
            }
        }
    } else {
        printf("[ESP-NOW] Size mismatch or noise. Ignoring.\n");
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
            printf("[ESP-NOW] Found network '%s' on channel %d. Switching radio...\n", TARGET_WIFI_SSID, (int)channel);
            
            esp_wifi_set_promiscuous(true);
            esp_wifi_set_channel(channel, WIFI_SECOND_CHAN_NONE);
            esp_wifi_set_promiscuous(false);
        } else {
            printf("[ESP-NOW] Network '%s' not found. Defaulting to channel 1. (Check secrets.h)\n", TARGET_WIFI_SSID);
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
        printf("[ESP-NOW] Current Radio Channel: %d (Target: %d)\n", (int)WiFi.channel(), (int)channel);

        // Register peer
        esp_now_peer_info_t peerInfo;
        memset(&peerInfo, 0, sizeof(peerInfo));
        memcpy(peerInfo.peer_addr, broadcastAddress, 6);
        peerInfo.channel = channel; 
        peerInfo.encrypt = false;
        
        if (esp_now_add_peer(&peerInfo) != ESP_OK) {
            printf("[ESP-NOW] Failed to add Peer Broadcast\n");
            return false;
        }

        printf("[ESP-NOW] Peer Broadcast Added OK\n");
        return true;
    }

    void requestPumpSync() {
        struct_message msg;
        memset(&msg, 0, sizeof(msg));
        msg.id = REMOTE_CMD_SYNC_REQUEST; // Request Sync
        esp_now_send(broadcastAddress, (uint8_t *) &msg, sizeof(msg));
    }

    void requestRecipeSync() {
        struct_message msg;
        memset(&msg, 0, sizeof(msg));
        msg.id = REMOTE_CMD_RECIPE_SYNC_REQUEST;
        esp_now_send(broadcastAddress, (uint8_t *) &msg, sizeof(msg));
        printf("[ESP-NOW] Requested Recipe Sync...\n");
    }

    void setRecipeCallback(std::function<void(const RecipeSyncData&)> cb) {
        on_recipe_recv_cb = cb;
    }

    bool isConnected() {
        if (millis() - last_sync_time > 8000) is_server_connected = false;
        return is_server_connected;
    }

    struct_message& getSyncData() { return last_sync_data; }

    void sendDrinkSelection(const String& drinkName) {
        struct_message msg;
        memset(&msg, 0, sizeof(msg));
        
        msg.id = REMOTE_CMD_DRINK_ORDER; 
        strncpy(msg.choose, drinkName.c_str(), sizeof(msg.choose) - 1);
        
        printf("[ESP-NOW] Sending Drink Order: %s ... ", drinkName.c_str());
        esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &msg, sizeof(msg));
        
        if (result == ESP_OK) {
            printf("Queued OK\n");
        } else {
            printf("Error! (Code: %d)\n", result);
        }
    }

    void sendPumpCalibration(int pumpId, int pwm, int timeMs) {
        struct_message msg;
        memset(&msg, 0, sizeof(msg));
        
        msg.id = REMOTE_CMD_PUMP_UPDATE;
        // Server expects: "pump:ID:PWM:TIME_FLOAT"
        // We have time in MS. Convert to seconds.
        float timeSec = timeMs / 1000.0f;
        
        char cmd[64];
        snprintf(cmd, sizeof(cmd), "pump:%d:%d:%.2f", pumpId, pwm, timeSec);
        
        strncpy(msg.choose, cmd, sizeof(msg.choose) - 1);
        
        printf("[ESP-NOW] Sending Pump Update: %s ... ", cmd);
        esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &msg, sizeof(msg));
        
        if (result == ESP_OK) {
            printf("OK\n");
        } else {
            printf("Error! (%d)\n", result);
        }
    }

    void sendRecipeUpdate(const RecipeSyncData& data) {
        struct_message msg;
        memset(&msg, 0, sizeof(msg));
        
        msg.id = REMOTE_CMD_RECIPE_UPDATE;
        msg.recipeData = data; // Copy data
        
        printf("[ESP-NOW] Sending Recipe Update: %s ... ", data.name);
        esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &msg, sizeof(msg));
        
        if (result == ESP_OK) {
            printf("OK\n");
        } else {
            printf("Error! (%d)\n", result);
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
    
    uint8_t broadcastAddress[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
};

#endif // ESPNOW_MANAGER_HPP
