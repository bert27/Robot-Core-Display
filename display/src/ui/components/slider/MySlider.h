#ifndef MY_SLIDER_H
#define MY_SLIDER_H

#include "lvgl.h"

// Función para crear un slider personalizado
lv_obj_t* create_custom_slider(lv_obj_t* parent, int32_t min, int32_t max, int32_t width, lv_event_cb_t event_cb);

#endif // MY_SLIDER_H
