#include <string>
#include <format>
#include <unordered_map>

#include "globals.hpp"

extern "C" {
    #include <raylib.h>
}

std::unordered_map<std::string, Texture2D> textures;

static Texture2D ErrorTexture = {0, 0, 0, 0, 0};

static std::vector<std::string> logged_missing;

namespace CE::Assets::Textures {
    
    void Init() { // Must be ran before anything else 
        Image img = GenImageChecked(64, 64, 8, 8, MAGENTA, BLACK);
        ErrorTexture = LoadTextureFromImage(img);
    }

    void Load(const std::string& name, const std::string& filename) {
       
        auto lst = textures.find(name);

        if (lst != textures.end()) {
            TraceLog(LOG_INFO, "CE-Textures: '%s' is already loaded! ", name.c_str());
            return;
        }

        std::string filepath;
        filepath = std::format("{}/{}", CE::Global.data_path, filename); // Build path for the texture from the base path made by bootstrap

        Texture2D tex = {0, 0, 0, 0, 0};

        if(!FileExists(filepath.c_str())) { // Check if image exists
            TraceLog(LOG_WARNING, "CE-Textures: Missing asset '%s' ", filepath.c_str());
            tex = ErrorTexture; // If not use error texture
        } else {
            tex = LoadTexture(filepath.c_str());

            if (tex.id == 0) { // if failed to load we use error texture
                TraceLog(LOG_WARNING, "CE-Textures: Failed to load asset '%s'", filepath.c_str());
                tex = ErrorTexture;
            } else {
                TraceLog(LOG_INFO, "CE-Textures: Loaded texture from '%s'", filepath.c_str());
            }
        }
        textures.emplace(name, tex);

    }

    Texture2D& Get(const std::string& name) {   
        auto lst = textures.find(name);

        if (lst != textures.end()) {
            TraceLog(LOG_INFO, "CE-Textures: Got texture '%s' ", name.c_str());
            return lst->second;
        }

        TraceLog(LOG_ERROR, "CE-Textures: Couldn't find texture '%s", name.c_str());
        TraceLog(LOG_ERROR, "CE-Textures: Maybe you forgot to load?");
        return ErrorTexture;
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