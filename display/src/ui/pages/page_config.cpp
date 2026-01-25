#include "page_config.h"
#include "../components/MyButton.h"
#include "../components/MyTitle.h"
#include "../components/footer/MyFooter.h"
#include "../components/card/MyCard.h" 
#include "../assets/icons.h"
#include "../components/modal/MyRecipeModal.hpp"
#include "../../core/MemoryManager.hpp"
#include "../../core/ESPNowManager.hpp"
#include <vector>
#include <stdio.h>

#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>

// --- State ---
static std::vector<ConfigCocktail> recipes;
static bool sync_complete = false;
static unsigned long sync_start_time = 0;
static lv_timer_t * sync_timer = NULL;
static lv_timer_t * queue_timer = NULL;
static lv_obj_t * grid_config_cont = NULL;

static QueueHandle_t recipe_recv_queue = NULL;

// Pump mapping
static const char* PUMP_NAMES[] = {"Cocacola", "Orange Juice", "Vodka", "Grenadine"};

// Forward declaration
static void create_config_card(lv_obj_t * parent, ConfigCocktail * cocktail);
static void load_mock_recipes();

static void refresh_grid() {
    if (!grid_config_cont) return;
    lv_obj_clean(grid_config_cont);

    for(size_t i=0; i < recipes.size(); i++) {
         create_config_card(grid_config_cont, &recipes[i]);
         lv_obj_set_grid_cell(lv_obj_get_child(grid_config_cont, i), LV_GRID_ALIGN_STRETCH, i%4, 1, LV_GRID_ALIGN_STRETCH, i/4, 1);
    }
}

// Thread-Safe Callback (Runs in WiFi Task)
static void on_recipe_recv(const RecipeSyncData& data) {
    if (recipe_recv_queue) {
        xQueueSend(recipe_recv_queue, &data, 0); // Non-blocking send
    }
}

// UI Thread Timer to process queue
static void recipe_queue_timer_cb(lv_timer_t * t) {
    if (!recipe_recv_queue) return;
    
    RecipeSyncData data;
    bool needs_refresh = false;

    // Process all pending packets
    while (xQueueReceive(recipe_recv_queue, &data, 0) == pdTRUE) {
        printf("[UI] Processing Recipe Queue: %s\n", data.name);
        
        // Clear mock data on first real packet
        if (!sync_complete) {
            recipes.clear();
            sync_complete = true;
        }

        ConfigCocktail c;
        c.name = String(data.name);
        
        // Auto-assign icon/color
        if (c.name.indexOf("Coca") >= 0) { c.icon = ICON_COCKTAIL_COCA_COLA; c.color = 0xFF0000; }
        else if (c.name.indexOf("Orange") >= 0) { c.icon = ICON_COCKTAIL_GIN_TONIC; c.color = 0xFFA500; }
        else if (c.name.indexOf("Vodka") >= 0) { c.icon = ICON_COCKTAIL_VODKA; c.color = 0x00FFFF; }
        else if (c.name.indexOf("Sex") >= 0) { c.icon = ICON_COCKTAIL_SEX_ON_BEACH; c.color = 0xFF1493; }
        else if (c.name.indexOf("Tequila") >= 0) { c.icon = ICON_COCKTAIL_PORN_STAR; c.color = 0xFF4500; }
        else { c.icon = ICON_COCKTAIL_VODKA; c.color = 0x888888; } // Default

        // Parse ingredients
        for (int i=0; i<4; i++) {
             if (data.ingredientsMl[i] > 0) {
                 c.ingredients.push_back({PUMP_NAMES[i], i+1, data.ingredientsMl[i]});
             }
        }
        
        recipes.push_back(c);
        needs_refresh = true;
        
        // Reset timeout since we are receiving data
        sync_start_time = millis();
    }
    
    if (needs_refresh) {
        refresh_grid();
    }
}

static void sync_timeout_cb(lv_timer_t * t) {
    // If we haven't received anything in 2 seconds AND vector is empty
    if (recipes.empty()) {
        printf("[UI] Sync Timeout. Loading Mock Data.\n");
        load_mock_recipes();
        refresh_grid();
    }
    // Delete sync timer (one-shot logic)
    lv_timer_del(t);
    sync_timer = NULL;
}

static void init_recipes() {
    // Create Queue if not exists
    if (!recipe_recv_queue) {
        recipe_recv_queue = xQueueCreate(20, sizeof(RecipeSyncData));
    }
    
    // Create Queue Processing Timer
    if (!queue_timer) {
        queue_timer = lv_timer_create(recipe_queue_timer_cb, 100, NULL);
    }

    if (ESPNowManager::getInstance().isConnected()) {
        printf("[UI] Requesting Recipes from Server...\n");
        recipes.clear();
        sync_complete = false;
        ESPNowManager::getInstance().setRecipeCallback(on_recipe_recv);
        ESPNowManager::getInstance().requestRecipeSync();
        
        // Timeout check
        sync_start_time = millis();
        sync_timer = lv_timer_create(sync_timeout_cb, 2000, NULL);
    } else {
        printf("[UI] Server Offline. Using Mock.\n");
        load_mock_recipes();
    }
}

