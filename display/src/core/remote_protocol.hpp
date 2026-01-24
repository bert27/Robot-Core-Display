#ifndef REMOTE_PROTOCOL_HPP
#define REMOTE_PROTOCOL_HPP

#include <Arduino.h>

// Shared Remote Data Structures
// Must be binary compatible with the remote-control project

struct ValuesGiroscope {
    float X;
    float Y;
};

struct JoystickData {
    char buttonState[16];
    char direction[16];
};

struct PumpSyncData {
    int pwm[4];
    float calibration[4];
};

typedef struct __attribute__((packed)) struct_message {
    int id;
    float temp;
    int idReading;
    union {
        struct {
            char choose[96];      // Command from joystick (aligned)
            char giroscope[96];    // Command from gyro (aligned)
        };
        PumpSyncData pumpValues;   // Used for ID 101 (Sync Response)
    };
    ValuesGiroscope giroscopeValues;
    JoystickData joystickValues;
} struct_message;

#endif // REMOTE_PROTOCOL_HPP
