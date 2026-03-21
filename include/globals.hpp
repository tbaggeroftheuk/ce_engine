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
    inline bool isWinowOpen = true;
    inline std::string game_name = "CE Runtime 0.1";

    #ifdef NDEBUG
    inline bool Debug = true;
    #else
    inline bool Debug = false;
    #endif

    inline int should_exit = 0;
    inline std::string DATA_FILE_NAME = CE_DATA_FILE_NAME;

    inline std::string engine_ver = "Build 0.1";
    inline std::string game_ver = "";
    inline int int_engine_ver = 1;

    inline int MaxFPS = 60;

    struct Globals {

        // For game assets
        std::string data_path;
        // For setting's path
        std::string settings_path;
        // Save path
        std::string save_path;

        // Window shit
        int window_width = 1280;
        int window_height = 720;
    }; 
    inline Globals Global;

    struct Setting {
        bool fullscreen = false;

        int master_vol = 100;
        int music_vol = 100;
        int sfx_vol = 100;
    };
    inline Setting Settings;

    enum class GameState { 
        None,
        MainMenu,
        PauseMenu,
        InGame
    };

    inline GameState currentGameState = GameState::None;
    inline std::string currentGameStateName = "None";

}

namespace CE::Flags {
    inline bool show_plugin_logs = true;
    inline bool bypass_data_file_crc_crash = false;
    inline bool debug_win = false;
    inline bool custom_data_path = false;
}
