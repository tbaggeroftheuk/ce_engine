#include "raylib.h"
#include <vlc/vlc.h>
#include <string.h>

#ifdef _WIN32
    #include <malloc.h>   // _aligned_malloc / _aligned_free
#else
    #include <stdlib.h>   // aligned_alloc / free
    #include <unistd.h>
#endif

#include "globals.h"
#include "engine/modules/vid_player/video.h"

/* =========================================================
   Cross-platform aligned allocation helpers
   ========================================================= */

static void *ce_aligned_alloc(size_t alignment, size_t size)
{
#ifdef _WIN32
    return _aligned_malloc(size, alignment);
#else
    return aligned_alloc(alignment, size);
#endif
}

static void ce_aligned_free(void *ptr)
{
#ifdef _WIN32
    _aligned_free(ptr);
#else
    free(ptr);
#endif
}

/* =========================================================
   VLC callbacks
   ========================================================= */

static void *lock(void *opaque, void **planes)
{
    VideoPlayer *vp = (VideoPlayer *)opaque;
    *planes = vp->vlc_buffer;   // VLC writes here
    return NULL;
}

static void unlock(void *opaque, void *picture, void *const *planes)
{
    (void)picture;
    (void)planes;

    VideoPlayer *vp = (VideoPlayer *)opaque;

    if (!vp->vlc_buffer || !vp->pixels) return;


    memcpy(vp->pixels, vp->vlc_buffer, vp->width * vp->height * 4);


    unsigned char *p = vp->pixels;
    int count = vp->width * vp->height;

    for (int i = 0; i < count; i++)
    {
        unsigned char b = p[0];
        p[0] = p[2];
        p[2] = b;   
        p += 4;
    }
}


static void display(void *opaque, void *picture)
{
    (void)opaque;
    (void)picture;
    // Required by VLC but unused
}

/* =========================================================
   VideoPlayer implementation
   ========================================================= */

VideoPlayer VideoPlayer_Init(const char *filePath, int width, int height)
{
    VideoPlayer vp = {0};
    vp.width  = width;
    vp.height = height;

    size_t bufferSize = (size_t)width * height * 4;

    /* ---- Allocate aligned buffers ---- */
    vp.pixels     = (unsigned char *)ce_aligned_alloc(32, bufferSize);
    vp.vlc_buffer = (unsigned char *)ce_aligned_alloc(32, bufferSize);

    if (!vp.pixels || !vp.vlc_buffer)
    {
        TraceLog(LOG_ERROR, "VideoPlayer: Failed to allocate buffers");
        if (vp.pixels)     ce_aligned_free(vp.pixels);
        if (vp.vlc_buffer) ce_aligned_free(vp.vlc_buffer);
        return vp;
    }

    memset(vp.pixels, 0, bufferSize);
    memset(vp.vlc_buffer, 0, bufferSize);

    /* ---- VLC instance ---- */
    const char *vlc_args[] = {
        "--no-xlib",
        "--no-video-title-show",
        "--no-snapshot-preview",
        "--verbose=1"
    };

    vp.vlcInst = libvlc_new(
        sizeof(vlc_args) / sizeof(vlc_args[0]),
        vlc_args
    );

    if (!vp.vlcInst)
    {
        TraceLog(LOG_ERROR, "VideoPlayer: Failed to create VLC instance");
        ce_aligned_free(vp.pixels);
        ce_aligned_free(vp.vlc_buffer);
        return vp;
    }

    /* ---- Load media ---- */
    libvlc_media_t *media =
        libvlc_media_new_path(vp.vlcInst, filePath);

    if (!media)
    {
        TraceLog(LOG_ERROR, "VideoPlayer: Failed to load media: %s", filePath);
        libvlc_release(vp.vlcInst);
        ce_aligned_free(vp.pixels);
        ce_aligned_free(vp.vlc_buffer);
        return vp;
    }

    libvlc_media_parse(media);  // deprecated but still OK

    /* ---- Debug track info ---- */
    if (ce_globals.debug)
    {
        libvlc_media_track_t **tracks = NULL;
        unsigned int count = libvlc_media_tracks_get(media, &tracks);

        TraceLog(LOG_INFO, "VideoPlayer: Media tracks: %u", count);

        for (unsigned int i = 0; i < count; i++)
        {
            if (tracks[i]->i_type == libvlc_track_video)
            {
                TraceLog(LOG_INFO,
                    "Video track %u: %ux%u",
                    i,
                    tracks[i]->video->i_width,
                    tracks[i]->video->i_height);
            }
        }

        libvlc_media_tracks_release(tracks, count);
    }

    /* ---- Media player ---- */
    vp.vlcPlayer = libvlc_media_player_new_from_media(media);
    libvlc_media_release(media);

    if (!vp.vlcPlayer)
    {
        TraceLog(LOG_ERROR, "VideoPlayer: Failed to create media player");
        libvlc_release(vp.vlcInst);
        ce_aligned_free(vp.pixels);
        ce_aligned_free(vp.vlc_buffer);
        return vp;
    }

    /* ---- VLC video output ---- */
    libvlc_video_set_callbacks(
        vp.vlcPlayer,
        lock,
        unlock,
        display,
        &vp
    );

    libvlc_video_set_format(
        vp.vlcPlayer,
        "RV32",
        width,
        height,
        width * 4
    );

    /* ---- Raylib texture ---- */
    Image img = {0};
    img.data    = vp.pixels;
    img.width   = width;
    img.height  = height;
    img.mipmaps = 1;
    img.format  = PIXELFORMAT_UNCOMPRESSED_R8G8B8A8;

    vp.texture = LoadTextureFromImage(img);

    TraceLog(LOG_INFO,
        "VideoPlayer: Initialized (%dx%d)",
        width, height
    );

    return vp;
}

