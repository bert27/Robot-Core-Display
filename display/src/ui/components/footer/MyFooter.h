#ifndef MY_FOOTER_H
#define MY_FOOTER_H

#include "lvgl.h"

/**
 * Creates a transparent footer aligned at the bottom with an action icon on the right.
 * @param parent Parent object/screen
 * @param icon_src Icon for the action button (right)
 * @param event_cb Callback when the button is pressed
 * @return Pointer to the created footer object
 */
lv_obj_t* create_custom_footer(lv_obj_t* parent, const void* icon_src, lv_event_cb_t event_cb);

#endif // MY_FOOTER_H
