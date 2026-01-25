#include "MyFooter.h"
#include "../MyButton.h"
#include "../../assets/icons.h"

lv_obj_t* create_custom_footer(lv_obj_t* parent, const void* icon_src, lv_event_cb_t event_cb) {
    // Footer Container
    lv_obj_t * footer = lv_obj_create(parent);
    
    // 1. TOTAL STYLE CLEANUP (Expert)
    // Remove any inherited properties from the theme (hidden paddings, borders, default backgrounds).
    lv_obj_remove_style_all(footer);
    
    // 2. Define dimensions and position
    // Force explicit width of 800px to avoid ambiguities with PERCENTAGES and parent paddings
    lv_obj_set_size(footer, 800, 80); 
    // Explicitly align the container to the right as well
    lv_obj_align(footer, LV_ALIGN_BOTTOM_RIGHT, 0, 0);
    
    // 3. (DEBUG Removed) Final transparent background
    lv_obj_set_style_bg_opa(footer, LV_OPA_TRANSP, 0);
    

    // 4. Absolute Layout within the Footer (Safer than Flex)
    // Remove Flex to manually position the child
    // lv_obj_set_flex_flow(footer, LV_FLEX_FLOW_ROW);
    
    // Ensure 0 gaps
    lv_obj_set_style_pad_all(footer, 0, 0);
    
    // Create Clickable Image (Standard LVGL Pattern for Icons)
    // Using lv_image instead of lv_imagebutton because lv_imagebutton ignores lv_image_set_scale
    lv_obj_t *img_btn = lv_image_create(footer);
    lv_image_set_src(img_btn, icon_src);
    lv_obj_add_flag(img_btn, LV_OBJ_FLAG_CLICKABLE); // Make it behave like a button
    
    // Scale and Align
    // Image is 200px. We want ~50px.
    // Scale factor: 256 * (Target / Original) = 256 * (50 / 200) = 64
    lv_image_set_scale(img_btn, 64); 
    
    // Use ABSOLUTE positioning at the right edge
    lv_obj_align(img_btn, LV_ALIGN_RIGHT_MID, 0, 0); 
    
    // Apply negative margin to push it further right if it has transparent clamping
    lv_obj_set_style_margin_right(img_btn, -30, 0); 

    // Remove background opacity (debug) - Set to transparent for production
    lv_obj_set_style_bg_opa(img_btn, LV_OPA_TRANSP, 0); 

    // Add event callback
    if (event_cb) {
        lv_obj_add_event_cb(img_btn, event_cb, LV_EVENT_CLICKED, NULL);
    }
    
    // Status Label Removed
    
    return footer;
}

void footer_set_status_text(lv_obj_t* footer, const char* text) {
   // Disabled
}

lv_obj_t* create_nav_footer(lv_obj_t* parent, lv_event_cb_t on_back, lv_event_cb_t on_next) {
    lv_obj_t * footer = lv_obj_create(parent);
    lv_obj_remove_style_all(footer);
    lv_obj_set_size(footer, 800, 70);
    lv_obj_align(footer, LV_ALIGN_BOTTOM_MID, 0, 0);
    lv_obj_set_style_bg_opa(footer, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(footer, 0, 0);
    lv_obj_set_flex_flow(footer, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(footer, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_column(footer, 40, 0);

    if (on_back) {
        create_custom_button(footer, ICON_NAV_PREV, "Back", 180, 50, lv_color_hex(0x800000), on_back, NULL);
    }
    
    if (on_next) {
        create_custom_button(footer, ICON_NAV_NEXT, "Next", 180, 50, lv_color_hex(0x008000), on_next, NULL);
    }

    return footer;
}

lv_obj_t* create_simple_nav_footer(lv_obj_t* parent, lv_event_cb_t on_back) {
    return create_nav_footer(parent, on_back, NULL);
}