void VideoPlayer_Update(VideoPlayer *vp)
{
    if (!vp || !vp->pixels) return;
    UpdateTexture(vp->texture, vp->pixels);
}

void VideoPlayer_Play(VideoPlayer *vp)
{
    if (!vp || !vp->vlcPlayer) return;

    if (libvlc_media_player_play(vp->vlcPlayer) == -1)
    {
        TraceLog(LOG_ERROR, "VideoPlayer: Playback failed");
    }
}

void VideoPlayer_Pause(VideoPlayer *vp)
{
    if (!vp || !vp->vlcPlayer) return;
    libvlc_media_player_pause(vp->vlcPlayer);
}

void VideoPlayer_Stop(VideoPlayer *vp)
{
    if (!vp || !vp->vlcPlayer) return;
    libvlc_media_player_stop(vp->vlcPlayer);
}

void VideoPlayer_Close(VideoPlayer *vp)
{
    if (!vp) return;

    if (vp->vlcPlayer)
    {
        libvlc_media_player_stop(vp->vlcPlayer);
        libvlc_media_player_release(vp->vlcPlayer);
        vp->vlcPlayer = NULL;
    }

    if (vp->vlcInst)
    {
        libvlc_release(vp->vlcInst);
        vp->vlcInst = NULL;
    }

    if (vp->texture.id)
    {
        UnloadTexture(vp->texture);
        vp->texture.id = 0;
    }

    if (vp->pixels)
    {
        ce_aligned_free(vp->pixels);
        vp->pixels = NULL;
    }

    if (vp->vlc_buffer)
    {
        ce_aligned_free(vp->vlc_buffer);
        vp->vlc_buffer = NULL;
    }
}

bool VideoPlayer_IsFinished(VideoPlayer *vp)
{
    if (!vp || !vp->vlcPlayer) return true;

    libvlc_state_t state =
        libvlc_media_player_get_state(vp->vlcPlayer);

    return (state == libvlc_Ended || state == libvlc_Error);
}

bool VideoPlayer_IsPlaying(VideoPlayer *vp)
{
    if (!vp || !vp->vlcPlayer) return false;

    return libvlc_media_player_get_state(vp->vlcPlayer)
           == libvlc_Playing;
}

float VideoPlayer_GetPosition(VideoPlayer *vp)
{
    if (!vp || !vp->vlcPlayer) return 0.0f;
    return libvlc_media_player_get_position(vp->vlcPlayer);
}

void VideoPlayer_SetPosition(VideoPlayer *vp, float position)
{
    if (!vp || !vp->vlcPlayer) return;
    libvlc_media_player_set_position(vp->vlcPlayer, position);
}
