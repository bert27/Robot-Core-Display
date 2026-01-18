#include "MyTitle.h"

lv_obj_t* create_custom_title(lv_obj_t* parent, const char* text) {
    lv_obj_t * label = lv_label_create(parent);
    lv_label_set_text(label, text);
    
    // Style
    lv_obj_set_style_text_color(label, lv_color_white(), 0);
    lv_obj_set_style_text_font(label, &lv_font_montserrat_24, 0); // Standard font for titles
    
    // Default position (can be overridden outside if necessary, but this standardizes it)
    lv_obj_align(label, LV_ALIGN_TOP_MID, 0, 15);
    
    return label;
}
