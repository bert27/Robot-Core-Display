#ifndef DISPLAY_MANAGER_HPP
#define DISPLAY_MANAGER_HPP

#include <Arduino_GFX_Library.h>
#include <lvgl.h>
#include "Config.hpp"
#include "TouchDriver.hpp"
#include "../ui/ui.h"
#include "ESPNowManager.hpp"
#include <WiFi.h>

class DisplayManager {
public:
    static DisplayManager& getInstance() {
        static DisplayManager instance;
        return instance;
    }

    bool begin() {
        delay(5000); 
        printf("\n\n>>> HARDWARE INIT (Sync v3.2.1) <<<\n");
        printf("Configuring RGB Panel...\n");
        
        if (!psramFound()) {
            printf("ERROR: PSRAM not found!\n");
        } else {
            printf("OK: PSRAM active (%d bytes)\n", (int)ESP.getPsramSize());
        }

        bus = new Arduino_ESP32RGBPanel(
            RGB_DE, RGB_VSYNC, RGB_HSYNC, RGB_PCLK,
            RGB_R0, RGB_R1, RGB_R2, RGB_R3, RGB_R4,
            RGB_G0, RGB_G1, RGB_G2, RGB_G3, RGB_G4, RGB_G5,
            RGB_B0, RGB_B1, RGB_B2, RGB_B3, RGB_B4,
            0 /* hsync_polarity */, H_FRONT_PORCH, H_PULSE_WIDTH, H_BACK_PORCH,
            0 /* vsync_polarity */, V_FRONT_PORCH, V_PULSE_WIDTH, V_BACK_PORCH,
            PCLK_INV, PCLK_SPEED, false /* useBigEndian */,
            0 /* de_idle_high */, 0 /* pclk_idle_high */,
            BOUNCE_BUFFER_SIZE // Restore correct buffer size (was incorrectly set to true/1)
        );

        gfx = new Arduino_RGB_Display(SCREEN_WIDTH, SCREEN_HEIGHT, bus, 0, true);

        Serial.println("Calling gfx->begin()...");
        if (!gfx->begin()) {
            Serial.println("FATAL ERROR: gfx->begin failed!");
            return false;
        }

        Serial.println("GFX OK. Backlight ON.");

        // Turn on backlight and quick visual test
        pinMode(TFT_BL, OUTPUT);
        digitalWrite(TFT_BL, HIGH);
        
        gfx->fillScreen(BLUE); // Visual test: blue screen means driver works
        delay(200);
        gfx->fillScreen(BLACK);

        Serial.println("Initializing LVGL...");
        lv_init();
        lv_tick_set_cb(get_millis);

        // Memory Management for Buffer (INTERNAL RAM to avoid flickering)
        size_t bufSize = SCREEN_WIDTH * LVGL_DRAW_LINES;
        
        disp_draw_buf = (lv_color_t *)heap_caps_malloc(bufSize * sizeof(lv_color_t), MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
        disp_draw_buf2 = (lv_color_t *)heap_caps_malloc(bufSize * sizeof(lv_color_t), MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);

        if (!disp_draw_buf || !disp_draw_buf2) {
            log_e("ERROR: Double buffer malloc failed in SRAM! Retrying in PSRAM...");
            disp_draw_buf = (lv_color_t *)heap_caps_malloc(bufSize * sizeof(lv_color_t), MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
            disp_draw_buf2 = (lv_color_t *)heap_caps_malloc(bufSize * sizeof(lv_color_t), MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
        }
        
        if (!disp_draw_buf) {
            log_e("FATAL ERROR: No memory for LVGL buffers");
            return false;
        }
        
        log_i("LVGL Double Buffer (%d lines) in %s OK.", LVGL_DRAW_LINES, 
              (esp_ptr_internal(disp_draw_buf) ? "SRAM" : "PSRAM"));

        disp = lv_display_create(SCREEN_WIDTH, SCREEN_HEIGHT);
        lv_display_set_flush_cb(disp, disp_flush_cb);
        
        // Back to PARTIAL to use fast SRAM
        lv_display_set_buffers(disp, disp_draw_buf, disp_draw_buf2, bufSize * sizeof(lv_color_t), LV_DISPLAY_RENDER_MODE_PARTIAL);

        // Input Device Configuration (Touch)
        touch_init();
        
        lv_indev_t *indev = lv_indev_create();
        lv_indev_set_type(indev, LV_INDEV_TYPE_POINTER);
        lv_indev_set_read_cb(indev, touchpad_read_cb);

        printf("Initializing UI...\n");
        ui_init();

        printf("Initializing ESP-NOW Manager...\n");
        ESPNowManager::getInstance().begin();

        printf("System ready\n");
        return true;
    }

    void update() {
        lv_task_handler();
        delay(5);
    }

private:
    DisplayManager() : bus(nullptr), gfx(nullptr), disp(nullptr), disp_draw_buf(nullptr), disp_draw_buf2(nullptr) {}

    // Hardware objects
    Arduino_ESP32RGBPanel *bus;
    Arduino_RGB_Display   *gfx;
    
    // LVGL drawing buffer
    lv_display_t *disp;
    lv_color_t   *disp_draw_buf;
    lv_color_t   *disp_draw_buf2;

    // Internal static methods for LVGL
    static uint32_t get_millis() {
        return esp_timer_get_time() / 1000;
    }

    static void disp_flush_cb(lv_display_t *disp, const lv_area_t *area, uint8_t *px_map) {
#ifndef DIRECT_MODE
        DisplayManager& self = getInstance();
        uint32_t w = lv_area_get_width(area);
        uint32_t h = lv_area_get_height(area);
        self.gfx->draw16bitRGBBitmap(area->x1, area->y1, (uint16_t *)px_map, w, h);
#endif
        lv_disp_flush_ready(disp);
    }

    static void touchpad_read_cb(lv_indev_t *indev, lv_indev_data_t *data) {
        if (touch_has_signal()) {
            if (touch_touched()) {
                data->state = LV_INDEV_STATE_PRESSED;
                data->point.x = touch_last_x;
                data->point.y = touch_last_y;
            } else {
                data->state = LV_INDEV_STATE_RELEASED;
            }
        } else {
            data->state = LV_INDEV_STATE_RELEASED;
        }
    }

    static void log_cb(lv_log_level_t level, const char *buf) {
        Serial.println(buf);
        Serial.flush();
    }
};

#endif // DISPLAY_MANAGER_HPP
