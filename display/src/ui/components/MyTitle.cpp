#include "MyTitle.h"

lv_obj_t* create_custom_title(lv_obj_t* parent, const char* text, const lv_font_t* font, lv_align_t align, int32_t x_ofs, int32_t y_ofs) {
    lv_obj_t * label = lv_label_create(parent);
    lv_label_set_text(label, text);
    
    // Style
    lv_obj_set_style_text_color(label, lv_color_white(), 0);
    lv_obj_set_style_text_font(label, font, 0); 
    
    // Position
    lv_obj_align(label, align, x_ofs, y_ofs);
    
    return label;
}
