#ifndef VIDEO_H
#define VIDEO_H

#include "raylib.h"
#include <vlc/vlc.h>

// In video.h, add vlc_buffer to your VideoPlayer struct:
typedef struct VideoPlayer {
    libvlc_instance_t *vlcInst;
    libvlc_media_player_t *vlcPlayer;
    unsigned char *pixels;      // For Raylib display
    unsigned char *vlc_buffer;  // For VLC writing - ADD THIS
    Texture2D texture;
    int width;
    int height;
} VideoPlayer;

VideoPlayer VideoPlayer_Init(const char *filePath, int width, int height);

void VideoPlayer_Play(VideoPlayer *vp);

void VideoPlayer_Update(VideoPlayer *vp);

void VideoPlayer_Close(VideoPlayer *vp);

bool VideoPlayer_IsFinished(VideoPlayer *vp);

#endif
