#ifndef MY_ICON_H
#define MY_ICON_H

#include "lvgl.h"

// Función para crear un icono personalizado (wrapper de lv_label para símbolos)
lv_obj_t* create_custom_icon(lv_obj_t* parent, const char* symbol, const lv_font_t * font, lv_color_t color);

#endif // MY_ICON_H
