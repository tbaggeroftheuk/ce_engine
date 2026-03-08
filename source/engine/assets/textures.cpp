#include <string>
#include <format>
#include <unordered_map>
#include <filesystem>

#include "globals.hpp"

#include "engine/assets/assets.hpp"

extern "C" {
    #include <raylib.h>
}

namespace CE::Assets::Textures {
    static std::unordered_map<std::string, Texture2D> Textures;
    static Texture2D ErrorTexture = {0, 0, 0, 0, 0};
    
    void Init() { // Must be ran before anything else 
        Image Img = GenImageChecked(64, 64, 8, 8, MAGENTA, BLACK);
        ErrorTexture = LoadTextureFromImage(Img);
        UnloadImage(Img);
    }

    void Load(const std::string& name, const std::string& filename) {
        
        std::string Filepath = std::format("{}/{}", CE::Global.data_path, filename);
        Texture2D Tex = {0, 0, 0, 0, 0};

        if (!FileExists(Filepath.c_str())) {
            TraceLog(LOG_WARNING, "CE-Textures: Missing asset '%s' ", Filepath.c_str());
            Tex = ErrorTexture;
        } else {
            Tex = LoadTexture(Filepath.c_str());
            if (Tex.id == 0) {
                TraceLog(LOG_WARNING, "CE-Textures: Failed to load asset '%s'", Filepath.c_str());
                Tex = ErrorTexture;
            } else {
                TraceLog(LOG_INFO, "CE-Textures: Loaded texture from '%s'", Filepath.c_str());
            }
        }

        // Always insert into the map, even if it's the error texture
        Textures.emplace(name, Tex);
    }

    void LoadFolder(const std::string& folderPath) {
        std::string FullPath = std::format("{}/{}", CE::Global.data_path, folderPath);

        if (!std::filesystem::exists(FullPath)) {
            TraceLog(LOG_WARNING, "CE-Textures: Folder does not exist '%s'", FullPath.c_str());
            return;
        }

        if (!std::filesystem::is_directory(FullPath)) {
            TraceLog(LOG_WARNING, "CE-Textures: Path is not a directory '%s'", FullPath.c_str());
            return;
        }

        int LoadedCount = 0;
        for (const auto& Entry : std::filesystem::recursive_directory_iterator(FullPath)) {
            if (Entry.is_regular_file()) {
                const auto& Path = Entry.path();
                std::string Extension = Path.extension().string();

                if (Extension == ".png" || Extension == ".jpg" || Extension == ".bmp" ||
                    Extension == ".tga" || Extension == ".gif" || Extension == ".pic") {

                    std::filesystem::path RelativePath =
                        std::filesystem::relative(Path, CE::Global.data_path);

                    RelativePath.replace_extension("");

                    std::string Name = RelativePath.generic_string();

                    Load(Name, RelativePath.string() + Extension);
                    LoadedCount++;
                }
            }
        }

        TraceLog(LOG_INFO, "CE-Textures: Loaded %d textures from folder '%s'", LoadedCount, FullPath.c_str());
    }


    Texture2D& Get(const std::string& name) {
        auto Lst = Textures.find(name);
        if (Lst != Textures.end()) {
            return Lst->second;
        }

        TraceLog(LOG_ERROR, "CE-Textures: Couldn't find texture '%s'", name.c_str());
        TraceLog(LOG_ERROR, "CE-Textures: Maybe you forgot to load?");
        return ErrorTexture;
    }


    void Draw(const std::string& path, int posX, int posY, Color tint){
        auto It = Textures.find(path);
        if (It == Textures.end()) {
            Load(path, path);
        }
        DrawTexture(Get(path), posX, posY, tint);
    }

    int LoadedTextures() {
        return Textures.size();
    }

    int LoadedTexturesNoError() {
        int count = 0;
            for (auto& [name, tex] : Textures) {
                if (tex.id != ErrorTexture.id) count++;
            }
        return count;
    }

    int LoadedTexturesError() {
        int count = 0;
            for (auto& [name, tex] : Textures) {
                if (tex.id == ErrorTexture.id) count++;
            }
        return count; 
    }


    void Unload(const std::string& name) {
        auto Lst = Textures.find(name);

        if (Lst != Textures.end()){
            UnloadTexture(Lst->second);
            Textures.erase(Lst);
            TraceLog(LOG_INFO, "CE-Textures: Unloaded texture '%s' ", name.c_str());
            return;
        }

        TraceLog(LOG_ERROR, "CE-Textures: Tried to unload a non-existant texture!");
        return;
    }

    void UnloadAll(){
        for (auto& [name, texture] : Textures) {
           UnloadTexture(texture);
        }
        Textures.clear();
    }


    void Shutdown() {
        for (auto& [name, texture] : Textures) {
           UnloadTexture(texture);
        }
        Textures.clear();
        UnloadTexture(ErrorTexture);
    }
}