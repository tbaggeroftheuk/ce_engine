#include <cstdint>
#include <vector>

extern "C" {
    #include <raylib.h>
}

#include "engine/assets/assets.hpp"

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