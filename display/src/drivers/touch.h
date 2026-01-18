#ifndef TOUCH_H
#define TOUCH_H

#include <Arduino.h>
#include "../core/Config.h"

// --- Touch Library Configuration ---
// (We keep the defines here so they are visible)
#define TOUCH_GT911
#define TOUCH_GT911_SCL 20
#define TOUCH_GT911_SDA 19
#define TOUCH_GT911_INT 0
#define TOUCH_GT911_RST 38
#define TOUCH_GT911_ROTATION ROTATION_NORMAL
#define TOUCH_MAP_X1 480
#define TOUCH_MAP_X2 0
#define TOUCH_MAP_Y1 272
#define TOUCH_MAP_Y2 0

// --- External Variables ---
extern int touch_last_x;
extern int touch_last_y;

// --- Module Interface ---
void touch_init();
bool touch_has_signal();
bool touch_touched();
bool touch_released();

#endif // TOUCH_H
