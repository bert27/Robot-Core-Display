/**
 * Sunton ESP32-8048S043 (4.3" 800x480)
 * Project: Senior Refactoring
 * 
 * This sketch acts as the Entry Point.
 * All hardware logic is encapsulated in DisplayManager.
 */

#include "src/core/DisplayManager.h"
#include "src/core/MemoryManager.h"

void setup() {
    // Initialize serial port
    Serial.begin(115200);
    
    // Wait up to 5 seconds for serial monitor connection
    uint32_t start_time = millis();
    while (!Serial && (millis() - start_time) < 5000) {
        delay(10);
    }
    
    Serial.println("\n\n######################################");
    Serial.println("[System] FIRMWARE BOOT OK");
    Serial.println("######################################\n");
    Serial.flush();

    // Initialize NVS (Preferences)
    MemoryManager::begin();

    // Get manager instance and start
    if (!DisplayManager::getInstance().begin()) {
        Serial.println("[System] FATAL ERROR: Could not initialize hardware");
        while(1) delay(1000); // Safety block
    }

    Serial.println("[System] Running main loop...");
}

void loop() {
    // Keep LVGL and touch events alive
    DisplayManager::getInstance().update();
}
