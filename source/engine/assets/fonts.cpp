#include <string>
#include <format>
#include <unordered_map>
#include <filesystem>

#include "globals.hpp"

#include "engine/assets/assets.hpp"

extern "C" {
    #include <raylib.h>
}

namespace CE::Assets::Fonts {
    static std::unordered_map<std::string, Font> Fonts;
    static Font defaultFont = GetFontDefault();

    void Load(const std::string& name, const std::string& path) {
        namespace fs = std::filesystem;
        std::string filePath = CE::Global.data_path + path;

        if(!fs::exists(filePath)) {
            TraceLog(LOG_WARNING, "CE-Fonts: Missing font assets: %s", path.c_str());
            return;
        }

        Font font_file = LoadFont(filePath.c_str());

        if(!IsFontValid(font_file)) {
            TraceLog(LOG_WARNING, "CE-Fonts: Invalid font file: %s", path.c_str());
            UnloadFont(font_file);
            return;
        }

        if (Fonts.contains(name)) {
            TraceLog(LOG_WARNING, "CE-Fonts: Font already loaded over-writing: %s", name.c_str());
        }
        Fonts[name] = font_file; 
    }

    void LoadEx(const std::string& name,const int size, const std::string& path) {
        namespace fs = std::filesystem;
        std::string filePath = CE::Global.data_path + path;

        if(!fs::exists(filePath)) {
            TraceLog(LOG_WARNING, "CE-Fonts: Missing font assets: %s", path.c_str());
            return;
        }

        Font font_file = LoadFontEx(filePath.c_str(), size, nullptr, 0);

        if(!IsFontValid(font_file)) {
            TraceLog(LOG_WARNING, "CE-Fonts: Invalid font file: %s", path.c_str());
            UnloadFont(font_file);
            return;
        }

        if (Fonts.contains(name)) {
            TraceLog(LOG_WARNING, "CE-Fonts: Font already loaded over-writing: %s", name.c_str());
        }
        Fonts[name] = font_file; 
    }

    void DrawEx(const std::string& name, const std::string& text, int fontsize, int PosX, int PosY, const Color color) {
        Vector2 Position = { (float)PosX, (float)PosY };
        float spacing = fontsize / 10.0f;

        auto font = Fonts.find(name);
        if (font != Fonts.end()) {
            DrawTextEx(font->second, text.c_str(), Position, (float)fontsize, spacing, color);
        } else {
            TraceLog(LOG_WARNING, "CE-Fonts: Tried to draw missing font: %s", name.c_str());
            DrawTextEx(defaultFont, text.c_str(), Position, (float)fontsize, spacing, RED);
        }
    }

    void Draw(const std::string& text, int fontsize, int PosX, int PosY, const Color color) {
        Vector2 Position = { (float)PosX, (float)PosY };
        float spacing = fontsize / 10.0f;
        DrawTextEx(defaultFont, text.c_str(), Position, (float)fontsize, spacing, color);
    }

    void SetDefaultFont(const std::string& name) {
        auto font = Fonts.find(name);

        if(font != Fonts.end()) {
            defaultFont = font->second;
            TraceLog(LOG_INFO, "CE-Fonts: Default font has been changed to: %s", name.c_str());
            return;
        }
        defaultFont = GetFontDefault();
        TraceLog(LOG_WARNING, "CE-Fonts: Tried to set default font to an unloaded or missing font: %s", name.c_str());
        return;
    }

    void Unload(const std::string& name) {
        auto Lst = Fonts.find(name);

        if (Lst != Fonts.end()) {
            UnloadFont(Lst->second);
            Fonts.erase(Lst);
            TraceLog(LOG_INFO, "CE-Fonts: Unloaded font: '%s' ", name.c_str());
            return;
        }
        TraceLog(LOG_WARNING, "CE-Fonts: Tried to unload a non-existant font: %s", name.c_str());
    }

    void UnloadAll() {
        for (auto& [name, font] : Fonts) {
            UnloadFont(font);
        }
        Fonts.clear();
    }
}
