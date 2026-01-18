#include "MyTitle.h"

lv_obj_t* create_custom_title(lv_obj_t* parent, const char* text) {
    lv_obj_t * label = lv_label_create(parent);
    lv_label_set_text(label, text);
    
    // Estilo
    lv_obj_set_style_text_color(label, lv_color_white(), 0);
    lv_obj_set_style_text_font(label, &lv_font_montserrat_24, 0); // Fuente estándar para títulos
    
    // Posición por defecto (se puede sobreescribir fuera si necesario, pero esto estandariza)
    lv_obj_align(label, LV_ALIGN_TOP_MID, 0, 15);
    
    return label;
}
