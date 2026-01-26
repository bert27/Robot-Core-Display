#include "page_config.h"
#include "../components/MyButton.h"
#include "../components/MyTitle.h"
#include "../components/footer/MyFooter.h"
#include "../components/card/MyCard.h" 
#include "../assets/icons.h"
#include "../components/modal/MyRecipeModal.hpp"

#include "../../core/ESPNowManager.hpp"
#include <vector>
#include <stdio.h>
#include "../../core/Config.hpp"
#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>

// --- State ---
static lv_timer_t * status_timer = NULL;
static lv_timer_t * refresh_timer = NULL;
static lv_timer_t * sync_retry_timer = NULL;
static lv_obj_t * grid_config_cont = NULL;
static unsigned long last_processed_update = 0;

static void create_config_card(lv_obj_t * parent, const ICocktail * cocktail);
static void load_mock_recipes();

static void refresh_grid() {
    if (!grid_config_cont) return;
    lv_obj_clean(grid_config_cont);

    const auto& recipes = DataManager::getInstance().getRecipes();
    for(size_t i=0; i < recipes.size(); i++) {
         create_config_card(grid_config_cont, &recipes[i]);
         lv_obj_set_grid_cell(lv_obj_get_child(grid_config_cont, i), LV_GRID_ALIGN_STRETCH, i%4, 1, LV_GRID_ALIGN_STRETCH, i/4, 1);
    }
}

// UI Thread Timer to refresh if DataManager updated
static void refresh_timer_cb(lv_timer_t * t) {
    if (DataManager::getInstance().getLastUpdate() > last_processed_update) {
        printf("[UI] DataManager updated. Refreshing grid...\n");
        last_processed_update = DataManager::getInstance().getLastUpdate();
        refresh_grid();
    }
}

static void status_timer_cb(lv_timer_t * t) {
    lv_obj_t * icon = (lv_obj_t *)lv_timer_get_user_data(t);
    if (!icon) return;

    if (!DataManager::getInstance().isUsingMocks()) {
        lv_label_set_text(icon, LV_SYMBOL_WIFI " Online");
        lv_obj_set_style_text_color(icon, lv_color_hex(0x00FF00), 0);
    } else {
        lv_label_set_text(icon, LV_SYMBOL_WARNING " Offline (Mock)");
        lv_obj_set_style_text_color(icon, lv_color_hex(0xFF8800), 0);
    }
}



static void sync_retry_timer_cb(lv_timer_t * t) {
    if (DataManager::getInstance().isUsingMocks()) {
        printf("[Config] Still using mocks. Retrying Sync Request...\n");
        ESPNowManager::getInstance().requestRecipeSync();
        ESPNowManager::getInstance().requestPumpSync();
    } else {
        printf("[Config] Real data detected. Stopping Sync Retry Timer.\n");
        lv_timer_del(t);
        sync_retry_timer = NULL;
    }
}

static void page_config_delete_cb(lv_event_t * e) {
    printf("[UI] Deleting Page Config. Cleaning up timers...\n");
    if (refresh_timer) { lv_timer_del(refresh_timer); refresh_timer = NULL; }
    if (status_timer) { lv_timer_del(status_timer); status_timer = NULL; }
    if (sync_retry_timer) { lv_timer_del(sync_retry_timer); sync_retry_timer = NULL; }
}

static void init_recipes() {
    // 1. Initial background request if not synced
    if (!DataManager::getInstance().isRecipesSynced()) {
        printf("[UI] Global Cache empty. Requesting from Server...\n");
        ESPNowManager::getInstance().requestRecipeSync();
        ESPNowManager::getInstance().requestPumpSync();
    }
    
    // 2. Initial Draw from Cache
    refresh_grid();
    last_processed_update = DataManager::getInstance().getLastUpdate();

    // 3. Create Refresh Timer to catch background updates
    if (!refresh_timer) {
        refresh_timer = lv_timer_create(refresh_timer_cb, 500, NULL);
    }

    // 4. Start Retry Timer if using Mocks
    if (DataManager::getInstance().isUsingMocks() || !DataManager::getInstance().isRecipesSynced()) {
        if (sync_retry_timer) lv_timer_del(sync_retry_timer);
        sync_retry_timer = lv_timer_create(sync_retry_timer_cb, SYNC_RETRY_INTERVAL_MS, NULL);
    }
}

