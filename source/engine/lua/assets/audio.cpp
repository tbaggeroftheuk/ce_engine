#include <string_view>

#include "engine/lua.hpp"
#include "engine/assets/assets.hpp"

extern "C" {
    #include <raylib.h>
    #include "lua.h"
    #include "lauxlib.h"
    #include "lualib.h"
}

#define LCS luaL_checkstring
#define LCI luaL_checkinteger
#define LCN luaL_checknumber
#define LPB lua_pushboolean
#define LPS lua_pushstring
#define LPN lua_pushnumber

static bool HasAudioExt(std::string_view s) {
    auto dot = s.find_last_of('.');
    if (dot == std::string_view::npos) return false;
    std::string_view ext = s.substr(dot);
    return ext == ".ogg" || ext == ".mp3" || ext == ".flac" || ext == ".qoa" || ext == ".wav";
}

static int CE_Audio_LoadFolder(lua_State* L) {
    const char* folderPath = LCS(L, 1);
    CE::Assets::Audio::LoadFolder(folderPath);
    return 0;
}

static int CE_Audio_UnloadEverything(lua_State* L) {
    (void)L;
    CE::Assets::Audio::UnloadEverything();
    return 0;
}

static int CE_Audio_SFX_Load(lua_State* L) {
    const char* name = LCS(L, 1);
    const char* path = LCS(L, 2);
    CE::Assets::Audio::LoadSFX(name, path);
    return 0;
}

static int CE_Audio_SFX_Play(lua_State* L) {
    const char* name = LCS(L, 1);
    CE::Assets::Audio::PlaySFX(name);
    return 0;
}

static int CE_Audio_SFX_Unload(lua_State* L) {
    const char* name = LCS(L, 1);
    CE::Assets::Audio::UnloadSFX(name);
    return 0;
}

static int CE_Audio_SFX_UnloadAll(lua_State* L) {
    (void)L;
    CE::Assets::Audio::UnloadAllSFX();
    return 0;
}

static int CE_Audio_Wave_Load(lua_State* L) {
    const char* name = LCS(L, 1);
    const char* path = LCS(L, 2);
    CE::Assets::Audio::LoadSFX_Wave(name, path);
    return 0;
}

static int CE_Audio_Wave_Play(lua_State* L) {
    const char* name = LCS(L, 1);
    CE::Assets::Audio::PlaySFXWave(name);
    return 0;
}

static int CE_Audio_Wave_Unload(lua_State* L) {
    const char* name = LCS(L, 1);
    CE::Assets::Audio::UnloadSFX_Wave(name);
    return 0;
}

static int CE_Audio_Wave_UnloadAll(lua_State* L) {
    (void)L;
    CE::Assets::Audio::UnloadAllSFX_Wave();
    return 0;
}

static int CE_Audio_Music_Load(lua_State* L) {
    const char* a = LCS(L, 1);
    const char* b = LCS(L, 2);

    std::string_view s1(a);
    std::string_view s2(b);

    const char* name = a;
    const char* path = b;

    // Back-compat: if called like Load("music.mp3", "Name"), swap.
    if (HasAudioExt(s1) && !HasAudioExt(s2)) {
        name = b;
        path = a;
    }

    CE::Assets::Audio::LoadMusic(name, path);
    return 0;
}

static int CE_Audio_Music_LoadFolder(lua_State* L) {
    const char* folderPath = LCS(L, 1);
    CE::Assets::Audio::LoadFolderMusic(folderPath);
    return 0;
}

static int CE_Audio_Music_Play(lua_State* L) {
    const char* name = LCS(L, 1);
    CE::Assets::Audio::PlayMusic(name);
    return 0;
}

static int CE_Audio_Music_Update(lua_State* L) {
    (void)L;
    CE::Assets::Audio::UpdateMusic();
    return 0;
}

static int CE_Audio_Music_Pause(lua_State* L) {
    const char* name = LCS(L, 1);
    CE::Assets::Audio::PauseMusic(name);
    return 0;
}

static int CE_Audio_Music_Resume(lua_State* L) {
    const char* name = LCS(L, 1);
    CE::Assets::Audio::ResumeMusic(name);
    return 0;
}

static int CE_Audio_Music_Stop(lua_State* L) {
    const char* name = LCS(L, 1);
    CE::Assets::Audio::StopMusic(name);
    return 0;
}

static int CE_Audio_Music_StopAll(lua_State* L) {
    (void)L;
    CE::Assets::Audio::StopAllMusic();
    return 0;
}

