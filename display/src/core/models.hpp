#ifndef MODELS_HPP
#define MODELS_HPP

#include <Arduino.h>
#include <vector>

/**
 * @brief Unified interface for a drink ingredient
 */
struct IIngredient {
    String name;
    int pump;
    int quantity;
};

/**
 * @brief Unified interface for a cocktail recipe
 */
struct ICocktail {
    String name;
    const void * icon; // Reference to LVGL image source
    uint32_t color;    // Representative color (HEX)
    std::vector<IIngredient> ingredients;
};

/**
 * @brief Unified interface for pump settings
 */
struct IPumpSettings {
    int pwm[4] = {255, 255, 255, 255};
    int timeMs[4] = {1600, 1600, 1600, 1600};
    bool synced = false;
};

#endif // MODELS_HPP
