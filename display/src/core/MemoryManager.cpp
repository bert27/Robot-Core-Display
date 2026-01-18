#include "MemoryManager.h"

Preferences MemoryManager::preferences;

void MemoryManager::begin() {
    preferences.begin("cocktails", false); // false = R/W mode
}

void MemoryManager::saveInt(const char* key, int value) {
    preferences.putInt(key, value);
    // putInt ya hace commit automáticamente en implementaciones recientes, pero es eficiente.
}

int MemoryManager::getInt(const char* key, int defaultValue) {
    return preferences.getInt(key, defaultValue);
}
