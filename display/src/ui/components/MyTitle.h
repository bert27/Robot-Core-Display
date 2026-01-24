#ifndef MY_TITLE_H
#define MY_TITLE_H

#include "lvgl.h"

/**
 * @brief Creates a standardized title for the top of the screen.
 * 
 * @param parent Parent object (usually screen)
 * @param text Title text
 * @param font Optional font (default Montserrat 24)
 * @param align Optional alignment (default TOP_MID)
 * @param x_ofs Optional X offset
 * @param y_ofs Optional Y offset
 * @return lv_obj_t* Pointer to the created label object
 */
lv_obj_t* create_custom_title(lv_obj_t* parent, const char* text, const lv_font_t* font = &lv_font_montserrat_24, lv_align_t align = LV_ALIGN_TOP_MID, int32_t x_ofs = 0, int32_t y_ofs = 15);

#endif // MY_TITLE_H
