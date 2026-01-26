#ifndef MY_RECIPE_MODAL_HPP
#define MY_RECIPE_MODAL_HPP

#include <lvgl.h>
#include <vector>
#include <Arduino.h>
#include "../slider/MySlider.h" 
#include "../../../core/models.hpp"

// --- Callback Type ---
typedef void (*RecipeSaveCallback)(ICocktail* modifiedCocktail);

// --- State Management ---
static lv_obj_t * g_recipe_modal = NULL;
static ICocktail g_edit_copy; // Safe local copy
static RecipeSaveCallback g_save_cb = NULL;

// Helper to close modal
inline void recipe_modal_close_cb(lv_event_t * e) {
    if (g_recipe_modal) {
        lv_obj_del(g_recipe_modal);
        g_recipe_modal = NULL;
    }
}

// --- Context for Slider Event ---
struct IngredientContext {
    IIngredient* ingredient;
    lv_obj_t* label;
};

// Helper to update ml label and data
inline void ingredient_slider_cb(lv_event_t * e) {
    lv_obj_t * slider = (lv_obj_t *)lv_event_get_target(e);
    IngredientContext * ctx = (IngredientContext *)lv_event_get_user_data(e);
    
    if (!ctx) return;

    int val = lv_slider_get_value(slider);
    
    // 1. Update Label (Visual)
    if (lv_event_get_code(e) == LV_EVENT_VALUE_CHANGED) {
        lv_label_set_text_fmt(ctx->label, "%d ml", val);
        // Also update struct immediately so visual sync is maintained if redrawn
        if (ctx->ingredient) ctx->ingredient->quantity = val;
    }

    // 2. Trigger Save Callback (Network)
    if (lv_event_get_code(e) == LV_EVENT_RELEASED) {
        if (ctx->ingredient) ctx->ingredient->quantity = val; // Ensure final value is set
        
        if (g_save_cb) {
            printf("[Modal] Slider Released. Triggering Save for: %s\n", g_edit_copy.name.c_str());
            g_save_cb(&g_edit_copy);
        }
    }
}

// Helper to clean up context
inline void ingredient_context_cleanup(lv_event_t * e) {
    IngredientContext * ctx = (IngredientContext *)lv_event_get_user_data(e);
    if (ctx) delete ctx;
}