static int CE_Audio_Music_PauseAll(lua_State* L) {
    (void)L;
    CE::Assets::Audio::PauseAllMusic();
    return 0;
}

static int CE_Audio_Music_ResumeAll(lua_State* L) {
    (void)L;
    CE::Assets::Audio::ResumeAllMusic();
    return 0;
}

static int CE_Audio_Music_PlayAll(lua_State* L) {
    (void)L;
    CE::Assets::Audio::PlayAllMusic();
    return 0;
}

static int CE_Audio_Music_SetLoop(lua_State* L) {
    const char* name = LCS(L, 1);
    const float start = (float)LCN(L, 2);
    const float end = (float)LCN(L, 3);
    CE::Assets::Audio::SetMusicLoop(name, start, end);
    return 0;
}

static int CE_Audio_Music_Unload(lua_State* L) {
    const char* name = LCS(L, 1);
    CE::Assets::Audio::UnloadMusic(name);
    return 0;
}

static int CE_Audio_Music_UnloadAll(lua_State* L) {
    (void)L;
    CE::Assets::Audio::UnloadAllMusic();
    return 0;
}

namespace CE::Lua::Functions::Assets::Audio {
    void Register(lua_State* L) {
        lua_newtable(L); // Audio table

        lua_pushcfunction(L, CE_Audio_LoadFolder);
        lua_setfield(L, -2, "LoadFolder");

        lua_pushcfunction(L, CE_Audio_UnloadEverything);
        lua_setfield(L, -2, "UnloadEverything");

        // SFX table
        lua_newtable(L);
        lua_pushcfunction(L, CE_Audio_SFX_Load);
        lua_setfield(L, -2, "Load");
        lua_pushcfunction(L, CE_Audio_SFX_Play);
        lua_setfield(L, -2, "Play");
        lua_pushcfunction(L, CE_Audio_SFX_Unload);
        lua_setfield(L, -2, "Unload");
        lua_pushcfunction(L, CE_Audio_SFX_UnloadAll);
        lua_setfield(L, -2, "UnloadAll");
        lua_setfield(L, -2, "SFX"); // Audio.SFX = table

        lua_newtable(L);
        lua_pushcfunction(L, CE_Audio_Wave_Load);
        lua_setfield(L, -2, "Load");
        lua_pushcfunction(L, CE_Audio_Wave_Play);
        lua_setfield(L, -2, "Play");
        lua_pushcfunction(L, CE_Audio_Wave_Unload);
        lua_setfield(L, -2, "Unload");
        lua_pushcfunction(L, CE_Audio_Wave_UnloadAll);
        lua_setfield(L, -2, "UnloadAll");
        lua_setfield(L, -2, "Wave"); // Audio.Wave = table

        // Music table
        lua_newtable(L);
        lua_pushcfunction(L, CE_Audio_Music_Load);
        lua_setfield(L, -2, "Load");
        lua_pushcfunction(L, CE_Audio_Music_LoadFolder);
        lua_setfield(L, -2, "LoadFolder");
        lua_pushcfunction(L, CE_Audio_Music_Play);
        lua_setfield(L, -2, "Play");
        lua_pushcfunction(L, CE_Audio_Music_Update);
        lua_setfield(L, -2, "Update");
        lua_pushcfunction(L, CE_Audio_Music_Pause);
        lua_setfield(L, -2, "Pause");
        lua_pushcfunction(L, CE_Audio_Music_Resume);
        lua_setfield(L, -2, "Resume");
        lua_pushcfunction(L, CE_Audio_Music_Stop);
        lua_setfield(L, -2, "Stop");
        lua_pushcfunction(L, CE_Audio_Music_StopAll);
        lua_setfield(L, -2, "StopAll");
        lua_pushcfunction(L, CE_Audio_Music_PauseAll);
        lua_setfield(L, -2, "PauseAll");
        lua_pushcfunction(L, CE_Audio_Music_ResumeAll);
        lua_setfield(L, -2, "ResumeAll");
        lua_pushcfunction(L, CE_Audio_Music_PlayAll);
        lua_setfield(L, -2, "PlayAll");
        lua_pushcfunction(L, CE_Audio_Music_SetLoop);
        lua_setfield(L, -2, "SetLoop");
        lua_pushcfunction(L, CE_Audio_Music_Unload);
        lua_setfield(L, -2, "Unload");
        lua_pushcfunction(L, CE_Audio_Music_UnloadAll);
        lua_setfield(L, -2, "UnloadAll");
        lua_setfield(L, -2, "Music"); // Audio.Music = table

        lua_setglobal(L, "Audio");
    }
}
