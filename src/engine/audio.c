/*******************************************************************************
 * Silicon Legacy — Audio System (Implementation)
 * Generates simple tones procedurally — no external audio files needed
 ******************************************************************************/
#include "engine/audio.h"
#include <math.h>

#define AUDIO_FREQ     44100
#define AUDIO_SAMPLES  2048

static void audio_callback(void* userdata, Uint8* stream, int len) {
    AudioEngine* audio = (AudioEngine*)userdata;
    i16* out = (i16*)stream;
    int samples = len / 2;

    if (!audio->playing || !audio->tone_buffer) {
        memset(stream, 0, (size_t)len);
        return;
    }

    for (int i = 0; i < samples; i++) {
        if (audio->tone_pos < audio->tone_samples) {
            out[i] = (i16)(audio->tone_buffer[audio->tone_pos] * audio->master_volume);
            audio->tone_pos++;
        } else {
            out[i] = 0;
            audio->playing = false;
        }
    }
}

bool audio_init(AudioEngine* audio) {
    memset(audio, 0, sizeof(*audio));
    audio->master_volume = 0.3f;

    SDL_AudioSpec want = {0}, have = {0};
    want.freq = AUDIO_FREQ;
    want.format = AUDIO_S16SYS;
    want.channels = 1;
    want.samples = AUDIO_SAMPLES;
    want.callback = audio_callback;
    want.userdata = audio;

    audio->device = SDL_OpenAudioDevice(NULL, 0, &want, &have, 0);
    if (audio->device == 0) {
        fprintf(stderr, "[AUDIO] Failed to open: %s\n", SDL_GetError());
        return false;
    }

    /* Pre-allocate tone buffer (max 0.5 seconds) */
    audio->tone_samples = AUDIO_FREQ / 2;
    audio->tone_buffer = (i16*)malloc(sizeof(i16) * (size_t)audio->tone_samples);

    SDL_PauseAudioDevice(audio->device, 0);
    audio->initialized = true;
    printf("[AUDIO] Initialized\n");
    return true;
}

void audio_shutdown(AudioEngine* audio) {
    if (audio->device) SDL_CloseAudioDevice(audio->device);
    free(audio->tone_buffer);
    memset(audio, 0, sizeof(*audio));
}

/* Generate a simple tone into the buffer */
static void gen_tone(AudioEngine* audio, f32 freq, f32 duration, f32 decay) {
    int samples = (int)(AUDIO_FREQ * duration);
    if (samples > audio->tone_samples) samples = audio->tone_samples;

    for (int i = 0; i < samples; i++) {
        f32 t = (f32)i / AUDIO_FREQ;
        f32 envelope = 1.0f - (t / duration) * decay;
        if (envelope < 0) envelope = 0;
        f32 val = sinf(2.0f * 3.14159f * freq * t) * envelope * 16000.0f;
        audio->tone_buffer[i] = (i16)val;
    }
    audio->tone_pos = 0;
    audio->playing = true;
}

void audio_play_sfx(AudioEngine* audio, SoundEffect sfx) {
    if (!audio->initialized) return;

    SDL_LockAudioDevice(audio->device);
    switch (sfx) {
        case SFX_CLICK:      gen_tone(audio, 800.0f,  0.05f, 1.0f); break;
        case SFX_HOVER:      gen_tone(audio, 600.0f,  0.03f, 1.0f); break;
        case SFX_SUCCESS:    gen_tone(audio, 1200.0f, 0.15f, 0.8f); break;
        case SFX_ERROR:      gen_tone(audio, 200.0f,  0.2f,  0.5f); break;
        case SFX_CASH:       gen_tone(audio, 1000.0f, 0.1f,  0.9f); break;
        case SFX_RESEARCH:   gen_tone(audio, 440.0f,  0.3f,  0.7f); break;
        case SFX_BUILD:      gen_tone(audio, 300.0f,  0.2f,  0.6f); break;
        case SFX_ERA_CHANGE: gen_tone(audio, 880.0f,  0.5f,  0.4f); break;
        default: break;
    }
    SDL_UnlockAudioDevice(audio->device);
}

void audio_set_volume(AudioEngine* audio, f32 volume) {
    audio->master_volume = CLAMP(volume, 0.0f, 1.0f);
}
