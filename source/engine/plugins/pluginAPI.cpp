#include <cstdint>
#include <cstdarg>
#include <vector>
#include <iostream>

extern "C" {
    #include <raylib.h>
}

#include "engine/assets/assets.hpp"
#include "globals.hpp"
#include "engine/plugins/plugins.hpp"

namespace CE::PluginAPI {
    // Texture shit
    void TexturesDraw(const char* path, const uint32_t posX, const uint32_t posY) {
        CE::Assets::Textures::Draw(
            std::string(path), 
            static_cast<int>(posX), 
            static_cast<int>(posY)
        );
    }
    void TexturesUnload(const char* name) {
        CE::Assets::Textures::Unload(std::string(name));
    }
    void TexturesUnloadAll() {
        CE::Assets::Textures::UnloadAll();

    }

    void Log(const uint32_t log_level, const char* format, ...) {
        if (!CE::Flags::show_plugin_logs)
            return;

        va_list args;
        va_start(args, format);

        va_list args_copy;
        va_copy(args_copy, args);
        int required = vsnprintf(nullptr, 0, format, args_copy);
        va_end(args_copy);

        if (required < 0) {
            va_end(args);
            return;
        }

        std::vector<char> buffer(required + 1);
        vsnprintf(buffer.data(), buffer.size(), format, args);
        va_end(args);

        std::string message(buffer.data());

        switch (log_level) {
            case 1:
                std::cout << "PLUGIN_INFO: " << message << "\n";
                break;
            case 2:
                std::cout << "PLUGIN_DEBUG: " << message << "\n";
                break;
            case 3:
                std::cout << "PLUGIN_WARN: " << message << "\n";
                break;
            case 4:
                std::cout << "PLUGIN_ERROR: " << message << "\n";
                break;
            default:
                std::cout << "PLUGIN_UNKNOWN: " << message << "\n";
                break;
        }
    }

    void changeWindowTitle(const char* winTitle) {
        Log(CE_LOG_INFO, "PLUGIN HAS CHANGED WINDOW TITLE TO: %s", winTitle);
        SetWindowTitle(winTitle);
    }

    // Keyboard stuff
    uint32_t CeIsKeyPressed(uint32_t key) {
        if (IsKeyPressed(key)) {
            return 1;
        } else {
            return 0;
        }
    }
    uint32_t CeIsKeyPressedRepeat(uint32_t key) {
        if (IsKeyPressedRepeat(key)) {
            return 1;
        } else {
            return 0;
        }
    }
    uint32_t CeIsKeyDown(uint32_t key) {
        if (IsKeyDown(key)) {
            return 1;
        } else {
            return 0;
        }
    }
    uint32_t CeIsKeyReleased(uint32_t key) {
        if (IsKeyReleased(key)) {
            return 1;
        } else {
            return 0;
        }
    }
    uint32_t CeIsKeyUp(uint32_t key) {
        if (IsKeyUp(key)) {
            return 1;
        } else {
            return 0;
        }
    }
    uint32_t CeGetKeyPressed(void) {
        return GetKeyPressed();
    }
    uint32_t CeGetCharPressed(void) {
        return GetCharPressed();
    }
}