#include "TouchDriver.h"
#include <Wire.h>

#if defined(TOUCH_GT911)
#include <TAMC_GT911.h>
TAMC_GT911 ts = TAMC_GT911(TOUCH_GT911_SDA, TOUCH_GT911_SCL, TOUCH_GT911_INT, TOUCH_GT911_RST, max(TOUCH_MAP_X1, TOUCH_MAP_X2), max(TOUCH_MAP_Y1, TOUCH_MAP_Y2));
#endif

int touch_last_x = 0;
int touch_last_y = 0;

void touch_init() {
#if defined(TOUCH_GT911)
  Wire.begin(TOUCH_GT911_SDA, TOUCH_GT911_SCL);
  ts.begin();
  ts.setRotation(TOUCH_GT911_ROTATION);
#endif
}

bool touch_has_signal() {
#if defined(TOUCH_GT911)
  return true; 
#else
  return false;
#endif
}

bool touch_touched() {
#if defined(TOUCH_GT911)
  ts.read();
  if (ts.isTouched) {
    // Usamos las constantes de Config.h (vía TouchDriver.h)
    touch_last_x = map(ts.points[0].x, TOUCH_MAP_X1, TOUCH_MAP_X2, 0, SCREEN_WIDTH - 1);
    touch_last_y = map(ts.points[0].y, TOUCH_MAP_Y1, TOUCH_MAP_Y2, 0, SCREEN_HEIGHT - 1);
    return true;
  }
#endif
  return false;
}

bool touch_released() {
  return true;
}
