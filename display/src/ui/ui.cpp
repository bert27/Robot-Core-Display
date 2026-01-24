#include "ui.h"
#include "pages/page_cocktails.h"
#include "pages/page_config.h"
#include "pages/page_pumps.h"

static lv_obj_t * screen_cocktails = NULL;
static lv_obj_t * screen_config = NULL;
static lv_obj_t * screen_pumps = NULL;

// Forward declarations
static void show_screen_config(lv_event_t * e);
static void show_screen_cocktails(lv_event_t * e);
static void show_screen_pumps(lv_event_t * e);

// Navigation: Go to Config
static void show_screen_config(lv_event_t * e) {
    if (screen_config == NULL) {
        screen_config = page_config_create(show_screen_cocktails, show_screen_pumps);
    }
    lv_screen_load(screen_config);
}

// Navigation: Back to Cocktails
static void show_screen_cocktails(lv_event_t * e) {
    if (screen_cocktails == NULL) {
        screen_cocktails = page_cocktails_create(show_screen_config);
    }
    lv_screen_load(screen_cocktails);
}

// Navigation: Go to Pumps
static void show_screen_pumps(lv_event_t * e) {
    if (screen_pumps == NULL) {
        screen_pumps = page_pumps_create(show_screen_config);
    }
    lv_screen_load(screen_pumps);
}

void ui_init(void) {
    // Create initial screen
    screen_cocktails = page_cocktails_create(show_screen_config);
    lv_screen_load(screen_cocktails);
}

void ui_tick(void) {
    // Nothing for now
}
