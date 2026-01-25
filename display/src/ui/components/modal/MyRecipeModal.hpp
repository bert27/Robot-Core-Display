#ifndef MY_RECIPE_MODAL_HPP
#define MY_RECIPE_MODAL_HPP

#include <lvgl.h>
#include <vector>
#include <Arduino.h>

// --- Data Models (Local for UI Config) ---
struct ConfigIngredient {
    String name;
    int pumpId; // 1-4
    int amountMl;
};

struct ConfigCocktail {
    String name;
    const void* icon; // LVGL Image src
    uint32_t color;
    std::vector<ConfigIngredient> ingredients;
};

// --- Callback Type ---
typedef void (*RecipeSaveCallback)(ConfigCocktail* modifiedCocktail);

// --- State Management ---
static lv_obj_t * g_recipe_modal = NULL;
static ConfigCocktail * g_current_cocktail = NULL;
static RecipeSaveCallback g_save_cb = NULL;

// Helper to close modal
inline void recipe_modal_close_cb(lv_event_t * e) {
    if (g_recipe_modal) {
        lv_obj_del(g_recipe_modal);
        g_recipe_modal = NULL;
        g_current_cocktail = NULL;
    }
}

// Helper to update ml label
inline void ingredient_slider_cb(lv_event_t * e) {
    lv_obj_t * slider = (lv_obj_t *)lv_event_get_target(e);
    lv_obj_t * label = (lv_obj_t *)lv_event_get_user_data(e);
    
    int val = lv_slider_get_value(slider);
    lv_label_set_text_fmt(label, "%d ml", val);
    
    // Note: In a real implementation we would update the temp struct here
}

// Create the dynamic modal
inline void create_recipe_modal(lv_obj_t * parent, ConfigCocktail * cocktail, RecipeSaveCallback on_save) {
    g_current_cocktail = cocktail;
    g_save_cb = on_save;

    // 1. Overlay
    g_recipe_modal = lv_obj_create(parent ? parent : lv_scr_act());
    lv_obj_set_size(g_recipe_modal, LV_PCT(100), LV_PCT(100));
    lv_obj_set_style_bg_color(g_recipe_modal, lv_color_black(), 0);
    lv_obj_set_style_bg_opa(g_recipe_modal, LV_OPA_80, 0);
    lv_obj_set_style_border_width(g_recipe_modal, 0, 0);
    lv_obj_center(g_recipe_modal);
    lv_obj_add_flag(g_recipe_modal, LV_OBJ_FLAG_CLICKABLE); // Block clicks

    // 2. Modal Box
    lv_obj_t * mbox = lv_obj_create(g_recipe_modal);
    lv_obj_set_size(mbox, 500, 350);
    lv_obj_center(mbox);
    lv_obj_set_style_bg_color(mbox, lv_color_hex(0x202020), 0);
    lv_obj_set_style_border_color(mbox, lv_color_hex(cocktail->color), 0); // Border matches drink color
    lv_obj_set_style_border_width(mbox, 2, 0);
    lv_obj_set_flex_flow(mbox, LV_FLEX_FLOW_COLUMN);
    
    // 3. Header
    lv_obj_t * title = lv_label_create(mbox);
    lv_label_set_text_fmt(title, "EDITAR: %s", cocktail->name.c_str());
    lv_obj_set_style_text_font(title, &lv_font_montserrat_20, 0);
    lv_obj_set_style_text_color(title, lv_color_hex(cocktail->color), 0);
    lv_obj_set_style_align(title, LV_ALIGN_TOP_MID, 0);

    // 4. Ingredients List (Scrollable)
    lv_obj_t * list = lv_obj_create(mbox);
    lv_obj_set_size(list, LV_PCT(100), LV_PCT(65));
    lv_obj_set_style_bg_opa(list, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(list, 0, 0);
    lv_obj_set_flex_flow(list, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_pad_row(list, 15, 0);

    for (auto &ing : cocktail->ingredients) {
        lv_obj_t * item = lv_obj_create(list);
        lv_obj_set_width(item, LV_PCT(100));
        lv_obj_set_height(item, LV_SIZE_CONTENT);
        lv_obj_set_style_bg_opa(item, LV_OPA_TRANSP, 0);
        lv_obj_set_style_border_width(item, 0, 0);
        lv_obj_set_style_pad_left(item, 10, 0);
        lv_obj_set_style_pad_right(item, 10, 0);

        // Label: Name + Amount
        lv_obj_t * header = lv_obj_create(item);
        lv_obj_set_size(header, LV_PCT(100), LV_SIZE_CONTENT);
        lv_obj_set_style_bg_opa(header, LV_OPA_TRANSP, 0);
        lv_obj_set_style_border_width(header, 0, 0);
        lv_obj_set_style_pad_all(header, 0, 0);
        lv_obj_set_flex_flow(header, LV_FLEX_FLOW_ROW);
        lv_obj_set_flex_align(header, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

        lv_obj_t * name_lbl = lv_label_create(header);
        lv_label_set_text(name_lbl, ing.name.c_str());
        lv_obj_set_style_text_color(name_lbl, lv_color_hex(0xAAAAAA), 0);

        lv_obj_t * val_lbl = lv_label_create(header);
        lv_label_set_text_fmt(val_lbl, "%d ml", ing.amountMl);
        lv_obj_set_style_text_color(val_lbl, lv_color_white(), 0);

        // Slider
        lv_obj_t * slider = lv_slider_create(item);
        lv_obj_set_width(slider, LV_PCT(95)); // Reduce width slightly to avoid edge clipping
        lv_obj_set_align(slider, LV_ALIGN_CENTER); // Center it
        lv_slider_set_range(slider, 0, 300); // 0 to 300ml
        lv_slider_set_value(slider, ing.amountMl, LV_ANIM_OFF);
        
        // Style slider
        lv_obj_set_style_bg_color(slider, lv_color_hex(0x505050), LV_PART_MAIN);
        lv_obj_set_style_bg_color(slider, lv_color_hex(cocktail->color), LV_PART_INDICATOR);
        lv_obj_set_style_bg_color(slider, lv_color_white(), LV_PART_KNOB);

        lv_obj_add_event_cb(slider, ingredient_slider_cb, LV_EVENT_VALUE_CHANGED, val_lbl);
    }

    // 5. Buttons
    lv_obj_t * btn_cont = lv_obj_create(mbox);
    lv_obj_set_size(btn_cont, LV_PCT(100), LV_SIZE_CONTENT);
    lv_obj_set_style_bg_opa(btn_cont, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(btn_cont, 0, 0);
    lv_obj_set_flex_flow(btn_cont, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(btn_cont, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_top(btn_cont, 10, 0);

    // Close Button (Full width for now, user didn't ask for generic save logic yet)
    lv_obj_t * btn_close = lv_btn_create(btn_cont);
    lv_obj_set_width(btn_close, LV_PCT(100));
    lv_obj_set_style_bg_color(btn_close, lv_color_hex(0x444444), 0);
    lv_obj_t * lbl_close = lv_label_create(btn_close);
    lv_label_set_text(lbl_close, "CERRAR");
    lv_obj_center(lbl_close);
    lv_obj_add_event_cb(btn_close, recipe_modal_close_cb, LV_EVENT_CLICKED, NULL);

}

#endif // MY_RECIPE_MODAL_HPP
