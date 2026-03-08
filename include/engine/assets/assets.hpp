#pragma once
#include <string>

extern "C" { 
    #include "raylib.h" 
}

namespace CE::Assets::Textures {
    void Init();
    
    void Load(const std::string& name, const std::string& filename);
    void LoadFolder(const std::string& folderPath);
    Texture2D& Get(const std::string& name);
    void Draw(const std::string& path, const int posX, const int posY, const Color tint = WHITE);

    int LoadedTextures();
    int LoadedTexturesNoError();
    int LoadedTexturesError();
    
    void Unload(const std::string& name);
    void UnloadAll();
    void Shutdown(); 
}
