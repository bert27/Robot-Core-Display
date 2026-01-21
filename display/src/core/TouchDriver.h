#ifndef TOUCH_DRIVER_H
#define TOUCH_DRIVER_H

#include <Arduino.h>
#include "Config.h"

// --- Touch Hardware Configuration ---
// (Values are inherited from Config.h)

// --- External Variables ---
extern int touch_last_x;
extern int touch_last_y;

// --- Module Interface ---
void touch_init();
bool touch_has_signal();
bool touch_touched();
bool touch_released();

#endif // TOUCH_DRIVER_H
