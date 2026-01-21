#ifndef CONFIG_H
#define CONFIG_H

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
#define H_BACK_PORCH  20  // Reduced from 43 to 20 to gain active time

#define V_FRONT_PORCH 8
#define V_PULSE_WIDTH 4
#define V_BACK_PORCH  8   // Reduced from 12 to 8

#define PCLK_SPEED    16000000L // 16MHz: Smoothness attempt (was 14MHz)
#define PCLK_INV      1         
#define BOUNCE_BUFFER_SIZE (SCREEN_WIDTH * 20) // Keep buffer size conservative

// --- LVGL Configuration ---
#define SCREEN_WIDTH  800
#define SCREEN_HEIGHT 480
#define LVGL_DRAW_LINES 40 // Use 40 lines to fit in internal RAM
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
#define TOUCH_MAP_X1 800
#define TOUCH_MAP_X2 0
#define TOUCH_MAP_Y1 480
#define TOUCH_MAP_Y2 0

#endif // CONFIG_H
