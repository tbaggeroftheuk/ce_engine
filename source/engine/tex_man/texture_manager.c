#include <raylib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

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

static bool CETextures_IsImageExt(const char *ext) {
    return strcmp(ext, ".png")  == 0 ||
           strcmp(ext, ".jpg")  == 0 ||
           strcmp(ext, ".jpeg") == 0 ||
           strcmp(ext, ".bmp")  == 0 ||
           strcmp(ext, ".tga")  == 0;
}

void CETextures_Load(const char *name, const char *fileName) {
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

Texture2D CETextures_Get(const char *name) {
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

void CETextures_UnloadAll(void) { 
    for (int i = 0; i < textureCount; i++){
        if (textures[i].texture.id != 0 && textures[i].texture.id != ErrorTex.id)
            UnloadTexture(textures[i].texture);
    }

    if (ErrorTex.id != 0)
        UnloadTexture(ErrorTex);

    ErrorTex = (Texture2D){ 0 };
    textureCount = 0;
}

void CETextures_LoadDir(const char *currentDir) {
    char fullDir[PATH_MAX_LEN];
    snprintf(fullDir, sizeof(fullDir), "%s/%s", ce_globals.path, currentDir);

    if (!DirectoryExists(fullDir)) return;

    FilePathList files = LoadDirectoryFiles(fullDir);

    for (unsigned int i = 0; i < files.count; i++) {
        const char *path = files.paths[i];

        if (DirectoryExists(path)) {
            char nextDir[PATH_MAX_LEN];
            snprintf(nextDir, sizeof(nextDir), "%s/%s",
                     currentDir, GetFileName(path));
            CETextures_LoadDir(nextDir);
        } else {
            const char *ext = GetFileExtension(path);
            if (!CETextures_IsImageExt(ext)) continue;

            char name[64];
            snprintf(name, sizeof(name), "%s", GetFileNameWithoutExt(path));

            char relPath[PATH_MAX_LEN];
            snprintf(relPath, sizeof(relPath), "%s/%s",
                     currentDir, GetFileName(path));

            CETextures_Load(name, relPath);
        }
    }

    UnloadDirectoryFiles(files);
}


