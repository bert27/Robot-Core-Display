#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <Arduino.h>

/**
 * HARDWARE CONFIGURATION - SUNTON ESP32-8048S043 (4.3" 800x480)
 */

// --- RGB Panel Pins ---
#define RGB_DE    40
#define RGB_VSYNC 41
#define RGB_HSYNC 39
#define RGB_PCLK  42

#define RGB_R0 45
#define RGB_R1 48
#define RGB_R2 47
#define RGB_R3 21
#define RGB_R4 14

#define RGB_G0 5
#define RGB_G1 6
#define RGB_G2 7
#define RGB_G3 15
#define RGB_G4 16
#define RGB_G5 4

#define RGB_B0 8
#define RGB_B1 3
#define RGB_B2 46
#define RGB_B3 9
#define RGB_B4 1

// --- Backlight ---
#define TFT_BL 2

// --- Timing Configuration (Optimized 60FPS Attempt) ---
#define H_FRONT_PORCH 8
#define H_PULSE_WIDTH 4
#define H_BACK_PORCH  20  
#define V_FRONT_PORCH 8
#define V_PULSE_WIDTH 4
#define V_BACK_PORCH  8   
#define PCLK_SPEED    16000000L // 16MHz: Smoothness attempt
#define PCLK_INV      1         
#define BOUNCE_BUFFER_SIZE (SCREEN_WIDTH * 20) // Keep buffer size conservative

// --- LVGL Configuration ---
#define SCREEN_WIDTH  800
#define SCREEN_HEIGHT 480
#define LVGL_DRAW_LINES 100 // Reduced to 20 to free SRAM and avoid bus contention
#define DOUBLE_BUFFER_ENABLED 1
// Disabled to avoid reboot loop

// --- Standard Colors (RGB565) ---
#ifndef BLACK
#define BLACK   0x0000
#endif
#ifndef BLUE
#define BLUE    0x001F
#endif
#ifndef RED
#define RED     0xF800
#endif
#ifndef GREEN
#define GREEN   0x07E0
#endif
#ifndef CYAN
#define CYAN    0x07FF
#endif
#ifndef MAGENTA
#define MAGENTA 0xF81F
#endif
#ifndef YELLOW
#define YELLOW  0xFFE0
#endif
#ifndef WHITE
#define WHITE   0xFFFF
#endif

// --- Touch Panel (GT911) ---
#define TOUCH_GT911
#define TOUCH_GT911_SCL 20
#define TOUCH_GT911_SDA 19
#define TOUCH_GT911_INT 18
#define TOUCH_GT911_RST 38
#define TOUCH_GT911_ROTATION ROTATION_NORMAL
// --- Touch Mapping Standard (Factory 800x480) ---
#define TOUCH_MAP_X1 800
#define TOUCH_MAP_X2 0
#define TOUCH_MAP_Y1 480
#define TOUCH_MAP_Y2 0

// --- Custom Calibration (Uncomment if your specific screen has a compressed range) ---
#define USE_CUSTOM_CALIBRATION
#ifdef USE_CUSTOM_CALIBRATION
  #undef TOUCH_MAP_X2
  #undef TOUCH_MAP_Y2
  #define TOUCH_MAP_X2 330 // Adjusted margin (Raw ~338 -> 800)
  #define TOUCH_MAP_Y2 220 // Adjusted margin (Raw ~224 -> 480)
#endif


// --- Application Defaults (Mocks) ---
#define SYNC_RETRY_INTERVAL_MS 5000

#include <vector>
#include "models.hpp"

inline std::vector<ICocktail> getDefaultMockCocktails() {
    return {
        {"Cocacola", nullptr, 0xFF0000, {{"Cocacola", 1, 200}}},
        {"Orange Juice", nullptr, 0xFFA500, {{"Orange", 2, 200}}},
        {"Vodka shot", nullptr, 0x00FFFF, {{"Vodka", 3, 50}}},
        {"Vodka Coke", nullptr, 0x8B0000, {{"Vodka", 3, 50}, {"Cocacola", 1, 150}}},
        {"Screwdriver", nullptr, 0xFFD700, {{"Vodka", 3, 50}, {"Orange", 2, 150}}},
        {"Sex on Beach", nullptr, 0xFF1493, {{"Vodka", 3, 40}, {"Orange", 2, 100}, {"Grenadine", 4, 10}}},
        {"Tequila Sun", nullptr, 0xFF4500, {{"Tequila", 3, 50}, {"Orange", 2, 120}, {"Grenadine", 4, 10}}},
        {"Shirley T.", nullptr, 0xFF69B4, {{"Orange", 2, 100}, {"Grenadine", 4, 20}, {"Cocacola", 1, 50}}}
    };
}

inline IPumpSettings getDefaultPumpSettings() {
    IPumpSettings s;
    for(int i=0; i<4; i++) {
        s.pwm[i] = 255;
        s.timeMs[i] = 1600;
    }
    s.synced = false;
    return s;
}

#endif // CONFIG_HPP