static void load_mock_recipes() {
    auto mockCocktails = getDefaultMockCocktails();
    for (const auto& mock : mockCocktails) {
        ICocktail c = mock; // Directly assign since types now match!
        DataManager::getInstance().addRecipeFromConfig(c);
    }
}

static void on_recipe_save(ICocktail * c) {
    if (!c) return;

    printf("[Config] Saving Recipe: %s\n", c->name.c_str());

    RecipeSyncData data;
    memset(&data, 0, sizeof(data));
    
    // Name
    strncpy(data.name, c->name.c_str(), sizeof(data.name) - 1);
    
    // Ingredients Mapping
    // Client has Name/Qty. Server Protocol expects array [0..3] corresponding to Pump 1..4.
    // We strictly map by Pump Name standard? Or just trust the order?
    // DataManager mapMetadata uses names to assign icons.
    // Here we need to map "Cocacola" -> Pump 1 (Index 0)
    // "Orange Juice" -> Pump 2 (Index 1)
    // "Vodka" -> Pump 3 (Index 2)
    // "Grenadine" -> Pump 4 (Index 3)
    
    // We assume standard names for now as defined in Config/DataManager.
    // If user renamed liquid in pumps, we might have a mismatch if we don't know the pump config here.
    // But DataManager has getPumpSettings()... but that lacks liquid names.
    // HOWEVER, the `ICocktail` came from DataManager which was synced from Server.
    // The Server uses specific names. 
    // We will do a generic map based on the ingredient name string match.
    
    for (const auto& ing : c->ingredients) {
        if (ing.name.indexOf("Coca") >= 0) data.ingredientsMl[0] = ing.quantity;
        else if (ing.name.indexOf("Orange") >= 0) data.ingredientsMl[1] = ing.quantity;
        else if (ing.name.indexOf("Vodka") >= 0) data.ingredientsMl[2] = ing.quantity;
        else if (ing.name.indexOf("Grenadine") >= 0) data.ingredientsMl[3] = ing.quantity;
        // Fallback or other ingredients? 
        // If we strictly follow the Defaults:
        // Pump 1: Cocacola
        // Pump 2: Orange Juice
        // Pump 3: Vodka
        // Pump 4: Grenadine
    }

    // Index & Total are less relevant for Update, but let's keep them zero or valid if we knew them.
    // Server RemoteActions uses Name to find the cocktail, so Index is not used for lookup there.
    data.index = 0; 
    data.total = 0;

    ESPNowManager::getInstance().sendRecipeUpdate(data);
    
    // OPTIMISTIC UPDATE: Update local cache immediately so UI reflects changes
    // even if Server Sync packet hasn't arrived yet.
    DataManager::getInstance().updateRecipe(*c);
}

static void on_edit_click(lv_event_t * e) {
    ICocktail * cocktail = (ICocktail *)lv_event_get_user_data(e);
    if (cocktail) {
        printf("Editing recipe: %s\n", cocktail->name.c_str());
        create_recipe_modal(lv_scr_act(), cocktail, on_recipe_save);
    }
}

