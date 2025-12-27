#include "ui/tgc.h"
#include <stdlib.h>
#include <string.h>


int TGC_Open(TGC_Video* vid, const char* path)
{
    memset(vid, 0, sizeof(*vid));

    FILE* f = fopen(path, "rb");
    if (!f) return 0;

    TGC_Header h;
    if (fread(&h, sizeof(h), 1, f) != 1) goto fail;

    if (memcmp(h.magic, TGC_MAGIC, 3) != 0) goto fail;
    if (h.version != TGC_VERSION) goto fail;

    vid->width  = h.width;
    vid->height = h.height;
    vid->fps    = h.fps;
    vid->format = (TGC_PixelFormat)h.pixel_format;
    vid->frame_count = h.frame_count;

    vid->index = (TGC_IndexEntry*)malloc(sizeof(TGC_IndexEntry) * h.frame_count);
    if (!vid->index) goto fail;

    if (fread(vid->index, sizeof(TGC_IndexEntry), h.frame_count, f) != h.frame_count)
        goto fail;

    vid->payload_offset = (uint32_t)ftell(f);
    vid->file = f;

    return 1;

fail:
    if (vid->index) free(vid->index);
    fclose(f);
    return 0;
}

void TGC_Close(TGC_Video* vid)
{
    if (vid->file) fclose(vid->file);
    if (vid->index) free(vid->index);
    memset(vid, 0, sizeof(*vid));
}


int TGC_ReadFrame(TGC_Video* vid, uint32_t frame, void* out_buffer)
{
    if (frame >= vid->frame_count) return 0;

    TGC_IndexEntry* e = &vid->index[frame];

    fseek(vid->file, vid->payload_offset + e->offset, SEEK_SET);
    return fread(out_buffer, 1, e->size, vid->file) == e->size;
}

SDL_Texture* TGC_CreateTexture(SDL_Renderer* r, TGC_Video* vid)
{
    Uint32 fmt = (vid->format == TGC_RGBA32)
        ? SDL_PIXELFORMAT_RGBA32
        : SDL_PIXELFORMAT_RGB24;

    return SDL_CreateTexture(
        r,
        fmt,
        SDL_TEXTUREACCESS_STREAMING,
        vid->width,
        vid->height
    );
}

void TGC_Play(
    SDL_Renderer* renderer,
    SDL_Texture* texture,
    TGC_Video* vid,
    int loop
) {
    uint32_t bpp = (vid->format == TGC_RGBA32) ? 4 : 3;
    uint32_t pitch = vid->width * bpp;
    uint32_t frame_ms = 1000 / vid->fps;

    void* buffer = malloc(pitch * vid->height);
    if (!buffer) return;

    int running = 1;
    SDL_Event e;

    do {
        for (uint32_t i = 0; i < vid->frame_count && running; i++) {
            uint32_t start = SDL_GetTicks();

            while (SDL_PollEvent(&e)) {
                if (e.type == SDL_QUIT) running = 0;
            }

            TGC_ReadFrame(vid, i, buffer);
            SDL_UpdateTexture(texture, NULL, buffer, pitch);

            SDL_RenderClear(renderer);
            SDL_RenderCopy(renderer, texture, NULL, NULL);
            SDL_RenderPresent(renderer);

            uint32_t elapsed = SDL_GetTicks() - start;
            if (elapsed < frame_ms)
                SDL_Delay(frame_ms - elapsed);
        }
    } while (loop && running);

    free(buffer);
}
