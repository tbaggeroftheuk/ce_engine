#include <string>
#include <format>
#include <unordered_map>

#include "globals.hpp"

extern "C" {
    #include <raylib.h>
}

static std::unordered_map<std::string, Texture2D> textures;

static Texture2D ErrorTexture = {0, 0, 0, 0, 0};

namespace CE::Assets::Textures {
    
    void Init() { // Must be ran before anything else 
        Image img = GenImageChecked(64, 64, 8, 8, MAGENTA, BLACK);
        ErrorTexture = LoadTextureFromImage(img);
        UnloadImage(img);
    }

    void Load(const std::string& name, const std::string& filename) {
        
        std::string filepath = std::format("{}/{}", CE::Global.data_path, filename);
        Texture2D tex = {0, 0, 0, 0, 0};

        if (!FileExists(filepath.c_str())) {
            TraceLog(LOG_WARNING, "CE-Textures: Missing asset '%s' ", filepath.c_str());
            tex = ErrorTexture;
        } else {
            tex = LoadTexture(filepath.c_str());
            if (tex.id == 0) {
                TraceLog(LOG_WARNING, "CE-Textures: Failed to load asset '%s'", filepath.c_str());
                tex = ErrorTexture;
            } else {
                TraceLog(LOG_INFO, "CE-Textures: Loaded texture from '%s'", filepath.c_str());
            }
        }

        // Always insert into the map, even if it's the error texture
        textures.emplace(name, tex);
    }


    Texture2D& Get(const std::string& name) {
        auto lst = textures.find(name);
        if (lst != textures.end()) {
            return lst->second;
        }

        TraceLog(LOG_ERROR, "CE-Textures: Couldn't find texture '%s'", name.c_str());
        TraceLog(LOG_ERROR, "CE-Textures: Maybe you forgot to load?");
        return ErrorTexture;
    }


    void Draw(const std::string& path, int posX, int posY, Color tint = WHITE) {
        auto it = textures.find(path);
        if (it == textures.end()) {
            Load(path, path);
        }
        DrawTexture(Get(path), posX, posY, tint);
    }


    void Unload(const std::string& name) {
        auto lst = textures.find(name);

        if (lst != textures.end()){
            UnloadTexture(lst->second);
            textures.erase(lst);
            TraceLog(LOG_INFO, "CE-Textures: Unloaded texture '%s' ", name.c_str());
            return;
        }

        TraceLog(LOG_ERROR, "CE-Textures: Tried to unload a non-existant texture!");
        return;
    }

    void UnloadAll(){
        for (auto& [name, texture] : textures) {
           UnloadTexture(texture);
        }
        textures.clear();
    }


    void Shutdown() {
        for (auto& [name, texture] : textures) {
           UnloadTexture(texture);
        }
        textures.clear();
        UnloadTexture(ErrorTexture);
    }
}