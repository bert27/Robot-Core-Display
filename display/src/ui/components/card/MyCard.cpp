#include "MyCard.h"

lv_obj_t* create_custom_card(lv_obj_t* parent, const void* icon_src, const char* text, int32_t width, int32_t height, lv_color_t bg_color, lv_event_cb_t event_cb, const lv_font_t * font) {
    // 1. Crear Contenedor Transparente Principal
    lv_obj_t* container = lv_obj_create(parent);
    lv_obj_set_size(container, width, height); // Altura total (Btn + Texto)
    lv_obj_set_style_bg_opa(container, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(container, 0, 0);
    lv_obj_set_style_pad_all(container, 0, 0);
    
    // Calcular altura del botón (card) vs texto
    // Asumimos que el texto ocupa ~30px abajo.
    int32_t btn_height = height - 35; 

    // 2. Crear el Botón (La "Card" coloreada)
    lv_obj_t* btn = lv_button_create(container);
    lv_obj_set_size(btn, width, btn_height);
    lv_obj_set_style_bg_color(btn, bg_color, 0);
    lv_obj_set_style_pad_all(btn, 0, 0); // Sin padding para que la imagen toque bordes si es necesario
    lv_obj_align(btn, LV_ALIGN_TOP_MID, 0, 0);

    if (event_cb) {
        lv_obj_add_event_cb(btn, event_cb, LV_EVENT_CLICKED, NULL);
    }

    // 3. Imagen dentro del botón
    if (icon_src) {
        lv_obj_t* img = lv_image_create(btn);
        lv_image_set_src(img, icon_src);
        lv_obj_remove_flag(img, LV_OBJ_FLAG_CLICKABLE);
        
        // El usuario confirmó que las imágenes son de 200x200px.
        // Queremos que la imagen "ocupe toda la card" (verticalmente) sin salirse.
        // Calculamos la escala exacta para ajustar 200px a la altura disponible (btn_height).
        // Formula: (TargetSize / SourceSize) * 256
        int32_t scale = (btn_height * 256) / 200;
        
        // Limitamos escala máxima a 256 (100%) por si el botón es gigante
        if (scale > 256) scale = 256;
        
        lv_image_set_scale(img, scale);
        lv_obj_center(img);
    }

    // 4. Texto DEBAJO del botón
    if (text) {
        lv_obj_t* label = lv_label_create(container);
        lv_label_set_text(label, text);
        if (font) {
            lv_obj_set_style_text_font(label, font, 0);
        } else {
            // Fallback font si no se pasa
             lv_obj_set_style_text_font(label, &lv_font_montserrat_16, 0); // Asumiendo que existe
        }
        
        // Estilo Texto
        lv_obj_set_style_text_color(label, lv_color_white(), 0);
        lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_CENTER, 0);
        
        // Wrap y Ancho
        lv_obj_set_width(label, width);
        // lv_label_set_long_mode(label, LV_LABEL_LONG_WRAP); // Opcional
        
        // Alinear en la parte inferior del contenedor
        lv_obj_align(label, LV_ALIGN_BOTTOM_MID, 0, 0);
    }

    return container;
}
