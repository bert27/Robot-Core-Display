#ifndef PAGE_CONFIG_H
#define PAGE_CONFIG_H

#include "lvgl.h"

// Initializes and returns the Configuration screen
lv_obj_t* page_config_create(lv_event_cb_t on_nav_back, lv_event_cb_t on_nav_next);

#endif
