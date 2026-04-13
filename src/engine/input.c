/*******************************************************************************
 * Silicon Legacy — Input System (Implementation)
 ******************************************************************************/
#include "engine/input.h"

void input_begin_frame(InputState* input) {
    /* Clear per-frame states */
    memset(input->mouse_pressed,  0, sizeof(input->mouse_pressed));
    memset(input->mouse_released, 0, sizeof(input->mouse_released));
    memset(input->key_pressed,    0, sizeof(input->key_pressed));
    memset(input->key_released,   0, sizeof(input->key_released));
    input->mouse_dx = 0;
    input->mouse_dy = 0;
    input->mouse_scroll = 0;
    input->has_text_input = false;
    input->text_input[0] = '\0';
    input->window_resized = false;
}

void input_process_event(InputState* input, const SDL_Event* event) {
    switch (event->type) {
    case SDL_QUIT:
        input->quit_requested = true;
        break;

    case SDL_WINDOWEVENT:
        if (event->window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
            input->window_w = event->window.data1;
            input->window_h = event->window.data2;
            input->window_resized = true;
        }
        break;

    case SDL_MOUSEMOTION:
        input->mouse_dx = (f32)event->motion.xrel;
        input->mouse_dy = (f32)event->motion.yrel;
        input->mouse_x  = (f32)event->motion.x;
        input->mouse_y  = (f32)event->motion.y;
        break;

    case SDL_MOUSEBUTTONDOWN:
        if (event->button.button <= 3) {
            int idx = event->button.button - 1;
            input->mouse_down[idx] = true;
            input->mouse_pressed[idx] = true;
        }
        break;

    case SDL_MOUSEBUTTONUP:
        if (event->button.button <= 3) {
            int idx = event->button.button - 1;
            input->mouse_down[idx] = false;
            input->mouse_released[idx] = true;
        }
        break;

    case SDL_MOUSEWHEEL:
        input->mouse_scroll = (f32)event->wheel.y;
        break;

    case SDL_KEYDOWN:
        if (event->key.keysym.scancode < MAX_KEYS) {
            if (!event->key.repeat) {
                input->key_pressed[event->key.keysym.scancode] = true;
            }
            input->key_down[event->key.keysym.scancode] = true;
        }
        break;

    case SDL_KEYUP:
        if (event->key.keysym.scancode < MAX_KEYS) {
            input->key_down[event->key.keysym.scancode] = false;
            input->key_released[event->key.keysym.scancode] = true;
        }
        break;

    case SDL_TEXTINPUT:
        strncpy(input->text_input, event->text.text, sizeof(input->text_input) - 1);
        input->has_text_input = true;
        break;
    }
}
