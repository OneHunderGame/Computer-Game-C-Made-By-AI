#include "common.h"
#include "engine/memory.h"
#include "engine/renderer.h"
#include "engine/input.h"
#include "engine/audio.h"
#include "engine/ui.h"
#include "game/game_state.h"

int main(int argc, char** argv) {
    (void)argc; (void)argv;
    
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
        fprintf(stderr, "SDL_Init failed: %s\n", SDL_GetError());
        return 1;
    }

    Renderer renderer = {0};
    if (!renderer_init(&renderer, "Silicon Legacy: Hardware VM", WINDOW_W, WINDOW_H)) {
        return 1;
    }

    /* ESSENTIAL FOR KEYBOARD TEXT TO WORK */
    SDL_StartTextInput();

    InputState input = {0};
    AudioEngine audio = {0};
    audio_init(&audio);

    UIContext ui = {0};
    ui_init(&ui, &renderer, &input, &audio);

    GameState game = {0};
    game_state_init(&game);

    u64 last_time = SDL_GetPerformanceCounter();
    u64 freq = SDL_GetPerformanceFrequency();
    double sim_accumulator = 0.0;
    double sim_dt = 1.0 / SIM_TICK_RATE;

    while (!input.quit_requested && !game.should_quit) {
        u64 now = SDL_GetPerformanceCounter();
        double dt = (double)(now - last_time) / freq;
        last_time = now;
        
        if (dt > 0.25) dt = 0.25;

        input_begin_frame(&input);
        
        bool enter_pressed = false;
        
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            input_process_event(&input, &e);
            if (e.type == SDL_KEYDOWN) {
                if (e.key.keysym.sym == SDLK_RETURN) {
                    enter_pressed = true;
                }
                if (e.key.keysym.sym == SDLK_BACKSPACE) {
                    /* Allow backspace deletion as a rough text simulation */
                    game_handle_text_input(&game, "\b", false);
                }
                /* Forward keys like F3 directly into the game state */
                game_handle_key_down(&game, e.key.keysym.sym);
            }
            if (e.type == SDL_TEXTINPUT) {
                /* Properly capture OS-level text characters */
                game_handle_text_input(&game, e.text.text, false);
            }
        }

        /* Forward generated "Enter" cleanly */
        if (enter_pressed) {
            game_handle_text_input(&game, NULL, true);
        }

        sim_accumulator += dt;
        while (sim_accumulator >= sim_dt) {
            game_simulate_tick(&game, sim_dt);
            sim_accumulator -= sim_dt;
        }

        renderer_begin_frame(&renderer);
        ui_begin_frame(&ui, (f32)dt);
        
        game_render(&game, &renderer, &ui);
        
        ui_end_frame(&ui);
        renderer_end_frame(&renderer);
        
        SDL_Delay(1);
    }

    SDL_StopTextInput();
    audio_shutdown(&audio);
    renderer_shutdown(&renderer);
    SDL_Quit();

    return 0;
}
