#ifndef MY_MODAL_HPP
#define MY_MODAL_HPP

#include <lvgl.h>
#include <cstdio>
#include <cstring>

// Callback to close the modal
inline void modal_close_cb(lv_event_t * e) {
    lv_obj_t * modal_overlay = (lv_obj_t *)lv_event_get_user_data(e);
    if (modal_overlay) {
        lv_obj_del(modal_overlay);
    }
}

/**
 * @brief Creates a modal dialog overlay.
 * 
 * @param parent Parent object (usually NULL or the active screen)
 * @param title Title of the modal
 * @param message Message body
 * @param on_confirm Callback for confirm button (LV_EVENT_CLICKED)
 * @param on_cancel Callback for cancel button (LV_EVENT_CLICKED)
 * @param user_data Pointer to pass to callbacks (e.g. drink name string)
 */
inline void create_custom_modal(lv_obj_t * parent, const char * title, const char * message, 
                         lv_event_cb_t on_confirm, lv_event_cb_t on_cancel, void * user_data) {
    
    // 1. Full Screen Overlay (Dim Background)
    lv_obj_t * overlay = lv_obj_create(parent ? parent : lv_scr_act());
    lv_obj_set_size(overlay, LV_PCT(100), LV_PCT(100));
    lv_obj_set_style_bg_color(overlay, lv_color_black(), 0);
    lv_obj_set_style_bg_opa(overlay, LV_OPA_70, 0); // Semi-transparent
    lv_obj_set_style_border_width(overlay, 0, 0);
    lv_obj_center(overlay);
    
    // Block clicks on background
    lv_obj_add_flag(overlay, LV_OBJ_FLAG_CLICKABLE);

    // 2. Modal Box
    lv_obj_t * mbox = lv_obj_create(overlay);
    lv_obj_set_size(mbox, 400, 250);
    lv_obj_center(mbox);
    lv_obj_set_style_bg_color(mbox, lv_color_hex(0x303030), 0);
    lv_obj_set_style_border_color(mbox, lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_style_border_width(mbox, 2, 0);
    lv_obj_set_flex_flow(mbox, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(mbox, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

    // 3. Title
    lv_obj_t * lbl_title = lv_label_create(mbox);
    lv_label_set_text(lbl_title, title);
    lv_obj_set_style_text_font(lbl_title, &lv_font_montserrat_20, 0);
    lv_obj_set_style_text_color(lbl_title, lv_color_hex(0xFFA500), 0); // Orange

    // 4. Message Area (Icon + Text)
    lv_obj_t * msg_cont = lv_obj_create(mbox);
    lv_obj_set_size(msg_cont, LV_PCT(100), LV_SIZE_CONTENT);
    lv_obj_set_style_bg_opa(msg_cont, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(msg_cont, 0, 0);
    lv_obj_set_style_pad_all(msg_cont, 0, 0);
    lv_obj_set_flex_flow(msg_cont, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(msg_cont, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_column(msg_cont, 10, 0); // Gap between icon and text

    // Icon
    lv_obj_t * icon_lbl = lv_label_create(msg_cont);
    lv_label_set_text(icon_lbl, "?"); // Use literal "?" since LV_SYMBOL_QUESTION is missing
    lv_obj_set_style_text_font(icon_lbl, &lv_font_montserrat_20, 0);
    lv_obj_set_style_text_color(icon_lbl, lv_color_hex(0x00A0FF), 0); // Blue Question Mark

    // Text Message
    lv_obj_t * lbl_msg = lv_label_create(msg_cont);
    lv_label_set_text(lbl_msg, message);
    lv_label_set_long_mode(lbl_msg, LV_LABEL_LONG_WRAP);
    lv_obj_set_width(lbl_msg, 280); // Fixed width to allow wrapping within modal
    lv_obj_set_style_text_align(lbl_msg, LV_TEXT_ALIGN_LEFT, 0);
    lv_obj_set_style_text_font(lbl_msg, &lv_font_montserrat_16, 0);
    lv_obj_set_style_text_color(lbl_msg, lv_color_white(), 0);

    // 5. Buttons Container
    lv_obj_t * btn_cont = lv_obj_create(mbox);
    lv_obj_set_size(btn_cont, LV_PCT(100), LV_SIZE_CONTENT);
    lv_obj_set_style_bg_opa(btn_cont, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(btn_cont, 0, 0);
    lv_obj_set_flex_flow(btn_cont, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(btn_cont, LV_FLEX_ALIGN_SPACE_EVENLY, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

    // Cancel Button
    lv_obj_t * btn_cancel = lv_btn_create(btn_cont);
    lv_obj_set_size(btn_cancel, 120, 50);
    lv_obj_set_style_bg_color(btn_cancel, lv_color_hex(0xFF0000), 0); // Red
    lv_obj_t * lbl_cancel = lv_label_create(btn_cancel);
    lv_label_set_text(lbl_cancel, "CANCEL");
    lv_obj_center(lbl_cancel);
    
    // Add event to close modal + user callback
    if (on_cancel) {
        lv_obj_add_event_cb(btn_cancel, on_cancel, LV_EVENT_CLICKED, user_data);
    }
    lv_obj_add_event_cb(btn_cancel, modal_close_cb, LV_EVENT_CLICKED, overlay);

    // Confirm Button
    lv_obj_t * btn_confirm = lv_btn_create(btn_cont);
    lv_obj_set_size(btn_confirm, 120, 50);
    lv_obj_set_style_bg_color(btn_confirm, lv_color_hex(0x00AA00), 0); // Green
    lv_obj_t * lbl_confirm = lv_label_create(btn_confirm);
    lv_label_set_text(lbl_confirm, "CONFIRM");
    lv_obj_center(lbl_confirm);
    
    // Add event to close modal + user callback
    if (on_confirm) {
        lv_obj_add_event_cb(btn_confirm, on_confirm, LV_EVENT_CLICKED, user_data);
    }
    lv_obj_add_event_cb(btn_confirm, modal_close_cb, LV_EVENT_CLICKED, overlay);
}

#endif // MY_MODAL_HPP
