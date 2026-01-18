#include "MyFooter.h"

lv_obj_t* create_custom_footer(lv_obj_t* parent, const void* icon_src, lv_event_cb_t event_cb) {
    // Footer Container
    lv_obj_t * footer = lv_obj_create(parent);
    
    // 1. LIMPIEZA TOTAL DE ESTILOS (Experto)
    // Elimina cualquier propiedad heredada del tema (paddings ocultos, bordes, fondos default).
    lv_obj_remove_style_all(footer);
    
    // 2. Definir dimensiones y posición
    // Forzamos ancho explícito de 800px para evitar ambigüedades con PORCENTAJES y paddings padres
    lv_obj_set_size(footer, 800, 80); 
    // Alineamos a la derecha explícitamente también el contenedor
    lv_obj_align(footer, LV_ALIGN_BOTTOM_RIGHT, 0, 0);
    
    // 3. (DEBUG Eliminado) Fondo transparente final
    lv_obj_set_style_bg_opa(footer, LV_OPA_TRANSP, 0);
    

    // 4. Absolute Layout within the Footer (Safer than Flex)
    // Remove Flex to manually position the child
    // lv_obj_set_flex_flow(footer, LV_FLEX_FLOW_ROW);
    
    // Ensure 0 gaps
    lv_obj_set_style_pad_all(footer, 0, 0);
    
    // Create Clickable Image (Standard LVGL Pattern for Icons)
    // Using lv_image instead of lv_imagebutton because lv_imagebutton ignores lv_image_set_scale
    lv_obj_t *img_btn = lv_image_create(footer);
    lv_image_set_src(img_btn, icon_src);
    lv_obj_add_flag(img_btn, LV_OBJ_FLAG_CLICKABLE); // Make it behave like a button
    
    // Scale and Align
    // Image is 200px. We want ~50px.
    // Scale factor: 256 * (Target / Original) = 256 * (50 / 200) = 64
    lv_image_set_scale(img_btn, 64); 
    
    // Use ABSOLUTE positioning at the right edge
    lv_obj_align(img_btn, LV_ALIGN_RIGHT_MID, 0, 0); 
    
    // Apply negative margin to push it further right if it has transparent clamping
    lv_obj_set_style_margin_right(img_btn, -30, 0); 

    // Remove background opacity (debug) - Set to transparent for production
    lv_obj_set_style_bg_opa(img_btn, LV_OPA_TRANSP, 0); 

    // Add event callback
    if (event_cb) {
        lv_obj_add_event_cb(img_btn, event_cb, LV_EVENT_CLICKED, NULL);
    }
    

    return footer;
}
