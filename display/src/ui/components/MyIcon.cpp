#include "MyIcon.h"

lv_obj_t* create_custom_icon(lv_obj_t* parent, const char* symbol, const lv_font_t * font, lv_color_t color) {
    lv_obj_t* icon = lv_label_create(parent);
    lv_label_set_text(icon, symbol);
    
    // Configure style
    lv_obj_set_style_text_color(icon, color, 0);
    
    if (font) {
        lv_obj_set_style_text_font(icon, font, 0);
    }

    // By default we don't align, we let the parent (layout) decide
    return icon;
}
