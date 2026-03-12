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
    static Font ErrorFont = GetFontDefault();

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

        Font font_file = LoadFontEx(filePath.c_str(), size, NULL, NULL);

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

    void Draw(const std::string& name, const std::string& text, int fontsize, int PosX, int PosY, const Color color) {
        Vector2 Position = { (float)PosX, (float)PosY };
        float spacing = fontsize / 10.0f;

        auto font = Fonts.find(name);
        if (font != Fonts.end()) {
            DrawTextEx(font->second, text.c_str(), Position, (float)fontsize, spacing, color);
        } else {
            TraceLog(LOG_WARNING, "CE-Fonts: Tried to draw missing font: %s", name.c_str());
            DrawTextEx(ErrorFont, text.c_str(), Position, (float)fontsize, spacing, RED);
        }
    }

    void Unload(const std::string& name) {
        auto Lst = Fonts.find(name);

        if (Lst != Fonts.end()) {
            UnloadFont(Lst->second);
            Fonts.erase(Lst);
            TraceLog(LOG_INFO, "CE-Audio: Unloaded font: '%s' ", name.c_str());
            return;
        }
        TraceLog(LOG_WARNING, "CE-Audio: Tried to unload a non-existant font: %s", name.c_str());
    }

    void UnloadAll() {
        for (auto& [name, font] : Fonts) {
            UnloadFont(font);
        }
        Fonts.clear();
    }
}