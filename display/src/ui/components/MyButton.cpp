#include "MyButton.h"

lv_obj_t* create_custom_button(lv_obj_t* parent, const void* icon, const char* text, int32_t width, int32_t height, lv_color_t bg_color, lv_event_cb_t event_cb, const lv_font_t * font) {
    lv_obj_t* btn = lv_button_create(parent);
    lv_obj_set_size(btn, width, height);
    lv_obj_set_style_bg_color(btn, bg_color, 0);
    
    // Use Flex Layout to align icon and text horizontally
    lv_obj_set_flex_flow(btn, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(btn, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_column(btn, 10, 0); // Space between icon and text

    if (event_cb) {
        lv_obj_add_event_cb(btn, event_cb, LV_EVENT_CLICKED, NULL);
    }

    // Icon (if exists)
    if (icon) {
        lv_obj_t* icon_img = lv_image_create(btn);
        lv_image_set_src(icon_img, icon);
        // The image adjusts automatically. If it's a symbol, it uses the default style.
        // If it's a PNG, it uses its native size.
    }

    // Text
    if (text) {
        lv_obj_t* label = lv_label_create(btn);
        lv_label_set_text(label, text);
        if (font) {
            lv_obj_set_style_text_font(label, font, 0);
        }
    }

    return btn;
}
