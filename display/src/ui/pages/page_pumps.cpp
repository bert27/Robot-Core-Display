#include "page_pumps.h"
#include "../components/MyButton.h"
#include "../components/slider/MySlider.h"
#include "../components/MyTitle.h"
#include "../components/footer/MyFooter.h"
#include "../assets/icons.h"
#include "../../core/MemoryManager.hpp"
#include "../../core/ESPNowManager.hpp"
#include <stdio.h>

// Pump configuration variables
static int p1_pwm = 255;
static int p1_time = 1600; // ms per 20ml
static int p2_pwm = 255;
static int p2_time = 1600;
static int p3_pwm = 255;
static int p3_time = 1600;
static int p4_pwm = 255;
static int p4_time = 1600;

// NVS Keys
static const char* KEY_P1_PWM = "p1_pwm";
static const char* KEY_P1_TIME = "p1_time";
static const char* KEY_P2_PWM = "p2_pwm";
static const char* KEY_P2_TIME = "p2_time";
static const char* KEY_P3_PWM = "p3_pwm";
static const char* KEY_P3_TIME = "p3_time";
static const char* KEY_P4_PWM = "p4_pwm";
static const char* KEY_P4_TIME = "p4_time";

struct PumpConfigData {
    lv_obj_t* label_val; 
    const char* key;
    int* val_ptr; 
    bool is_time; // false for PWM, true for Time
};

// Load values from NVS
static void load_settings() {
    // 1. Load from Local NVS (Fallback/Mock)
    p1_pwm = MemoryManager::getInt(KEY_P1_PWM, 255);
    p1_time = MemoryManager::getInt(KEY_P1_TIME, 1600);
    p2_pwm = MemoryManager::getInt(KEY_P2_PWM, 255);
    p2_time = MemoryManager::getInt(KEY_P2_TIME, 1600);
    p3_pwm = MemoryManager::getInt(KEY_P3_PWM, 255);
    p3_time = MemoryManager::getInt(KEY_P3_TIME, 1600);
    p4_pwm = MemoryManager::getInt(KEY_P4_PWM, 255);
    p4_time = MemoryManager::getInt(KEY_P4_TIME, 1600);

    // 2. Overwrite with Server Data if connected
    if (ESPNowManager::getInstance().isConnected()) {
        struct_message& data = ESPNowManager::getInstance().getSyncData();
        p1_pwm = data.pumpValues.pwm[0];
        p1_time = (int)(data.pumpValues.calibration[0] * 1000.0f);
        p2_pwm = data.pumpValues.pwm[1];
        p2_time = (int)(data.pumpValues.calibration[1] * 1000.0f);
        p3_pwm = data.pumpValues.pwm[2];
        p3_time = (int)(data.pumpValues.calibration[2] * 1000.0f);
        p4_pwm = data.pumpValues.pwm[3];
        p4_time = (int)(data.pumpValues.calibration[3] * 1000.0f);
        printf("[Pumps] Synchronized with Server Data.\n");
    } else {
        printf("[Pumps] Server offline. Using Mock/Local data.\n");
    }
}

static void pump_slider_event_cb(lv_event_t * e) {
    lv_obj_t * slider = (lv_obj_t *)lv_event_get_target(e);
    PumpConfigData * data = (PumpConfigData *)lv_event_get_user_data(e);
    
    if (!data) return;

    int val = lv_slider_get_value(slider);
    
    // 1. Update Value Label
    if (data->label_val) {
        if (data->is_time) {
            lv_label_set_text_fmt(data->label_val, "%.1fs", (float)val / 1000.0f);
        } else {
            lv_label_set_text_fmt(data->label_val, "%d", val);
        }
    }

    // 2. Update static variable
    if (data->val_ptr) {
        *(data->val_ptr) = val;
    }

    // 3. Save to NVS (Only on release)
    if (lv_event_get_code(e) == LV_EVENT_RELEASED) {
        if (data->key) {
            MemoryManager::saveInt(data->key, val);
            printf("Saved to NVS [%s]: %d\n", data->key, val);
        }
    }
}

static void slider_cleanup_cb(lv_event_t * e) {
    PumpConfigData * data = (PumpConfigData *)lv_event_get_user_data(e);
    if (data) delete data;
}

