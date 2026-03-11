#include <string>
#include <format>
#include <unordered_map>
#include <filesystem>

#include "globals.hpp"

#include "engine/assets/assets.hpp"

extern "C" {
    #include <raylib.h>
}

namespace CE::Assets::Audio {
    
    struct MusicLoopData {
        float loopStart;
        float loopEnd;
    };

    static std::unordered_map<std::string, Sound> SFXs;
    static std::unordered_map<std::string, Wave> SFX_Wavs;
    static std::unordered_map<std::string, Music> Musics;
    static std::unordered_map<std::string, MusicLoopData> MusicLoops;

    bool audioDeviceReady = false;
    
    void Init() {
        InitAudioDevice();
        if(!IsAudioDeviceReady()) {
            TraceLog(LOG_ERROR, "CE-Audio: Unable to init audio device");
            audioDeviceReady = false;
        }
        SetMasterVolume(static_cast<float>(CE::Settings.master_vol));
        audioDeviceReady = true;
    }

    void LoadSFX(const std::string& name, const std::string& filename) {
        if(audioDeviceReady) {
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
                return;
            }

            SFXs.emplace(name, SFX);
            return;
        } else {
            TraceLog(LOG_WARNING, "CE-Audio: Audio device is uninitialize!");
            return;
        }
    }

    void LoadSFX_Wave(const std::string& name, const std::string& filename) {
        if(audioDeviceReady) { 
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
                return;
            }

            SFX_Wavs.emplace(name, SFX);
            return;
        } else {
            TraceLog(LOG_WARNING, "CE-Audio: Audio device is uninitialize!");
            return;
        }
    }

    void LoadMusic(const std::string& name, const std::string& filename) {
        if(audioDeviceReady) {
            namespace fs = std::filesystem;
            std::string filePath = std::format("{}/{}", CE::Global.data_path, filename);

            if(!fs::exists(filePath)) {
                 TraceLog(LOG_WARNING, "CE-Audio: Missing audio asset: %s", filename.c_str());
                 return;
            }

            Music Music_stream = LoadMusicStream(filePath.c_str());

            if(!IsMusicValid(Music_stream)) {
                TraceLog(LOG_WARNING, "CE-Audio: Invalid music, unloading: %s", filename.c_str());
                UnloadMusicStream(Music_stream);
                return;
            }

            Musics.emplace(name, Music_stream);
            return;
        } else  {
            TraceLog(LOG_WARNING, "CE-Audio: Audio device is uninitialize!");
        }
    }

    void LoadFolder(const std::string& folderPath) {
        std::string FullPath = std::format("{}/{}", CE::Global.data_path, folderPath);

        if (!std::filesystem::exists(FullPath)) {
            TraceLog(LOG_WARNING, "CE-Audio: Folder does not exist '%s'", FullPath.c_str());
            return;
        }

        if (!std::filesystem::is_directory(FullPath)) {
            TraceLog(LOG_WARNING, "CE-Audio: Path is not a directory '%s'", FullPath.c_str());
            return;
        }

        int LoadedCount = 0;
        for (const auto& Entry : std::filesystem::recursive_directory_iterator(FullPath)) {
            if (Entry.is_regular_file()) {
                const auto& Path = Entry.path();
                std::string Extension = Path.extension().string();
                if (Extension == ".wav") {
                    std::filesystem::path RelativePath =
                        std::filesystem::relative(Path, CE::Global.data_path);

                    RelativePath.replace_extension("");
                    std::string Name = RelativePath.generic_string();
                    LoadSFX_Wave(Name, RelativePath.string() + Extension);
                    LoadedCount++;
                }

                if (Extension == ".ogg" || Extension == ".mp3" || Extension == ".flac" || Extension == ".qoa") {
                    std::filesystem::path RelativePath =
                        std::filesystem::relative(Path, CE::Global.data_path);
                    RelativePath.replace_extension("");
                    std::string Name = RelativePath.generic_string();
                    LoadSFX(Name, RelativePath.string() + Extension);
                    LoadedCount++;
                }
            }
        }
        TraceLog(LOG_INFO, "CE-Audio: Loaded %d audio files from folder '%s'", LoadedCount, FullPath.c_str());
    }

    void LoadFolderMusic(const std::string& folderPath) {
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
                if (Extension == ".ogg" || Extension == ".mp3" || Extension == ".flac" || Extension == ".qoa") {
                    std::filesystem::path RelativePath =
                        std::filesystem::relative(Path, CE::Global.data_path);
                    RelativePath.replace_extension("");
                    std::string Name = RelativePath.generic_string();
                    LoadMusic(Name, RelativePath.string() + Extension);
                    LoadedCount++;
                }
            }
        }
        TraceLog(LOG_INFO, "CE-Audio: Loaded %d music files from folder '%s'", LoadedCount, FullPath.c_str());
    }

    void PlaySFX(const std::string& name) {
        auto NormalSound = SFXs.find(name);

        if (NormalSound != SFXs.end()) {
            PlaySound(NormalSound->second);
            return;
        }
        TraceLog(LOG_WARNING, "CE-Audio: Tried to play unloaded or missing SFX: %s", name.c_str());
    }

    void PlaySFXWave(const std::string& name) {
        auto WaveSound = SFX_Wavs.find(name);

        if (WaveSound != SFX_Wavs.end()) {
            Sound s = LoadSoundFromWave(WaveSound->second);
            PlaySound(s);
            UnloadSound(s);
            return;
        }
        TraceLog(LOG_WARNING, "CE-Audio: Tried to play unloaded or missing wave: %s", name.c_str());
    }

    void PlayMusic(const std::string& name) {
        auto Moosic = Musics.find(name);

        if (Moosic != Musics.end()) {
            PlayMusicStream(Moosic->second);
            return;
        }
        TraceLog(LOG_WARNING, "CE-Audio: Tried to play unloaded or missing music: %s", name.c_str());
    }

    void UpdateMusic() {
        for (auto& [name, music] : Musics) {

            UpdateMusicStream(music);

            auto it = MusicLoops.find(name);
            if (it != MusicLoops.end()) {
                float time = GetMusicTimePlayed(music);

                if (time >= it->second.loopEnd) {
                    SeekMusicStream(music, it->second.loopStart);
                }
            }
        }
    }

    void PauseMusic(const std::string& name) {
        auto Moosic = Musics.find(name);

        if (Moosic != Musics.end()) {
            PauseMusicStream(Moosic->second);
            return;
        }
        TraceLog(LOG_WARNING, "CE-Audio: tried to pause an unloaded or missing music: %s", name.c_str());
        return;
    }

    void ResumeMusic(const std::string& name) {
        auto Moosic = Musics.find(name);

        if (Moosic != Musics.end()) {
            ResumeMusicStream(Moosic->second);
            return;
        }
        TraceLog(LOG_WARNING, "CE-Audio: tried to resume an unloaded or missing music: %s", name.c_str());
        return;
    }

    void StopMusic(const std::string& name) {
        auto Moosic = Musics.find(name);

        if (Moosic != Musics.end()) {
            StopMusicStream(Moosic->second);
            return;
        }
        TraceLog(LOG_WARNING, "CE-Audio: tried to stop an unloaded or missing music: %s", name.c_str());
    }

    void StopAllMusic() {
        for (auto& [name, Music] : Musics) {
           StopMusicStream(Music);
        }
    }

    void PauseAllMusic() {
        for (auto& [name, Music] : Musics) {
           PauseMusicStream(Music);
        }
    }

    void ResumeAllMusic() {
        for (auto& [name, Music] : Musics) {
           ResumeMusicStream(Music);
        }
    }

    void PlayAllMusic() {
        for (auto& [name, Music] : Musics) {
           ResumeMusicStream(Music);
        }
    }

    void SetMusicLoop(const std::string& name, const float start, const float end) {
        auto Moosics = Musics.find(name);

        if (Moosics == Musics.end()) {
            TraceLog(LOG_WARNING, "CE-Audio: tried to register a music loop for an unloaded or missing assets: %s", name.c_str());
            return;
        }
        MusicLoops[name] = {start, end};
        return;
    }

    void UnloadSFX_Wave(const std::string& name) {
        auto lst = SFX_Wavs.find(name);

        if (lst != SFX_Wavs.end()) {
            UnloadWave(lst->second);
            SFX_Wavs.erase(name);
            TraceLog(LOG_INFO, "CE-Audio: Unloaded Wave: %s", name.c_str());
            return;
        }
        TraceLog(LOG_ERROR, "CE-Audio: Tried to unload a non-existant wave: %s", name.c_str());
        return;
    }

    void UnloadSFX(const std::string& name) {
        auto lst = SFXs.find(name);
        if (lst != SFXs.end()) {
            UnloadSound(lst->second);
            SFXs.erase(lst);
            TraceLog(LOG_INFO, "CE-Audio: Unloaded sound: %s", name.c_str());
            return;
        }
        TraceLog(LOG_ERROR, "CE-Audio: tried to unload a non-existant sound: %s", name.c_str());
        return;
    }

    void UnloadMusic(const std::string& name) {
        auto lst = Musics.find(name);
        if (lst != Musics.end()) {
            UnloadMusicStream(lst->second);
            TraceLog(LOG_INFO, "CE-Audio: Unloaded music stream: %s", name.c_str());
            return;
        }
        TraceLog(LOG_ERROR, "CE-Audio: tried to unload a non-existant music stream: %s", name.c_str());
        return;
    }

    void UnloadAllSFX_Wave() {
        for (auto& [name, Wave] : SFX_Wavs) {
           UnloadWave(Wave);
        }
        SFX_Wavs.clear();
    }

    void UnloadAllSFX() {
        for (auto& [name, Sound] : SFXs) {
           UnloadSound(Sound);
        }
        SFXs.clear();
    }

    void UnloadAllMusic() {
        for (auto& [name, Music] : Musics) {
           UnloadMusicStream(Music);
        }
        Musics.clear();
    }

    void UnloadEverything() {
        // Unload wavs
        for (auto& [name, Wave] : SFX_Wavs) {
           UnloadWave(Wave);
        }
        SFX_Wavs.clear();
        
        // Unload normal sfx's
        for (auto& [name, Sound] : SFXs) {
            UnloadSound(Sound);
        }
        SFXs.clear();

        // Unload music streams
        for (auto& [name, Music] : Musics) {
           UnloadMusicStream(Music);
        }
        Musics.clear();

        return;
    }

    void Shutdown() {
        UnloadAllSFX();
        UnloadAllSFX_Wave();
        UnloadAllMusic();
        CloseAudioDevice();
    }
}