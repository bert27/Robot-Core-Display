#ifndef MY_ICON_H
#define MY_ICON_H

#include "lvgl.h"

// Function to create a custom icon (wrapper around lv_label for symbols)
lv_obj_t* create_custom_icon(lv_obj_t* parent, const char* symbol, const lv_font_t * font, lv_color_t color);

#endif // MY_ICON_H
