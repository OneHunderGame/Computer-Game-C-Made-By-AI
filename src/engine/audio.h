/*******************************************************************************
 * Silicon Legacy — Audio System
 * Simple tone generation and SFX via SDL2 audio
 ******************************************************************************/
#ifndef ENGINE_AUDIO_H
#define ENGINE_AUDIO_H

#include "common.h"
#include <SDL.h>

typedef enum {
    SFX_CLICK = 0,
    SFX_HOVER,
    SFX_SUCCESS,
    SFX_ERROR,
    SFX_CASH,
    SFX_RESEARCH,
    SFX_BUILD,
    SFX_ERA_CHANGE,
    SFX_COUNT
} SoundEffect;

typedef struct {
    SDL_AudioDeviceID device;
    bool              initialized;
    f32               master_volume;
    /* Generated tone buffer */
    i16*              tone_buffer;
    int               tone_samples;
    int               tone_pos;
    bool              playing;
} AudioEngine;

bool audio_init(AudioEngine* audio);
void audio_shutdown(AudioEngine* audio);
void audio_play_sfx(AudioEngine* audio, SoundEffect sfx);
void audio_set_volume(AudioEngine* audio, f32 volume);

#endif /* ENGINE_AUDIO_H */
