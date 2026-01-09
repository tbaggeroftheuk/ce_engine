#include "raylib.h"
#include <vlc/vlc.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>  // for usleep
#include "globals.h"
#include "engine/modules/vid_player/video.h"

// VLC callbacks
static void *lock(void *opaque, void **planes) {
    VideoPlayer *vp = (VideoPlayer *)opaque;
    *planes = vp->pixels;
    return NULL;
}

static void unlock(void *opaque, void *picture, void *const *planes) {
    (void)opaque;
    (void)picture;
    (void)planes;
}

static void display(void *opaque, void *picture) {
    (void)opaque;
    (void)picture;
}

// Initialize a VideoPlayer for a given file
VideoPlayer VideoPlayer_Init(const char *filePath, int width, int height) {
    VideoPlayer vp = {0};
    vp.width = width;
    vp.height = height;

    // Allocate pixel buffer
    vp.pixels = (unsigned char *)malloc(width * height * 4); // RGBA32
    memset(vp.pixels, 0, width * height * 4);

    // VLC arguments
    const char *vlc_args[] = {
        "--no-xlib",
        "--verbose=2"  // safe to include even if debug is false
    };

    vp.vlcInst = libvlc_new(sizeof(vlc_args) / sizeof(vlc_args[0]), vlc_args);
    if (!vp.vlcInst) {
        TraceLog(LOG_ERROR, "VideoPlayer: Failed to create VLC instance");
        free(vp.pixels);
        return vp;
    }

    libvlc_media_t *media = libvlc_media_new_path(vp.vlcInst, filePath);
    if (!media) {
        TraceLog(LOG_ERROR, "VideoPlayer: Failed to load media: %s", filePath);
        libvlc_release(vp.vlcInst);
        free(vp.pixels);
        return vp;
    }

    // Parse media to get track info (using new API)
    if (ce_globals.debug) {
        // Use the new parsing function with options
        libvlc_media_parse_with_options(media, libvlc_media_parse_local, -1);
        
        // Wait for parsing to complete (with timeout)
        int timeout = 5000; // 5 seconds
        int elapsed = 0;
        while (elapsed < timeout) {
            libvlc_media_parsed_status_t status = libvlc_media_get_parsed_status(media);
            if (status == libvlc_media_parsed_status_done) {
                break;
            } else if (status == libvlc_media_parsed_status_failed) {
                TraceLog(LOG_WARNING, "VideoPlayer: Media parsing failed");
                break;
            }
            usleep(10000); // Sleep for 10ms
            elapsed += 10;
        }

        // Get tracks using the new API
        libvlc_media_track_t **tracks = NULL;
        unsigned int track_count = libvlc_media_tracks_get(media, &tracks);
        
        TraceLog(LOG_INFO, "VideoPlayer: Media tracks found: %u", track_count);
        
        for (unsigned int i = 0; i < track_count; ++i) {
            if (tracks[i]->i_type == libvlc_track_video) {
                TraceLog(LOG_INFO, "Video track %u: codec=%u, %ux%u",
                         i, tracks[i]->i_codec,
                         tracks[i]->video->i_width,
                         tracks[i]->video->i_height);
            }
        }
        
        libvlc_media_tracks_release(tracks, track_count);
    }

    vp.vlcPlayer = libvlc_media_player_new_from_media(media);
    libvlc_media_release(media);

    // Set video callbacks
    libvlc_video_set_callbacks(vp.vlcPlayer, lock, unlock, display, &vp);
    libvlc_video_set_format(vp.vlcPlayer, "RGBA", width, height, width * 4);

    // Create raylib texture
    Image img = {
        .data = vp.pixels,      // Changed from vp->pixels to vp.pixels
        .width = width,
        .height = height,
        .mipmaps = 1,
        .format = PIXELFORMAT_UNCOMPRESSED_R8G8B8A8
    };
    vp.texture = LoadTextureFromImage(img);

    return vp;
}

// Play video frame (call every frame)
void VideoPlayer_Update(VideoPlayer *vp) {
    UpdateTexture(vp->texture, vp->pixels);
}

// Start playback
void VideoPlayer_Play(VideoPlayer *vp) {
    libvlc_media_player_play(vp->vlcPlayer);
}

// Stop and free resources
void VideoPlayer_Close(VideoPlayer *vp) {
    libvlc_media_player_stop(vp->vlcPlayer);
    libvlc_media_player_release(vp->vlcPlayer);
    libvlc_release(vp->vlcInst);
    UnloadTexture(vp->texture);
    free(vp->pixels);
}

void VideoPlayer_startup_vid(void) {
    // placeholder
}

bool VideoPlayer_IsFinished(VideoPlayer *vp) {
    libvlc_state_t state = libvlc_media_player_get_state(vp->vlcPlayer);
    return state == libvlc_Ended;
}