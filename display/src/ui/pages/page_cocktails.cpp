#include "page_cocktails.h"
#include "../components/card/MyCard.h"
#include "../components/footer/MyFooter.h"
#include "../components/MyButton.h"
#include "../components/MyTitle.h"
#include "../assets/icons.h"
#include "../../core/ESPNowManager.hpp"
#include "../../core/DataManager.hpp"

static lv_event_cb_t nav_callback = NULL;
static lv_obj_t* grid_cocktails_cont = NULL;
static lv_timer_t* cocktails_refresh_timer = NULL;
static lv_timer_t* sync_retry_timer = NULL;
static lv_timer_t* status_timer = NULL;
static unsigned long last_processed_update = 0;

#include "../components/modal/MyModal.hpp"

// Static buffer to hold the selected drink name between callbacks
static char selected_drink[64];

static void confirm_drink_cb(lv_event_t * e) {
    const char * drink = (const char *)lv_event_get_user_data(e);
    printf("[UI] Confirm button pressed in Modal!\n");
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
        snprintf(msg, sizeof(msg), "Seguro que quieres pedir un %s?", selected_drink);
        
        create_custom_modal(lv_scr_act(), "CONFIRMAR", msg, confirm_drink_cb, NULL, selected_drink);
        
    } else {
        printf("[UI] Error: Label not found in card container\n");
    }
}

static void refresh_grid() {
    if (!grid_cocktails_cont) return;
    lv_obj_clean(grid_cocktails_cont);

    const auto& recipes = DataManager::getInstance().getRecipes();
    for(size_t i=0; i < recipes.size(); i++) {
        const auto& r = recipes[i];
        create_custom_card(grid_cocktails_cont, r.icon, r.name.c_str(), 220, 135, lv_color_hex(r.color), drink_event_cb, &lv_font_montserrat_20);
    }
}

static void refresh_timer_cb(lv_timer_t * t) {
    if (DataManager::getInstance().getLastUpdate() > last_processed_update) {
        printf("[Cocktails] Data update detected. Refreshing...\n");
        last_processed_update = DataManager::getInstance().getLastUpdate();
        refresh_grid();
    }
}



static void sync_retry_timer_cb(lv_timer_t * t) {
    // If we are still using mocks, keep asking for real data
    if (DataManager::getInstance().isUsingMocks()) {
        printf("[Cocktails] Still using mocks. Retrying Sync Request...\n");
        ESPNowManager::getInstance().requestRecipeSync();
        ESPNowManager::getInstance().requestPumpSync();
    } else {
        // We have real data! Stop retrying.
        printf("[Cocktails] Real data detected. Stopping Sync Retry Timer.\n");
        lv_timer_del(t);
        sync_retry_timer = NULL;
    }
}

static void status_timer_cb(lv_timer_t * t) {
    lv_obj_t * icon = (lv_obj_t *)lv_timer_get_user_data(t);
    if (!icon) return;

    // Logic Update: Priority is Data Source, not just Link Heartbeat.
    // If we have real data (!usingMocks), we are effectively "Online" for the user.
    if (!DataManager::getInstance().isUsingMocks()) {
        lv_label_set_text(icon, LV_SYMBOL_WIFI " Online");
        lv_obj_set_style_text_color(icon, lv_color_hex(0x00FF00), 0);
    } else {
        lv_label_set_text(icon, LV_SYMBOL_WARNING " Offline (Mock)");
        lv_obj_set_style_text_color(icon, lv_color_hex(0xFF8800), 0);
    }
}

static void page_cocktails_delete_cb(lv_event_t * e) {
    printf("[Cocktails] Cleaning up timers.\n");
    if (cocktails_refresh_timer) { lv_timer_del(cocktails_refresh_timer); cocktails_refresh_timer = NULL; }
    if (sync_retry_timer) { lv_timer_del(sync_retry_timer); sync_retry_timer = NULL; }
    if (status_timer) { lv_timer_del(status_timer); status_timer = NULL; }
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

    // Connection Status Icon
    lv_obj_t * conn_icon = lv_label_create(screen);
    lv_obj_align(conn_icon, LV_ALIGN_TOP_RIGHT, -20, 15);
    lv_obj_set_style_text_font(conn_icon, &lv_font_montserrat_14, 0);

    // Initial label
    if (ESPNowManager::getInstance().isConnected()) {
        lv_label_set_text(conn_icon, LV_SYMBOL_WIFI " Online");
        lv_obj_set_style_text_color(conn_icon, lv_color_hex(0x00FF00), 0);
    } else {
        lv_label_set_text(conn_icon, LV_SYMBOL_WARNING " Offline (Mock)");
        lv_obj_set_style_text_color(conn_icon, lv_color_hex(0xFF8800), 0);
    }

    if (status_timer) lv_timer_del(status_timer);
    status_timer = lv_timer_create(status_timer_cb, 500, conn_icon);

    // Grid Container
    // Grid/Flex Container
    grid_cocktails_cont = lv_obj_create(screen);
    lv_obj_set_size(grid_cocktails_cont, 740, 300); 
    lv_obj_align(grid_cocktails_cont, LV_ALIGN_CENTER, 0, 0); 
    lv_obj_set_style_bg_opa(grid_cocktails_cont, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(grid_cocktails_cont, 0, 0);
    lv_obj_set_style_pad_all(grid_cocktails_cont, 10, 0); // Padding for aesthetics
    
    // Enable Scrolling
    lv_obj_add_flag(grid_cocktails_cont, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_scrollbar_mode(grid_cocktails_cont, LV_SCROLLBAR_MODE_AUTO);
    lv_obj_set_scroll_dir(grid_cocktails_cont, LV_DIR_VER);

    // Use Flex Layout (Responsive Row Wrap)
    lv_obj_set_flex_flow(grid_cocktails_cont, LV_FLEX_FLOW_ROW_WRAP);
    lv_obj_set_flex_align(grid_cocktails_cont, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);
    lv_obj_set_style_pad_column(grid_cocktails_cont, 20, 0); // Gap X
    lv_obj_set_style_pad_row(grid_cocktails_cont, 20, 0);    // Gap Y 

    // Register cleanup
    lv_obj_add_event_cb(screen, page_cocktails_delete_cb, LV_EVENT_DELETE, NULL);

    // Initial Sync/Mock logic
    if (!DataManager::getInstance().isRecipesSynced() || DataManager::getInstance().getRecipes().empty()) {
        printf("[Cocktails] Cache empty. Loading fallback mocks & Starting Sync Retry.\n");
        DataManager::getInstance().loadMocks(); // Proactive load
        
        // Trigger first sync immediately
        ESPNowManager::getInstance().requestRecipeSync();
        ESPNowManager::getInstance().requestPumpSync();
    }

    // Initial draw
    refresh_grid();
    last_processed_update = DataManager::getInstance().getLastUpdate();

    // Start Retry Timer (checks every 5s if we are still on mocks)
    if (sync_retry_timer) lv_timer_del(sync_retry_timer);
    sync_retry_timer = lv_timer_create(sync_retry_timer_cb, SYNC_RETRY_INTERVAL_MS, NULL);

    // Dynamic refresh timer
    cocktails_refresh_timer = lv_timer_create(refresh_timer_cb, 500, NULL);

    // Footer Container using MyFooter component
    create_custom_footer(screen, ICON_NAV_SETTINGS, on_nav_click);

    return screen;
}

