#include <SDL2/SDL.h>

void show_error_box(const char *title, const char *message){
    SDL_MessageBoxButtonData buttons[] = {
        {
            SDL_MESSAGEBOX_BUTTON_RETURNKEY_DEFAULT,
            0,
            "OK"
        }
    };

    SDL_MessageBoxData msgbox = {
        SDL_MESSAGEBOX_ERROR,     // flags (error icon)
        NULL,                     // no parent window
        title,                    // title
        message,                  // message text
        SDL_arraysize(buttons),   // number of buttons
        buttons,                  // button array
        NULL                      // color scheme (NULL = default)
    };

    SDL_ShowMessageBox(&msgbox, NULL);
}
