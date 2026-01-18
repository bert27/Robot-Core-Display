#ifndef DISPLAY_MANAGER_H
#define DISPLAY_MANAGER_H

#include <Arduino_GFX_Library.h>
#include <lvgl.h>
#include <eez-framework.h>
#include "Config.h"
#include "../drivers/touch.h"
#include "../ui/ui.h"

class DisplayManager {
public:
    static DisplayManager& getInstance() {
        static DisplayManager instance;
        return instance;
    }

    bool begin();
    void update();

private:
    DisplayManager(); // Singleton
    
    // Objetos de hardware
    Arduino_ESP32RGBPanel *bus;
    Arduino_RGB_Display   *gfx;
    
    // Buffer de dibujo de LVGL
    lv_display_t *disp;
    lv_color_t   *disp_draw_buf;
    lv_color_t   *disp_draw_buf2;

    // Métodos estáticos internos para LVGL
    static uint32_t get_millis();
    static void disp_flush_cb(lv_display_t *disp, const lv_area_t *area, uint8_t *px_map);
    static void touchpad_read_cb(lv_indev_t *indev, lv_indev_data_t *data);
    static void log_cb(lv_log_level_t level, const char *buf);
};

#endif // DISPLAY_MANAGER_H
