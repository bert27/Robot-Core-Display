#include "MySlider.h"

lv_obj_t* create_custom_slider(lv_obj_t* parent, int32_t min, int32_t max, int32_t width, lv_event_cb_t event_cb) {
    lv_obj_t* slider = lv_slider_create(parent);
    lv_slider_set_range(slider, min, max);
    lv_slider_set_range(slider, min, max);
    lv_slider_set_range(slider, min, max);
    lv_obj_set_width(slider, width);
    lv_obj_set_height(slider, 45); // Increased height (User Request)
    lv_obj_set_ext_click_area(slider, 20); // Extender área táctil
    
    if (event_cb) {
        lv_obj_add_event_cb(slider, event_cb, LV_EVENT_VALUE_CHANGED, NULL);
    }

    // Estilo "Chunky" (Grande)
    // Fondo más grueso
    lv_obj_set_style_bg_opa(slider, LV_OPA_COVER, LV_PART_MAIN);
    lv_obj_set_style_bg_color(slider, lv_color_hex(0x404040), LV_PART_MAIN);
    lv_obj_set_style_pad_ver(slider, 15, LV_PART_MAIN); // 15px pad -> 45px height - 30px = 15px track
    lv_obj_set_style_pad_hor(slider, 22, LV_PART_MAIN); // 22px pad hor -> Extra space to prevent knob clipping
    // Radio del fondo
    lv_obj_set_style_radius(slider, 20, LV_PART_MAIN); // Redondeado completo
    
    // Indicador (Barra rellena)
    lv_obj_set_style_bg_color(slider, lv_color_hex(0x00FF00), LV_PART_INDICATOR); // Verde por defecto

    // Knob (Bola) - Hacerla grande
    lv_obj_set_style_bg_color(slider, lv_color_white(), LV_PART_KNOB);
    lv_obj_set_style_pad_all(slider, 6, LV_PART_KNOB); // Increased padding (6px) for larger knob
    lv_obj_set_style_radius(slider, LV_RADIUS_CIRCLE, LV_PART_KNOB);

    return slider;
}
