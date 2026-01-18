#ifndef MY_CARD_H
#define MY_CARD_H

#include "lvgl.h"

/**
 * Creates an interactive card with an image at the top and text at the bottom.
 * @param parent Parent object
 * @param icon_src Image source (const void* to support &img_... or symbols)
 * @param text Text to display
 * @param width Width of the component
 * @param height Height of the component
 * @param bg_color Background color
 * @param event_cb Event callback (click)
 * @param font Font for the text (optional, defaults to Montserrat 20 if used in global context or theme one)
 * @return Pointer to the created object
 */
lv_obj_t* create_custom_card(lv_obj_t* parent, const void* icon_src, const char* text, int32_t width, int32_t height, lv_color_t bg_color, lv_event_cb_t event_cb, const lv_font_t * font);

#endif // MY_CARD_H
