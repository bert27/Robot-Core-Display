#ifndef MY_TITLE_H
#define MY_TITLE_H

#include "lvgl.h"

/**
 * @brief Crea un título estandarizado para la parte superior de la pantalla.
 * 
 * @param parent Objeto padre (normalmente screen)
 * @param text Texto del título
 * @return lv_obj_t* Puntero al objeto label creado
 */
lv_obj_t* create_custom_title(lv_obj_t* parent, const char* text);

#endif // MY_TITLE_H
