#include <raylib.h> 
#include <stdio.h>
#include <string.h>

#include "engine/tex_man/texture_manager.h"
#include "globals.h"

typedef struct {
    char name[32];
    Texture2D texture;
} TextureEntry;

static TextureEntry textures[MAX_TEXTURES];
static int textureCount = 0;
static Texture2D ErrorTex = { 0 };

void CETextures_Init(void){
    textureCount = 0;
    Image img = GenImageChecked(64, 64, 8, 8, MAGENTA, BLACK);
    ErrorTex = LoadTextureFromImage(img);
    UnloadImage(img);
}

void CETextures_Load(const char *name, const char *fileName)
{
    if (textureCount >= MAX_TEXTURES) return;

    char filepath[PATH_MAX_LEN];
    snprintf(filepath, sizeof(filepath), "%s/%s", ce_globals.path, fileName);

    Texture2D tex = { 0 };

    if (!FileExists(filepath)) {
        TraceLog(LOG_WARNING,
                 "CETexture: Missing file '%s', using ErrorTex", filepath);
        tex = ErrorTex;
    } else {
        tex = LoadTexture(filepath);

        if (tex.id == 0) {
            TraceLog(LOG_WARNING,
                     "CETexture: Failed to load '%s', using ErrorTex", filepath);
            tex = ErrorTex;
        } else {
            TraceLog(LOG_INFO,
                     "CETexture: Loaded texture from %s", filepath);
        }
    }

    textures[textureCount].texture = tex;
    strncpy(textures[textureCount].name, name, 31);
    textures[textureCount].name[31] = '\0';

    textureCount++;
}


Texture2D CETextures_Get(const char *name)
{
    static char logged_missing[1024][32];
    static int logged_count = 0;

    for (int i = 0; i < textureCount; i++) {
        if (strcmp(textures[i].name, name) == 0)
            return textures[i].texture;
    }

    for (int i = 0; i < logged_count; i++) {
        if (strcmp(logged_missing[i], name) == 0)
            return ErrorTex;
    }

    TraceLog(LOG_INFO, "CETexture: Missing texture detected %s", name);
    TraceLog(LOG_INFO, "CETexture: Maybe you forgot to use CETextures_Load");

    if (logged_count < 1024) {
        strncpy(logged_missing[logged_count], name, 31);
        logged_missing[logged_count][31] = '\0';
        logged_count++;
    }

    return ErrorTex;
}


void CETextures_UnloadAll(void){
    // Unload all textures except ErrorTex
    for (int i = 0; i < textureCount; i++){
        if (textures[i].texture.id != 0 && textures[i].texture.id != ErrorTex.id)
            UnloadTexture(textures[i].texture);
    }

    // Only unload ErrorTex once, if it exists
    if (ErrorTex.id != 0)
        UnloadTexture(ErrorTex);

    // Reset ErrorTex to empty
    ErrorTex = (Texture2D){ 0 };

    // Reset texture count
    textureCount = 0;
}




