#pragma once

#include <string>
#include "common/tdf.hpp"

#define DEFAULT_BUFFER 500

extern "C" {
    #include <raylib.h>
}



namespace CE {
    // General stuff
    inline Vector2 MousePos;
    inline std::string GameState = "MainMenu";
    inline std::string game_name = "CE Runtime 0.1";
    inline bool Debug = true;
    inline int should_exit = 0;

    inline std::string engine_ver = "Build 0.1";
    inline int int_engine_ver = 1;

    struct Globals {

        // For game assets
        std::string data_path;
        // For setting's path
        std::string settings_path;
        // Save path
        std::string save_path;

        // Window shit
        const int window_width = 1280;
        const int window_height = 720;
    }; 
    inline Globals Global;

    struct Setting {
        bool fullscreen = false;

        int master_vol = 100;
        int music_vol = 100;
        int sfx_vol = 100;
    };
    inline Setting Settings;

    enum Gamestate {
        MainMenu,
        PauseMenu,
        InGame,
        None
    };

}

namespace CE::Flags {
    inline bool show_plugin_logs = true;
    inline bool bypass_data_file_crc_crash = false;
    inline bool debug_win = false;
}