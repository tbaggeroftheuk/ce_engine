#ifndef TGC_H
#define TGC_H

#include <stdint.h>
#include <stdio.h>
#include <SDL2/SDL.h>

#define TGC_MAGIC "TGC"
#define TGC_VERSION 0x01

typedef enum {
    TGC_RGB24  = 0,
    TGC_RGBA32 = 1
} TGC_PixelFormat;

typedef struct {
    uint32_t offset;
    uint32_t size;
} TGC_IndexEntry;

typedef struct {
    uint16_t width;
    uint16_t height;
    uint16_t fps;
    TGC_PixelFormat format;
    uint32_t frame_count;

    uint32_t payload_offset;

    TGC_IndexEntry* index;
    FILE* file;
} TGC_Video;

#pragma pack(push, 1)
typedef struct {
    char     magic[3];
    uint8_t  version;
    uint16_t width;
    uint16_t height;
    uint16_t fps;
    uint8_t  pixel_format;
    uint8_t  flags;
    uint32_t frame_count;
} TGC_Header;
#pragma pack(pop)

int  TGC_Open(TGC_Video* vid, const char* path);
void TGC_Close(TGC_Video* vid);

int  TGC_ReadFrame(TGC_Video* vid, uint32_t frame, void* out_buffer);

SDL_Texture* TGC_CreateTexture(SDL_Renderer* renderer, TGC_Video* vid);

void TGC_Play(
    SDL_Renderer* renderer,
    SDL_Texture* texture,
    TGC_Video* vid,
    int loop
);

void play_boot_video(void);

#endif
