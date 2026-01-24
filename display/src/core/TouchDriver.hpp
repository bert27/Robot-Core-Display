#ifndef TOUCH_DRIVER_HPP
#define TOUCH_DRIVER_HPP

#include <Arduino.h>
#include "Config.hpp"
#include <Wire.h>

#if defined(TOUCH_GT911)
#include <TAMC_GT911.h>
inline TAMC_GT911 ts = TAMC_GT911(TOUCH_GT911_SDA, TOUCH_GT911_SCL, TOUCH_GT911_INT, TOUCH_GT911_RST, max(TOUCH_MAP_X1, TOUCH_MAP_X2), max(TOUCH_MAP_Y1, TOUCH_MAP_Y2));
#endif

// --- External Variables ---
inline int touch_last_x = 0;
inline int touch_last_y = 0;

// --- Module Interface ---
inline void touch_init() {
#if defined(TOUCH_GT911)
  Wire.begin(TOUCH_GT911_SDA, TOUCH_GT911_SCL);
  ts.begin();
  ts.setRotation(TOUCH_GT911_ROTATION); 
  printf("[Touch] Driver Initialized (Calibrated Mode)\n");
#endif
}

inline bool touch_has_signal() {
#if defined(TOUCH_GT911)
  return true; 
#else
  return false;
#endif
}

inline bool touch_touched() {
#if defined(TOUCH_GT911)
  ts.read();
  if (ts.isTouched) {
    int raw_x = ts.points[0].x;
    int raw_y = ts.points[0].y;

    touch_last_x = map(raw_x, TOUCH_MAP_X1, TOUCH_MAP_X2, 0, SCREEN_WIDTH - 1);
    touch_last_y = map(raw_y, TOUCH_MAP_Y1, TOUCH_MAP_Y2, 0, SCREEN_HEIGHT - 1);
    
    printf("[Touch] Raw: (%d, %d) -> Mapped: (%d, %d)\n", raw_x, raw_y, touch_last_x, touch_last_y);
    return true;
  }
#endif
  return false;
}

inline bool touch_released() {
  return true;
}

#endif // TOUCH_DRIVER_HPP
