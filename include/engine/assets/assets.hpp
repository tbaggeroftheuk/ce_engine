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
    void Draw(const std::string& name, const int posX, const int posY, const Color tint = WHITE);
    bool Exists(const std::string& name);

    int LoadedTextures();
    int LoadedTexturesNoError();
    int LoadedTexturesError();
    
    void Unload(const std::string& name);
    void UnloadAll();
    void Shutdown(); 
}

namespace CE::Assets::Audio {
    void Init();

    void LoadSFX(const std::string& name, const std::string& filename);
    void LoadSFX_Wave(const std::string& name, const std::string& filename);
    void LoadMusic(const std::string& name, const std::string& filename);
    void LoadFolder(const std::string& folderPath);
    void LoadFolderMusic(const std::string& folderPath);

    void PlaySFX(const std::string& name);
    void PlaySFXWave(const std::string& name);

    void PlayMusic(const std::string& name);
    void UpdateMusic();
    void PauseMusic(const std::string& name);
    void ResumeMusic(const std::string& name);
    void StopMusic(const std::string& name);
    void StopAllMusic();
    void PauseAllMusic();
    void ResumeAllMusic();
    void PlayAllMusic();
    void SetMusicLoop(const std::string& name, const float start, const float end);

    void UnloadSFX_Wave(const std::string& name);
    void UnloadSFX(const std::string& name);
    void UnloadMusic(const std::string& name);
    
    void UnloadAllSFX_Wave();
    void UnloadAllSFX();
    void UnloadAllMusic();
    void UnloadEverything();

    void Shutdown();
}