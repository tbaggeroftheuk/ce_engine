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
    static std::unordered_map<std::string, Sound> SFXs;
    static std::unordered_map<std::string, Wave> SFX_Wavs;
    static std::unordered_map<std::string, Music> Musics;

    bool audioDeviceReady = false;
    
    void Init() {
        InitAudioDevice();
        if(!IsAudioDeviceReady()) {
            TraceLog(LOG_ERROR, "CE-Audio: Unable to init audio device");
            audioDeviceReady = false;
        }
        SetMasterVolume(static_cast<float>(CE::Settings.master_vol));
    }

    void LoadSFX(const std::string& name, const std::string& filename) {
        namespace fs = std::filesystem;
        std::string filePath = std::format("{}/{}", CE::Global.data_path, filename);

        if (!fs::exists(filePath)) {
            TraceLog(LOG_WARNING, "CE-Audio: Missing audio asset: %s", filename.c_str());
            return;
        }

        Sound SFX = LoadSound(filePath.c_str());

        if(!IsSoundValid(SFX)) {
            TraceLog(LOG_WARNING, "CE-Audio: Invalid sound, unloading: %s", filePath.c_str());
            UnloadSound(SFX);
        }

        SFXs.emplace(name, SFX);
    }

    void LoadSFX_Wave(const std::string& name, const std::string& filename) {
        namespace fs = std::filesystem;
        std::string filePath = std::format("{}/{}", CE::Global.data_path, filename);

        if (!fs::exists(filePath)) {
            TraceLog(LOG_WARNING, "CE-Audio: Missing audio asset: %s", filename.c_str());
            return;
        }

        Wave SFX = LoadWave(filePath.c_str());

        if(!IsWaveValid(SFX)) {
            TraceLog(LOG_WARNING, "CE-Audio: Invalid wave, unloading: %s", filePath.c_str());
            UnloadWave(SFX);
        }

        SFX_Wavs.emplace(name, SFX);
    }

    void LoadFolder(const std::string& folderPath) {

    }

    void Unload(const std::string& name) {

    }

    void UnloadAll(){

    }


    void Shutdown() {
        CloseAudioDevice();
    }
}