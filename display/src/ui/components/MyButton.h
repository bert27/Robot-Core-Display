#ifndef MY_BUTTON_H
#define MY_BUTTON_H

#include "lvgl.h"

// Función para crear un botón personalizado
lv_obj_t* create_custom_button(lv_obj_t* parent, const void* icon, const char* text, int32_t width, int32_t height, lv_color_t bg_color, lv_event_cb_t event_cb, const lv_font_t * font);

#endif // MY_BUTTON_H
