#ifndef TEXTURE_MANAGER_H
#define TEXTURE_MANAGER_H

#include <raylib.h>

void CETextures_Init(void);

void CETextures_Load(const char *name, const char *fileName);

Texture2D CETextures_Get(const char *name);

void CETextures_UnloadAll(void);

#endif