#ifndef MY_TITLE_H
#define MY_TITLE_H

#include "lvgl.h"

/**
 * @brief Creates a standardized title for the top of the screen.
 * 
 * @param parent Parent object (usually screen)
 * @param text Title text
 * @return lv_obj_t* Pointer to the created label object
 */
lv_obj_t* create_custom_title(lv_obj_t* parent, const char* text);

#endif // MY_TITLE_H
