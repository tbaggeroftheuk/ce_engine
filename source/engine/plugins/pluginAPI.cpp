#include <cstdint>
#include <vector>
#include <iostream>

extern "C" {
    #include <raylib.h>
}

#include "engine/assets/assets.hpp"
#include "globals.hpp"

struct CE_Funcs {
    uint32_t Version;
    
    // Texture functions
    void (*Textures_Draw)(const char* path, uint32_t posX, uint32_t posY);
    void (*Textures_Unload)(const char* name);
    void (*Textures_UnloadAll)(void);
    
    // Keyboard functions
    uint32_t (*IsKeyPressed)(uint32_t key);
    uint32_t (*IsKeyPressedRepeat)(uint32_t key);
    uint32_t (*IsKeyDown)(uint32_t key);
    uint32_t (*IsKeyReleased)(uint32_t key);
    uint32_t (*IsKeyUp)(uint32_t key);
    uint32_t (*GetKeyPressed)(void);
    uint32_t (*GetCharPressed)(void);
};

namespace CE::PluginAPI {
    // Texture shit
    void Textures_Draw(const char* path, const uint32_t posX, const uint32_t posY) {
        CE::Assets::Textures::Draw(std::string(path), posX, posY);
    }
    void Textures_Unload(const char* name) {
        CE::Assets::Textures::Unload(std::string(name));
    }
    void Textures_UnloadAll() {
        CE::Assets::Textures::UnloadAll();

    }

    void CE_Log(const char* message) {
        
    }

    void Log(const uint32_t log_level, const char* message) { // This is to let plugins log
        if (CE::Flags::show_plugin_logs) { // To avoid clutter 
            switch (log_level) {
                case 1:
                    std::cout << "INFO:" << message;
                    break;
                case 2:
                    std::cout << "DEBUG: " << message;
                    break;
                case 3:
                    std::cout << "WARN: " << message;
                    break;
                case 4:
                    std::cout << "ERROR: " << message;
                    break;
                case 5:
                    std::cout << "FATAL: " << message;
                    break;
                default:
                    break;
            }
        }
    }
    // Keyboard stuff
    uint32_t CE_IsKeyPressed(uint32_t key) {
        if (IsKeyPressed(key)) {
            return 1;
        } else {
            return 0;
        }
    }
    uint32_t CE_IsKeyPressedRepeat(uint32_t key) {
        if (IsKeyPressedRepeat(key)) {
            return 1;
        } else {
            return 0;
        }
    }
    uint32_t CE_IsKeyDown(uint32_t key) {
        if (IsKeyDown(key)) {
            return 1;
        } else {
            return 0;
        }
    }
    uint32_t CE_IsKeyReleased(uint32_t key) {
        if (IsKeyReleased(key)) {
            return 1;
        } else {
            return 0;
        }
    }
    uint32_t CE_IsKeyUp(uint32_t key) {
        if (IsKeyUp(key)) {
            return 1;
        } else {
            return 0;
        }
    }
    uint32_t CE_GetKeyPressed(void) {
        return GetKeyPressed();
    }
    uint32_t CE_GetCharPressed(void) {
        return GetCharPressed();
    }
}