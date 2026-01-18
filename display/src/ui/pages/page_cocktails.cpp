#include "page_cocktails.h"
#include "../components/card/MyCard.h"
#include "../components/footer/MyFooter.h"
#include "../components/MyButton.h"
#include "../components/MyTitle.h"
#include "../assets/icons.h"

static lv_event_cb_t nav_callback = NULL;

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
    static int32_t row_dsc[] = {135, 135, LV_GRID_TEMPLATE_LAST}; 
    lv_obj_set_grid_dsc_array(grid_cont, col_dsc, row_dsc);
    lv_obj_set_grid_align(grid_cont, LV_GRID_ALIGN_CENTER, LV_GRID_ALIGN_START); 
    lv_obj_set_style_pad_column(grid_cont, 20, 0); 
    lv_obj_set_style_pad_row(grid_cont, 20, 0); 

    // Buttons (Row 1) - Now MyCard
    create_custom_card(grid_cont, ICON_COCKTAIL_SEX_ON_BEACH, "Sex on the Beach", 220, 135, lv_color_hex(0x800080), NULL, &lv_font_montserrat_20);
    create_custom_card(grid_cont, ICON_COCKTAIL_PORN_STAR, "Porn Star Martini", 220, 135, lv_color_hex(0x006400), NULL, &lv_font_montserrat_20);
    create_custom_card(grid_cont, ICON_COCKTAIL_COCA_COLA, "CocaCola", 220, 135, lv_color_hex(0x800000), NULL, &lv_font_montserrat_20);

    // Buttons (Row 2) - Now MyCard
    create_custom_card(grid_cont, ICON_COCKTAIL_VODKA, "Vodka", 220, 135, lv_color_hex(0xFF8C00), NULL, &lv_font_montserrat_20);
    create_custom_card(grid_cont, ICON_COCKTAIL_RON, "Ron", 220, 135, lv_color_hex(0x8B4513), NULL, &lv_font_montserrat_20);
    create_custom_card(grid_cont, ICON_COCKTAIL_GIN_TONIC, "Gin Tonic", 220, 135, lv_color_hex(0xFF69B4), NULL, &lv_font_montserrat_20);

    for(int i=0; i<6; i++) {
         lv_obj_set_grid_cell(lv_obj_get_child(grid_cont, i), LV_GRID_ALIGN_STRETCH, i%3, 1, LV_GRID_ALIGN_STRETCH, i/3, 1);
    }

    // Footer Container using MyFooter component
    create_custom_footer(screen, ICON_NAV_SETTINGS, on_nav_click);

    return screen;
}
