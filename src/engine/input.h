/*******************************************************************************
 * Silicon Legacy — Input System
 ******************************************************************************/
#ifndef ENGINE_INPUT_H
#define ENGINE_INPUT_H

#include "common.h"
#include <SDL.h>

#define MAX_KEYS 512

struct InputState {
    /* Mouse */
    f32  mouse_x, mouse_y;
    f32  mouse_dx, mouse_dy;          /* delta since last frame */
    bool mouse_down[3];               /* 0=left, 1=middle, 2=right */
    bool mouse_pressed[3];            /* just pressed this frame */
    bool mouse_released[3];           /* just released this frame */
    f32  mouse_scroll;                /* wheel delta */

    /* Keyboard */
    bool key_down[MAX_KEYS];
    bool key_pressed[MAX_KEYS];
    bool key_released[MAX_KEYS];

    /* Text input */
    char text_input[32];              /* text typed this frame */
    bool has_text_input;

    /* Window */
    bool quit_requested;
    bool window_resized;
    int  window_w, window_h;
};

void input_begin_frame(InputState* input);
void input_process_event(InputState* input, const SDL_Event* event);

/* Convenience */
static inline bool mouse_in_rect(const InputState* input, Rect r) {
    return rect_contains(r, input->mouse_x, input->mouse_y);
}

#endif /* ENGINE_INPUT_H */
