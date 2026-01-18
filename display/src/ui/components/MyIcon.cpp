#include "MyIcon.h"

lv_obj_t* create_custom_icon(lv_obj_t* parent, const char* symbol, const lv_font_t * font, lv_color_t color) {
    lv_obj_t* icon = lv_label_create(parent);
    lv_label_set_text(icon, symbol);
    
    // Configurar estilo
    lv_obj_set_style_text_color(icon, color, 0);
    
    if (font) {
        lv_obj_set_style_text_font(icon, font, 0);
    }

    // Por defecto no alineamos, dejamos que el padre (layout) decida
    return icon;
}
