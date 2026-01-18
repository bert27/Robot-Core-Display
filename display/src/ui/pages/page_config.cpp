#include "page_config.h"
#include "../components/MyButton.h"
#include "../components/slider/MySlider.h"
#include "../components/MyTitle.h"
#include "../assets/icons.h"
#include "../../core/MemoryManager.h"
#include <stdio.h>

// Variables de configuración de tiempos (ms)
static int time_sob = 1000;
static int time_psm = 1000;
static int time_coka = 1000;
static int time_vodka = 1000;
static int time_ron = 1000;
static int time_gin = 1000;

// Claves NVS
static const char* KEY_SOB = "sob";
static const char* KEY_PSM = "psm";
static const char* KEY_COKA = "coka";
static const char* KEY_VODKA = "vodka";
static const char* KEY_RON = "ron";
static const char* KEY_GIN = "gin";

struct ConfigData {
    lv_obj_t* label_val; // Solo necesitamos actualizar el valor
    const char* key;
    int* val_ptr; 
};

// Cargar valores desde NVS
static void load_settings() {
    time_sob = MemoryManager::getInt(KEY_SOB, 1000);
    time_psm = MemoryManager::getInt(KEY_PSM, 1000);
    time_coka = MemoryManager::getInt(KEY_COKA, 1000);
    time_vodka = MemoryManager::getInt(KEY_VODKA, 1000);
    time_ron = MemoryManager::getInt(KEY_RON, 1000);
    time_gin = MemoryManager::getInt(KEY_GIN, 1000);
}

// Callback para actualizar label y guardar en NVS
static void config_slider_event_cb(lv_event_t * e) {
    lv_obj_t * slider = (lv_obj_t *)lv_event_get_target(e);
    ConfigData * data = (ConfigData *)lv_event_get_user_data(e);
    
    if (!data) return;

    int val = lv_slider_get_value(slider);
    
    // 1. Actualizar Label Valor
    if (data->label_val) {
        lv_label_set_text_fmt(data->label_val, "%d ms", val);
    }

    // 2. Actualizar variable estática
    if (data->val_ptr) {
        *(data->val_ptr) = val;
    }

    // 3. Guardar en NVS (Solo al soltar)
    if (lv_event_get_code(e) == LV_EVENT_RELEASED) {
        if (data->key) {
            MemoryManager::saveInt(data->key, val);
            printf("Guardado NVS [%s]: %d\n", data->key, val);
        }
    }
}

// Limpieza de memoria del struct
static void slider_cleanup_cb(lv_event_t * e) {
    ConfigData * data = (ConfigData *)lv_event_get_user_data(e);
    if (data) {
        delete data;
    }
}

static void create_config_slider(lv_obj_t * parent, const char * name, int* val_ptr, const char* key) {
    lv_obj_t * cont = lv_obj_create(parent);
    lv_obj_set_width(cont, LV_PCT(48)); 
    lv_obj_set_height(cont, LV_SIZE_CONTENT);
    lv_obj_set_style_bg_opa(cont, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(cont, 0, 0);
    lv_obj_set_style_pad_all(cont, 0, 0); // Padding mínimo del contenedor
    lv_obj_set_style_pad_bottom(cont, 10, 0); // Separación con el de abajo
    lv_obj_set_flex_flow(cont, LV_FLEX_FLOW_COLUMN);
    
    // --- Header de Texto (Nombre Izq | Valor Der) ---
    lv_obj_t * header_cont = lv_obj_create(cont);
    lv_obj_set_width(header_cont, LV_PCT(100));
    lv_obj_set_height(header_cont, LV_SIZE_CONTENT);
    lv_obj_set_style_bg_opa(header_cont, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(header_cont, 0, 0);
    lv_obj_set_style_pad_all(header_cont, 0, 0);
    lv_obj_set_flex_flow(header_cont, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(header_cont, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_END, LV_FLEX_ALIGN_END); // Alinear abajo para que coincidan las bases

    // Label Nombre
    lv_obj_t * lbl_name = lv_label_create(header_cont);
    lv_label_set_text(lbl_name, name);
    lv_obj_set_style_text_color(lbl_name, lv_color_white(), 0);
    lv_obj_set_style_text_font(lbl_name, &lv_font_montserrat_20, 0);

    // Label Valor
    lv_obj_t * lbl_val = lv_label_create(header_cont);
    lv_label_set_text_fmt(lbl_val, "%d ms", *val_ptr);
    lv_obj_set_style_text_color(lbl_val, lv_color_hex(0x00FF00), 0); // Verde a juego con slider
    lv_obj_set_style_text_font(lbl_val, &lv_font_montserrat_16, 0); // Un pelín más pequeño (No tengo bold 18 cargada, uso 16)

    // --- Slider ---
    // Crear struct de datos para este slider
    ConfigData * data = new ConfigData();
    data->label_val = lbl_val; // Guardamos ref al label del valor
    data->key = key;
    data->val_ptr = val_ptr;

    // Crear slider 
    lv_obj_t * slider = create_custom_slider(cont, 0, 5000, LV_PCT(100), NULL);
    lv_slider_set_value(slider, *val_ptr, LV_ANIM_OFF);
    
    // Añadir eventos con user_data
    lv_obj_add_event_cb(slider, config_slider_event_cb, LV_EVENT_VALUE_CHANGED, data);
    lv_obj_add_event_cb(slider, config_slider_event_cb, LV_EVENT_RELEASED, data);
    lv_obj_add_event_cb(slider, slider_cleanup_cb, LV_EVENT_DELETE, data);
}

lv_obj_t* page_config_create(lv_event_cb_t on_nav_back) {
    load_settings(); // Cargar valores guardados

    lv_obj_t* screen = lv_obj_create(NULL);
    lv_obj_set_style_bg_color(screen, lv_color_hex(0x202020), LV_PART_MAIN); 
    lv_obj_set_style_bg_opa(screen, LV_OPA_COVER, LV_PART_MAIN);

    // Título
    create_custom_title(screen, "CONFIGURAR");

    // Contenedor Scroll
    lv_obj_t * list_cont = lv_obj_create(screen);
    lv_obj_set_size(list_cont, LV_PCT(95), 320);
    lv_obj_align(list_cont, LV_ALIGN_TOP_MID, 0, 50);
    lv_obj_set_style_bg_opa(list_cont, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(list_cont, 0, 0);
    lv_obj_set_flex_flow(list_cont, LV_FLEX_FLOW_ROW_WRAP); 
    lv_obj_set_flex_align(list_cont, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);
    lv_obj_set_style_pad_row(list_cont, 10, 0); 
    lv_obj_set_style_pad_column(list_cont, 10, 0); 

    // Sliders
    create_config_slider(list_cont, "Sex on Beach", &time_sob, KEY_SOB);
    create_config_slider(list_cont, "Porn Star", &time_psm, KEY_PSM);
    create_config_slider(list_cont, "CocaCola", &time_coka, KEY_COKA);
    create_config_slider(list_cont, "Vodka", &time_vodka, KEY_VODKA);
    create_config_slider(list_cont, "Ron", &time_ron, KEY_RON);
    create_config_slider(list_cont, "Gin Tonic", &time_gin, KEY_GIN);

    // Botón Volver
    lv_obj_t * btn = create_custom_button(screen, ICON_NAV_PREV, "Volver", 180, 50, lv_color_hex(0x008000), on_nav_back, NULL);
    lv_obj_align(btn, LV_ALIGN_BOTTOM_MID, 0, -20);

    return screen;
}