static void create_pump_card(lv_obj_t * parent, const char * name, int* pwm_ptr, const char* pwm_key, int* time_ptr, const char* time_key) {
    lv_obj_t * card = lv_obj_create(parent);
    lv_obj_set_width(card, LV_PCT(48)); 
    lv_obj_set_height(card, LV_SIZE_CONTENT);
    lv_obj_set_style_bg_color(card, lv_color_hex(0x303030), 0);
    lv_obj_set_style_bg_opa(card, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(card, 1, 0);
    lv_obj_set_style_border_color(card, lv_color_hex(0x505050), 0);
    lv_obj_set_style_pad_all(card, 10, 0);
    lv_obj_set_flex_flow(card, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_pad_row(card, 5, 0);

    // Pump Title
    create_custom_title(card, name, &lv_font_montserrat_20, LV_ALIGN_TOP_LEFT, 0, 0);

    // --- PWM Slider Section ---
    lv_obj_t * pwm_cont = lv_obj_create(card);
    lv_obj_set_width(pwm_cont, LV_PCT(100));
    lv_obj_set_height(pwm_cont, LV_SIZE_CONTENT);
    lv_obj_set_style_bg_opa(pwm_cont, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(pwm_cont, 0, 0);
    lv_obj_set_style_pad_all(pwm_cont, 0, 0);
    
    lv_obj_t * lbl_pwm_title = lv_label_create(pwm_cont);
    lv_label_set_text(lbl_pwm_title, "PWM");
    lv_obj_set_style_text_color(lbl_pwm_title, lv_color_hex(0xAAAAAA), 0);
    lv_obj_set_style_text_font(lbl_pwm_title, &lv_font_montserrat_14, 0);
    lv_obj_align(lbl_pwm_title, LV_ALIGN_TOP_LEFT, 0, 0);

    lv_obj_t * lbl_pwm_val = lv_label_create(pwm_cont);
    lv_label_set_text_fmt(lbl_pwm_val, "%d", *pwm_ptr);
    lv_obj_set_style_text_color(lbl_pwm_val, lv_color_white(), 0);
    lv_obj_set_style_text_font(lbl_pwm_val, &lv_font_montserrat_14, 0);
    lv_obj_align(lbl_pwm_val, LV_ALIGN_TOP_RIGHT, 0, 0);

    PumpConfigData * pwm_data = new PumpConfigData();
    pwm_data->label_val = lbl_pwm_val;
    pwm_data->key = pwm_key;
    pwm_data->val_ptr = pwm_ptr;
    pwm_data->is_time = false;

    lv_obj_t * pwm_slider = create_custom_slider(card, 0, 255, LV_PCT(100), NULL);
    lv_obj_set_style_bg_color(pwm_slider, lv_palette_main(LV_PALETTE_ORANGE), LV_PART_INDICATOR); // Orange for PWM
    lv_slider_set_value(pwm_slider, *pwm_ptr, LV_ANIM_OFF);
    lv_obj_add_event_cb(pwm_slider, pump_slider_event_cb, LV_EVENT_VALUE_CHANGED, pwm_data);
    lv_obj_add_event_cb(pwm_slider, pump_slider_event_cb, LV_EVENT_RELEASED, pwm_data);
    lv_obj_add_event_cb(pwm_slider, slider_cleanup_cb, LV_EVENT_DELETE, pwm_data);

    // --- Time Slider Section ---
    lv_obj_t * time_cont = lv_obj_create(card);
    lv_obj_set_width(time_cont, LV_PCT(100));
    lv_obj_set_height(time_cont, LV_SIZE_CONTENT);
    lv_obj_set_style_bg_opa(time_cont, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(time_cont, 0, 0);
    lv_obj_set_style_pad_all(time_cont, 0, 0);
    lv_obj_set_style_margin_top(time_cont, 5, 0);

    lv_obj_t * lbl_time_title = lv_label_create(time_cont);
    lv_label_set_text(lbl_time_title, "Time(s)/20ml");
    lv_obj_set_style_text_color(lbl_time_title, lv_color_hex(0xAAAAAA), 0);
    lv_obj_set_style_text_font(lbl_time_title, &lv_font_montserrat_14, 0);
    lv_obj_align(lbl_time_title, LV_ALIGN_TOP_LEFT, 0, 0);

    lv_obj_t * lbl_time_val = lv_label_create(time_cont);
    lv_label_set_text_fmt(lbl_time_val, "%.1fs", (float)*time_ptr / 1000.0f);
    lv_obj_set_style_text_color(lbl_time_val, lv_color_white(), 0);
    lv_obj_set_style_text_font(lbl_time_val, &lv_font_montserrat_14, 0);
    lv_obj_align(lbl_time_val, LV_ALIGN_TOP_RIGHT, 0, 0);

    PumpConfigData * time_data = new PumpConfigData();
    time_data->label_val = lbl_time_val;
    time_data->key = time_key;
    time_data->val_ptr = time_ptr;
    time_data->is_time = true;

    lv_obj_t * time_slider = create_custom_slider(card, 500, 10000, LV_PCT(100), NULL);
    lv_obj_set_style_bg_color(time_slider, lv_palette_main(LV_PALETTE_BLUE), LV_PART_INDICATOR); // Blue for Time
    lv_slider_set_value(time_slider, *time_ptr, LV_ANIM_OFF);
    lv_obj_add_event_cb(time_slider, pump_slider_event_cb, LV_EVENT_VALUE_CHANGED, time_data);
    lv_obj_add_event_cb(time_slider, pump_slider_event_cb, LV_EVENT_RELEASED, time_data);
    lv_obj_add_event_cb(time_slider, slider_cleanup_cb, LV_EVENT_DELETE, time_data);
}

lv_obj_t* page_pumps_create(lv_event_cb_t on_nav_back) {
    ESPNowManager::getInstance().requestPumpSync(); // Fetch latest from server
    load_settings();

    lv_obj_t* screen = lv_obj_create(NULL);
    lv_obj_set_style_bg_color(screen, lv_color_hex(0x202020), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(screen, LV_OPA_COVER, LV_PART_MAIN);

    // Title & Connection Status
    create_custom_title(screen, "PUMP CONFIG");
    
    lv_obj_t * conn_icon = lv_label_create(screen);
    if (ESPNowManager::getInstance().isConnected()) {
        lv_label_set_text(conn_icon, LV_SYMBOL_WIFI " Online");
        lv_obj_set_style_text_color(conn_icon, lv_color_hex(0x00FF00), 0);
    } else {
        lv_label_set_text(conn_icon, LV_SYMBOL_WARNING " Offline (Mock)");
        lv_obj_set_style_text_color(conn_icon, lv_color_hex(0xFF8800), 0);
    }
    lv_obj_align(conn_icon, LV_ALIGN_TOP_RIGHT, -20, 15);
    lv_obj_set_style_text_font(conn_icon, &lv_font_montserrat_14, 0);

    lv_obj_t * grid_cont = lv_obj_create(screen);
    lv_obj_set_size(grid_cont, LV_PCT(95), 330);
    lv_obj_align(grid_cont, LV_ALIGN_TOP_MID, 0, 50);
    lv_obj_set_style_bg_opa(grid_cont, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(grid_cont, 0, 0);
    lv_obj_set_flex_flow(grid_cont, LV_FLEX_FLOW_ROW_WRAP);
    lv_obj_set_flex_align(grid_cont, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);
    lv_obj_set_style_pad_row(grid_cont, 10, 0);
    lv_obj_set_style_pad_column(grid_cont, 10, 0);

    create_pump_card(grid_cont, "Cocacola", &p1_pwm, KEY_P1_PWM, &p1_time, KEY_P1_TIME);
    create_pump_card(grid_cont, "Orange Juice", &p2_pwm, KEY_P2_PWM, &p2_time, KEY_P2_TIME);
    create_pump_card(grid_cont, "Vodka", &p3_pwm, KEY_P3_PWM, &p3_time, KEY_P3_TIME);
    create_pump_card(grid_cont, "Grenadine", &p4_pwm, KEY_P4_PWM, &p4_time, KEY_P4_TIME);

    // Footer using centralized component
    create_simple_nav_footer(screen, on_nav_back);

    return screen;
}