// Create the dynamic modal
inline void create_recipe_modal(lv_obj_t * parent, ICocktail * cocktail, RecipeSaveCallback on_save) {
    if (!cocktail) return;
    
    // Create a safe copy of the data. 
    // This prevents crashes if DataManager updates/clears the original vector 
    // while we are editing (pointer invalidation).
    g_edit_copy = *cocktail; 
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
    lv_obj_set_style_border_color(mbox, lv_color_hex(g_edit_copy.color), 0); // Border matches drink color
    lv_obj_set_style_border_width(mbox, 2, 0);
    lv_obj_set_flex_flow(mbox, LV_FLEX_FLOW_COLUMN);
    
    // 3. Header
    lv_obj_t * title = lv_label_create(mbox);
    lv_label_set_text_fmt(title, "EDITAR: %s", g_edit_copy.name.c_str());
    lv_obj_set_style_text_font(title, &lv_font_montserrat_20, 0);
    lv_obj_set_style_text_color(title, lv_color_hex(g_edit_copy.color), 0);
    lv_obj_set_style_align(title, LV_ALIGN_TOP_MID, 0);

    // 4. Ingredients List (Scrollable)
    lv_obj_t * list = lv_obj_create(mbox);
    lv_obj_set_size(list, LV_PCT(100), LV_PCT(65));
    lv_obj_set_style_bg_opa(list, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(list, 0, 0);
    lv_obj_set_flex_flow(list, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_pad_row(list, 15, 0);

    // Context list to be cleaned up? No, attached to objects.
    
    // We iterate over our COPY
    for (size_t i = 0; i < g_edit_copy.ingredients.size(); i++) {
        
        auto &ing = g_edit_copy.ingredients[i];

        lv_obj_t * item = lv_obj_create(list);
        lv_obj_set_width(item, LV_PCT(100));
        lv_obj_set_height(item, LV_SIZE_CONTENT);
        lv_obj_set_style_bg_opa(item, LV_OPA_TRANSP, 0);
        lv_obj_set_style_border_width(item, 0, 0);
        lv_obj_clear_flag(item, LV_OBJ_FLAG_SCROLLABLE); 
        lv_obj_clear_flag(item, LV_OBJ_FLAG_CLICKABLE); 
        
        lv_obj_set_flex_flow(item, LV_FLEX_FLOW_COLUMN);
        lv_obj_set_style_pad_all(item, 10, 0);
        lv_obj_set_style_pad_row(item, 15, 0); 

        // 1. Slider 
        lv_obj_t * slider = create_custom_slider(item, 0, 300, LV_PCT(100), NULL);
        lv_slider_set_value(slider, ing.quantity, LV_ANIM_OFF);
        lv_obj_set_style_bg_color(slider, lv_color_hex(g_edit_copy.color), LV_PART_INDICATOR);

        // 2. Label Header
        lv_obj_t * header = lv_obj_create(item);
        lv_obj_set_size(header, LV_PCT(100), LV_SIZE_CONTENT);
        lv_obj_set_style_bg_opa(header, LV_OPA_TRANSP, 0);
        lv_obj_set_style_border_width(header, 0, 0);
        lv_obj_clear_flag(header, LV_OBJ_FLAG_SCROLLABLE);
        lv_obj_clear_flag(header, LV_OBJ_FLAG_CLICKABLE); 
        lv_obj_set_flex_flow(header, LV_FLEX_FLOW_ROW);
        lv_obj_set_flex_align(header, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
        
        lv_obj_t * name_lbl = lv_label_create(header);
        lv_label_set_text(name_lbl, ing.name.c_str());
        lv_obj_set_style_text_color(name_lbl, lv_color_hex(0xAAAAAA), 0);
        lv_obj_set_style_text_font(name_lbl, &lv_font_montserrat_16, 0); 

        lv_obj_t * val_lbl = lv_label_create(header);
        lv_label_set_text_fmt(val_lbl, "%d ml", ing.quantity);
        lv_obj_set_style_text_color(val_lbl, lv_color_white(), 0);
        lv_obj_set_style_text_font(val_lbl, &lv_font_montserrat_16, 0);

        // 3. Callback Context
        IngredientContext* ctx = new IngredientContext();
        ctx->ingredient = &ing;
        ctx->label = val_lbl;

        lv_obj_add_event_cb(slider, ingredient_slider_cb, LV_EVENT_VALUE_CHANGED, ctx);
        lv_obj_add_event_cb(slider, ingredient_slider_cb, LV_EVENT_RELEASED, ctx);
        lv_obj_add_event_cb(slider, ingredient_context_cleanup, LV_EVENT_DELETE, ctx);
    }

    // 5. Buttons
    lv_obj_t * btn_cont = lv_obj_create(mbox);
    lv_obj_set_size(btn_cont, LV_PCT(100), LV_SIZE_CONTENT);
    lv_obj_set_style_bg_opa(btn_cont, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(btn_cont, 0, 0);
    lv_obj_set_flex_flow(btn_cont, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(btn_cont, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_top(btn_cont, 10, 0);

    // Close Button
    lv_obj_t * btn_close = lv_btn_create(btn_cont);
    lv_obj_set_width(btn_close, LV_PCT(100));
    lv_obj_set_style_bg_color(btn_close, lv_color_hex(0x444444), 0);
    lv_obj_t * lbl_close = lv_label_create(btn_close);
    lv_label_set_text(lbl_close, "CERRAR");
    lv_obj_center(lbl_close);
    lv_obj_add_event_cb(btn_close, recipe_modal_close_cb, LV_EVENT_CLICKED, NULL);
}


#endif // MY_RECIPE_MODAL_HPP
