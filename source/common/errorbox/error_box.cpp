#include <string>
#include <cstdlib>
#include <iostream>

#include "common/errorbox/error_box.hpp"

extern "C" {
    #include <SDL2/SDL.h>
}

void ShowError(const std::string& msg) {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::cerr << "SDL_Init Error: " << SDL_GetError() << std::endl;
        return;
    }

    SDL_ShowSimpleMessageBox(
        SDL_MESSAGEBOX_ERROR,
        "Fatal Error",        
        msg.c_str(),           
        nullptr              
    );

    SDL_Quit(); 
    std::exit(1);
}