static void load_mock_recipes() {
    if (!recipes.empty()) return;

    recipes.push_back({"Cocacola", ICON_COCKTAIL_COCA_COLA, 0xFF0000, {
        {"Cocacola", 1, 200}
    }});
    
    recipes.push_back({"Orange Juice", ICON_COCKTAIL_GIN_TONIC, 0xFFA500, {
        {"Orange", 2, 200}
    }});

    recipes.push_back({"Vodka shot", ICON_COCKTAIL_VODKA, 0x00FFFF, {
        {"Vodka", 3, 50}
    }});

    recipes.push_back({"Vodka Coke", ICON_COCKTAIL_VODKA, 0x8B0000, {
        {"Vodka", 3, 50},
        {"Cocacola", 1, 150}
    }});

    recipes.push_back({"Screwdriver", ICON_COCKTAIL_VODKA, 0xFFD700, {
        {"Vodka", 3, 50},
        {"Orange", 2, 150}
    }});
    
    recipes.push_back({"Sex on Beach", ICON_COCKTAIL_SEX_ON_BEACH, 0xFF1493, {
        {"Vodka", 3, 40},
        {"Orange", 2, 100},
        {"Grenadine", 4, 10}
    }});
    
    recipes.push_back({"Tequila Sun", ICON_COCKTAIL_PORN_STAR, 0xFF4500, {
        {"Tequila", 3, 50}, 
        {"Orange", 2, 120},
        {"Grenadine", 4, 10}
    }});
    
    recipes.push_back({"Shirley T.", ICON_COCKTAIL_COCA_COLA, 0xFF69B4, {
        {"Orange", 2, 100},
        {"Grenadine", 4, 20},
        {"Cocacola", 1, 50}
    }});
}

static void on_edit_click(lv_event_t * e) {
    ConfigCocktail * cocktail = (ConfigCocktail *)lv_event_get_user_data(e);
    if (cocktail) {
        printf("Editing recipe: %s\n", cocktail->name.c_str());
        create_recipe_modal(lv_scr_act(), cocktail, NULL);
    }
}

static void create_config_card(lv_obj_t * parent, ConfigCocktail * cocktail) {
    lv_obj_t * card = lv_obj_create(parent);
    lv_obj_set_width(card, 170); 
    lv_obj_set_height(card, 220); // Smaller card
    lv_obj_set_style_bg_color(card, lv_color_hex(0x202020), 0); // Dark Gray
    lv_obj_set_style_bg_opa(card, LV_OPA_COVER, 0);
    lv_obj_set_style_border_color(card, lv_color_hex(cocktail->color), 0);
    lv_obj_set_style_border_width(card, 2, 0);
    lv_obj_set_style_radius(card, 15, 0);
    lv_obj_set_flex_flow(card, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(card, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_all(card, 10, 0);

    // Icon
    lv_obj_t * img = lv_image_create(card); // Use lv_image_create for v9 compat / scaling support
    lv_image_set_src(img, cocktail->icon);
    lv_image_set_scale(img, 128); // 50% scale (Small photo)
    lv_obj_set_style_img_recolor(img, lv_color_hex(cocktail->color), 0);
    lv_obj_set_style_img_recolor_opa(img, LV_OPA_30, 0); // Subtle tint

    // Title
    lv_obj_t * label = lv_label_create(card);
    lv_label_set_text(label, cocktail->name.c_str());
    lv_obj_set_style_text_font(label, &lv_font_montserrat_16, 0); // Smaller font for smaller card
    lv_obj_set_style_text_color(label, lv_color_white(), 0);
    lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_CENTER, 0);
    lv_label_set_long_mode(label, LV_LABEL_LONG_WRAP);
    lv_obj_set_width(label, LV_PCT(100));

    // Edit Button
    lv_obj_t * btn = lv_btn_create(card);
    lv_obj_set_width(btn, LV_PCT(100));
    lv_obj_set_height(btn, 35); // Smaller button
    lv_obj_set_style_bg_color(btn, lv_color_hex(0x333333), 0);
    lv_obj_set_style_shadow_width(btn, 0, 0);
    
    lv_obj_t * btn_lbl = lv_label_create(btn);
    lv_label_set_text(btn_lbl, LV_SYMBOL_EDIT " EDITAR");
    lv_obj_set_style_text_font(btn_lbl, &lv_font_montserrat_14, 0);
    lv_obj_center(btn_lbl);
    
    lv_obj_add_event_cb(btn, on_edit_click, LV_EVENT_CLICKED, cocktail);
}

lv_obj_t* page_config_create(lv_event_cb_t on_nav_back, lv_event_cb_t on_nav_next) {
    grid_config_cont = NULL; // Reset static
    
    lv_obj_t* screen = lv_obj_create(NULL);
    lv_obj_set_style_bg_color(screen, lv_color_hex(0x101010), LV_PART_MAIN); 
    lv_obj_set_style_bg_opa(screen, LV_OPA_COVER, LV_PART_MAIN);

    // Title
    create_custom_title(screen, "CONFIG RECIPES");

    // Connection Status Icon
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

    // Grid Container
    grid_config_cont = lv_obj_create(screen);
    lv_obj_set_size(grid_config_cont, 740, 300);
    lv_obj_align(grid_config_cont, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_bg_opa(grid_config_cont, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(grid_config_cont, 0, 0);
    lv_obj_set_scrollbar_mode(grid_config_cont, LV_SCROLLBAR_MODE_OFF); 
    
    static int32_t col_dsc[] = {170, 170, 170, 170, LV_GRID_TEMPLATE_LAST}; // 4 Columns now possible? 170*4=680 < 740
    static int32_t row_dsc[] = {220, 220, LV_GRID_TEMPLATE_LAST}; 
    lv_obj_set_grid_dsc_array(grid_config_cont, col_dsc, row_dsc);
    lv_obj_set_grid_align(grid_config_cont, LV_GRID_ALIGN_CENTER, LV_GRID_ALIGN_START); 
    lv_obj_set_style_pad_column(grid_config_cont, 15, 0); 
    lv_obj_set_style_pad_row(grid_config_cont, 15, 0); 

    // Initialize (triggers Sync or Mock)
    init_recipes();
    refresh_grid(); // Initial draw (empty or mock)

    // Footer container
    create_nav_footer(screen, on_nav_back, on_nav_next);

    return screen;
}
