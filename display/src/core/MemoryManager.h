#ifndef MEMORY_MANAGER_H
#define MEMORY_MANAGER_H

#include <Arduino.h>
#include <Preferences.h>

class MemoryManager {
public:
    static void begin();
    static void saveInt(const char* key, int value);
    static int getInt(const char* key, int defaultValue);
    
private:
    static Preferences preferences;
};

#endif // MEMORY_MANAGER_H
