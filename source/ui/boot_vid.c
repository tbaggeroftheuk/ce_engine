#include <SDL2/SDL.h>

#include "globals.h"
#include "ui/tgc.h"

void play_boot_video(void) {
    char path_combined[256];
    snprintf(path_combined, sizeof(path_combined), "%s/media/%s", ce_globals.path, ce_globals.startup_video);
    
    TGC_Video vid;
    if (!TGC_Open(&vid, path_combined)) {
        fprintf(stderr, "Failed to open TGC video: %s\n", path_combined);
        return;
    }

    SDL_Texture* texture = TGC_CreateTexture(ce_globals.renderer, &vid);
    if (!texture) {
        fprintf(stderr, "Failed to create texture for video\n");
        TGC_Close(&vid);
        return;
    }

    TGC_Play(ce_globals.renderer, texture, &vid, 0);

    SDL_DestroyTexture(texture);
    TGC_Close(&vid);
}    
