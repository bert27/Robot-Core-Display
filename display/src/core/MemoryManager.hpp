#ifndef MEMORY_MANAGER_HPP
#define MEMORY_MANAGER_HPP

#include <Arduino.h>
#include <Preferences.h>

class MemoryManager {
public:
    static void begin() {
        preferences.begin("cocktails", false); // false = R/W mode
    }

    static void saveInt(const char* key, int value) {
        preferences.putInt(key, value);
    }

    static int getInt(const char* key, int defaultValue) {
        return preferences.getInt(key, defaultValue);
    }
    
private:
    inline static Preferences preferences;
};

#endif // MEMORY_MANAGER_HPP