static void create_config_card(lv_obj_t * parent, const ICocktail * cocktail) {
    lv_obj_t * card = lv_obj_create(parent);
    lv_obj_clear_flag(card, LV_OBJ_FLAG_SCROLLABLE); 
    lv_obj_add_flag(card, LV_OBJ_FLAG_CLICKABLE); // Fix: The whole card is now clickable
    lv_obj_set_width(card, 170); 
    lv_obj_set_height(card, 220);
    lv_obj_set_style_bg_color(card, lv_color_hex(0x202020), 0);
    lv_obj_set_style_bg_opa(card, LV_OPA_COVER, 0);
    lv_obj_set_style_border_color(card, lv_color_hex(cocktail->color), 0);
    lv_obj_set_style_border_width(card, 2, 0);
    lv_obj_set_style_radius(card, 15, 0);
    lv_obj_set_flex_flow(card, LV_FLEX_FLOW_COLUMN);
    // Align content to top (START) to avoid spreading too wide if space is excess
    lv_obj_set_flex_align(card, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_all(card, 10, 0);
    lv_obj_set_style_pad_row(card, 5, 0); // Small gap between sections

    // Add Event to the main card
    lv_obj_add_event_cb(card, on_edit_click, LV_EVENT_CLICKED, (void*)cocktail);

    // 1. Image Container (Fixed Height to prevent overflow)
    lv_obj_t * img_cont = lv_obj_create(card);
    lv_obj_clear_flag(img_cont, LV_OBJ_FLAG_SCROLLABLE); 
    lv_obj_clear_flag(img_cont, LV_OBJ_FLAG_CLICKABLE); // Transparent to clicks
    lv_obj_set_size(img_cont, LV_PCT(100), 100); 
    lv_obj_set_style_bg_opa(img_cont, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(img_cont, 0, 0);
    lv_obj_set_style_pad_all(img_cont, 0, 0);
    
    // Icon
    lv_obj_t * img = lv_image_create(img_cont); 
    lv_image_set_src(img, cocktail->icon);
    lv_image_set_scale(img, 128); 
    lv_obj_set_style_img_recolor(img, lv_color_hex(cocktail->color), 0);
    lv_obj_set_style_img_recolor_opa(img, LV_OPA_30, 0); 
    lv_obj_center(img); 

    // 2. Title
    lv_obj_t * label = lv_label_create(card);
    lv_obj_clear_flag(label, LV_OBJ_FLAG_CLICKABLE); // Transparent to clicks
    lv_label_set_text(label, cocktail->name.c_str());
    lv_obj_set_style_text_font(label, &lv_font_montserrat_16, 0); 
    lv_obj_set_style_text_color(label, lv_color_white(), 0);
    lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_CENTER, 0);
    lv_label_set_long_mode(label, LV_LABEL_LONG_WRAP);
    lv_obj_set_width(label, LV_PCT(100));

    // 3. Edit Button (Visual only now, click passes to card)
    lv_obj_t * filler = lv_obj_create(card);
    lv_obj_clear_flag(filler, LV_OBJ_FLAG_SCROLLABLE); 
    lv_obj_clear_flag(filler, LV_OBJ_FLAG_CLICKABLE); 
    lv_obj_set_style_bg_opa(filler, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(filler, 0, 0);
    lv_obj_set_flex_grow(filler, 1); 

    lv_obj_t * btn = lv_btn_create(card);
    lv_obj_clear_flag(btn, LV_OBJ_FLAG_CLICKABLE); // Make button itself non-clickable so click passes to card
    lv_obj_set_width(btn, LV_PCT(100));
    lv_obj_set_height(btn, 35); 
    lv_obj_set_style_bg_color(btn, lv_color_hex(0x007ACC), 0); 
    lv_obj_set_style_shadow_width(btn, 0, 0);
    
    lv_obj_t * btn_lbl = lv_label_create(btn);
    lv_label_set_text(btn_lbl, LV_SYMBOL_EDIT " EDITAR");
    lv_obj_set_style_text_font(btn_lbl, &lv_font_montserrat_14, 0);
    lv_obj_center(btn_lbl);
}

lv_obj_t* page_config_create(lv_event_cb_t on_nav_back, lv_event_cb_t on_nav_next) {
    grid_config_cont = NULL; // Reset static
    
    lv_obj_t* screen = lv_obj_create(NULL);
    lv_obj_set_style_bg_color(screen, lv_color_hex(0x101010), LV_PART_MAIN); 
    lv_obj_set_style_bg_opa(screen, LV_OPA_COVER, LV_PART_MAIN);

    // Register Cleanup Callback
    lv_obj_add_event_cb(screen, page_config_delete_cb, LV_EVENT_DELETE, NULL);

    // Title
    create_custom_title(screen, "CONFIG RECIPES");

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
    grid_config_cont = lv_obj_create(screen);
    lv_obj_set_size(grid_config_cont, 740, 300);
    lv_obj_align(grid_config_cont, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_bg_opa(grid_config_cont, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(grid_config_cont, 0, 0);
    lv_obj_set_scrollbar_mode(grid_config_cont, LV_SCROLLBAR_MODE_OFF); 
    lv_obj_set_scroll_dir(grid_config_cont, LV_DIR_VER); // Fix: Prevent horizontal scroll
    lv_obj_set_style_pad_all(grid_config_cont, 0, 0); // Ensure no padding around grid 
    
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
