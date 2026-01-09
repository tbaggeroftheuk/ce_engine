#ifndef VIDEO_H
#define VIDEO_H

#include "raylib.h"
#include <vlc/vlc.h>

typedef struct {
    libvlc_instance_t *vlcInst;
    libvlc_media_player_t *vlcPlayer;
    unsigned char *pixels;
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
