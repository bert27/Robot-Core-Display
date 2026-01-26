#ifndef DATA_MANAGER_HPP
#define DATA_MANAGER_HPP

#include <Arduino.h>
#include <vector>
#include "remote_protocol.hpp"
#include "models.hpp"
#include "Config.hpp"
#include "../ui/assets/icons.h"

class DataManager {
public:
    static DataManager& getInstance() {
        static DataManager instance;
        return instance;
    }

    // --- Recipes ---
    bool isRecipesSynced() const { return recipesSynced; }
    bool isUsingMocks() const { return usingMocks; }
    const std::vector<ICocktail>& getRecipes() const { return recipes; }
    
    void clearRecipes() { 
        recipes.clear(); 
        recipesSynced = false;
        usingMocks = false;
    }

    void addRecipe(const RecipeSyncData& data) {
        // Clear if new sync session starts (Index 0) OR if we were using mocks
        if (data.index == 0 || usingMocks) {
            if (usingMocks) printf("[DataManager] Real data received. Clearing Mocks.\n");
            else printf("[DataManager] New Sync Session (Index 0). Clearing old data.\n");
            
            recipes.clear();
            recipesSynced = false;
            usingMocks = false;
        }

        if (!recipesSynced) {
            recipesSynced = true; // Mark as syncing started
        }

        ICocktail c;
        c.name = String(data.name);
        mapMetadata(c);

        static const char* PUMP_NAMES[] = {"Cocacola", "Orange Juice", "Vodka", "Grenadine"};
        for (int i=0; i<4; i++) {
             if (data.ingredientsMl[i] > 0) {
                 c.ingredients.push_back({PUMP_NAMES[i], i+1, (int)data.ingredientsMl[i]});
             }
        }
        
        recipes.push_back(c);
        lastUpdateTime = millis();
    }

    void addRecipeFromConfig(const ICocktail& cocktail) {
        if (usingMocks) {
            recipes.clear(); 
            usingMocks = false;
        }
        if (!recipesSynced) {
            recipes.clear();
            recipesSynced = true;
        }
        ICocktail c = cocktail;
        mapMetadata(c); 
        recipes.push_back(c);
        lastUpdateTime = millis();
    }

    void updateRecipe(const ICocktail& updatedCocktail) {
        for (auto& c : recipes) {
            if (c.name == updatedCocktail.name) {
                c.ingredients = updatedCocktail.ingredients;
                lastUpdateTime = millis();
                printf("[DataManager] Optimistic Update for: %s\n", c.name.c_str());
                return;
            }
        }
        printf("[DataManager] Warning: Recipe not found for update: %s\n", updatedCocktail.name.c_str());
    }

    void loadMocks() {
        if (recipesSynced && !recipes.empty() && !usingMocks) return; // Don't overwrite REAL live data
        
        printf("[DataManager] Loading Mocks.\n");
        auto mocks = getDefaultMockCocktails();
        recipes.clear();
        for (const auto& m : mocks) {
            ICocktail c = m;
            mapMetadata(c);
            recipes.push_back(c);
        }
        recipesSynced = true; 
        usingMocks = true;    // Mark as Mocks
        lastUpdateTime = millis();
    }


    // --- Pumps ---
    const IPumpSettings& getPumpSettings() const { return pumps; }
    
    void updatePumps(const PumpSyncData& data) {
        for(int i=0; i<4; i++) {
            pumps.pwm[i] = data.pwm[i];
            pumps.timeMs[i] = (int)(data.calibration[i] * 1000.0f);
        }
        pumps.synced = true;
        lastUpdateTime = millis();
    }

    unsigned long getLastUpdate() const { return lastUpdateTime; }

private:
    DataManager() {}

    void mapMetadata(ICocktail& c) {
        // Centralized logic for icon/color assignment
        if (c.name.indexOf("Coca") >= 0) { c.icon = ICON_COCKTAIL_COCA_COLA; c.color = 0xFF0000; }
        else if (c.name.indexOf("Orange") >= 0) { c.icon = ICON_COCKTAIL_GIN_TONIC; c.color = 0xFFA500; }
        else if (c.name.indexOf("Vodka") >= 0) { c.icon = ICON_COCKTAIL_VODKA; c.color = 0x00FFFF; }
        else if (c.name.indexOf("Sex") >= 0) { c.icon = ICON_COCKTAIL_SEX_ON_BEACH; c.color = 0xFF1493; }
        else if (c.name.indexOf("Tequila") >= 0) { c.icon = ICON_COCKTAIL_PORN_STAR; c.color = 0xFF4500; }
        else if (c.name.indexOf("Gin") >= 0) { c.icon = ICON_COCKTAIL_GIN_TONIC; c.color = 0xADD8E6; }
        else { c.icon = ICON_COCKTAIL_VODKA; c.color = 0x888888; }
    }
    
    std::vector<ICocktail> recipes;
    bool recipesSynced = false;
    bool usingMocks = false;
    
    IPumpSettings pumps = getDefaultPumpSettings();
    unsigned long lastUpdateTime = 0;
};

#endif // DATA_MANAGER_HPP
