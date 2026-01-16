#pragma once

#include <string>
#include "raylib-cpp.hpp"

namespace CE {

    // General stuff
    inline std::string game_name = "goober game";
    inline bool debug = true;
    inline bool should_exit = false;
    inline std::string engine_ver = "Build 0.1";

    inline raylib::Window main_window; // Because of raii I need to have this here

    struct Globals {

        // For game assets
        std::string data_path;
        // For setting's path
        std::string settings_path;

        // Window shit
        const int window_width = 1280;
        const int window_height = 720;
    }; 
    inline Globals Global;

    struct Settings {
        bool fullscreen = false;

        int master_vol = 100;
        int music_vol = 100;
        int sfx_vol = 100;
    };
    inline Settings setting;
}