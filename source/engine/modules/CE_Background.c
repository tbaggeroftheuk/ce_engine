#include <stdio.h>
#include <raylib.h>

#include "globals.h"
#include "engine/modules/CE_Background.h"


void ce_background_set(char *path_to_image) {
    char full_path[PATH_MAX_LEN];
    snprintf(full_path, PATH_MAX_LEN, "%s%s", ce_globals.path, path_to_image);
    ce_global_textures.bg_texture = LoadTexture(full_path);
    DrawTexture(ce_global_textures.bg_texture, 0, 0, WHITE);
}

