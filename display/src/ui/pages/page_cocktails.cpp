#include "page_cocktails.h"
#include "../components/card/MyCard.h"
#include "../components/footer/MyFooter.h"
#include "../components/MyButton.h"
#include "../components/MyTitle.h"
#include "../assets/icons.h"
#include "../../core/ESPNowManager.hpp"

static lv_event_cb_t nav_callback = NULL;
static lv_obj_t * footer_obj = NULL;

#include "../components/modal/MyModal.hpp"

// Static buffer to hold the selected drink name between callbacks
static char selected_drink[64];

static void confirm_drink_cb(lv_event_t * e) {
    const char * drink = (const char *)lv_event_get_user_data(e);
    if (drink) {
        printf("[UI] Confirmed drink: %s\n", drink);
        
        // Send Command
        ESPNowManager::getInstance().sendDrinkSelection(drink);
    }
}

static void drink_event_cb(lv_event_t * e) {
    lv_obj_t * btn = (lv_obj_t *)lv_event_get_target(e);
    lv_obj_t * container = lv_obj_get_parent(btn);
    
    // In MyCard.cpp: child 0 is btn, child 1 is label
    lv_obj_t * label = lv_obj_get_child(container, 1); 
    
    if (label) {
        const char * drink_name = lv_label_get_text(label);
        
        // Copy to static buffer to ensure it persists comfortably
        strncpy(selected_drink, drink_name, sizeof(selected_drink) - 1);
        
        printf("[UI] Requesting drink: %s. Opening Modal.\n", selected_drink);
        
        char msg[128];
        snprintf(msg, sizeof(msg), "¿Seguro que quieres pedir un %s?", selected_drink);
        
        create_custom_modal(lv_scr_act(), "CONFIRMAR", msg, confirm_drink_cb, NULL, selected_drink);
        
    } else {
        printf("[UI] Error: Label not found in card container\n");
    }
}

lv_obj_t* page_cocktails_create(lv_event_cb_t on_nav_click) {
    nav_callback = on_nav_click;
    lv_obj_t* screen = lv_obj_create(NULL);
    
    // Ensure screen has no padding constraining content
    lv_obj_set_style_pad_all(screen, 0, 0);
    
    // Configure Dark Blue background
    lv_obj_set_style_bg_color(screen, lv_color_hex(0x000033), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(screen, LV_OPA_COVER, LV_PART_MAIN);

    // Main Title
    create_custom_title(screen, "ROBOT MIXOLOGY");

    // Grid Container
    lv_obj_t * grid_cont = lv_obj_create(screen);
    lv_obj_set_size(grid_cont, 740, 300); // Reduced height to 300 (fitted to content)
    lv_obj_align(grid_cont, LV_ALIGN_CENTER, 0, 0); // Fully centered
    lv_obj_set_style_bg_opa(grid_cont, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(grid_cont, 0, 0);
    lv_obj_set_style_pad_all(grid_cont, 0, 0); 
    lv_obj_set_scrollbar_mode(grid_cont, LV_SCROLLBAR_MODE_OFF); 
    
    static int32_t col_dsc[] = {220, 220, 220, LV_GRID_TEMPLATE_LAST};
    static int32_t row_dsc[] = {135, 135, 135, LV_GRID_TEMPLATE_LAST}; 
    lv_obj_set_grid_dsc_array(grid_cont, col_dsc, row_dsc);
    lv_obj_set_grid_align(grid_cont, LV_GRID_ALIGN_CENTER, LV_GRID_ALIGN_START); 
    lv_obj_set_style_pad_column(grid_cont, 20, 0); 
    lv_obj_set_style_pad_row(grid_cont, 20, 0); 

    // Cocktail List (Synchronized with serverEspReact)
    create_custom_card(grid_cont, ICON_COCKTAIL_COCA_COLA, "Cocacola", 220, 135, lv_color_hex(0xFF0000), drink_event_cb, &lv_font_montserrat_20);
    create_custom_card(grid_cont, ICON_COCKTAIL_GIN_TONIC, "Orange Juice", 220, 135, lv_color_hex(0xFFA500), drink_event_cb, &lv_font_montserrat_20);
    create_custom_card(grid_cont, ICON_COCKTAIL_VODKA, "Vodka shot", 220, 135, lv_color_hex(0x00FFFF), drink_event_cb, &lv_font_montserrat_20);
    create_custom_card(grid_cont, ICON_COCKTAIL_VODKA, "Vodka with Cocacola", 220, 135, lv_color_hex(0x8B0000), drink_event_cb, &lv_font_montserrat_20);
    create_custom_card(grid_cont, ICON_COCKTAIL_VODKA, "Screwdriver", 220, 135, lv_color_hex(0xFFD700), drink_event_cb, &lv_font_montserrat_20);
    create_custom_card(grid_cont, ICON_COCKTAIL_SEX_ON_BEACH, "Sex on the beach", 220, 135, lv_color_hex(0xFF1493), drink_event_cb, &lv_font_montserrat_20);
    create_custom_card(grid_cont, ICON_COCKTAIL_PORN_STAR, "Tequila sunrise", 220, 135, lv_color_hex(0xFF4500), drink_event_cb, &lv_font_montserrat_20);
    create_custom_card(grid_cont, ICON_COCKTAIL_COCA_COLA, "Shirley Temple", 220, 135, lv_color_hex(0xFF69B4), drink_event_cb, &lv_font_montserrat_20);

    for(int i=0; i<8; i++) {
         lv_obj_set_grid_cell(lv_obj_get_child(grid_cont, i), LV_GRID_ALIGN_STRETCH, i%3, 1, LV_GRID_ALIGN_STRETCH, i/3, 1);
    }

    // Footer Container using MyFooter component
    footer_obj = create_custom_footer(screen, ICON_NAV_SETTINGS, on_nav_click);

    return screen;
}
