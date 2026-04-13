#ifndef GAME_STATE_H
#define GAME_STATE_H
#include "common.h"
#include "engine/ui.h"
#include "game/hardware.h"
#include "game/vm.h"

struct GameState {
    bool should_quit;
    ScreenID current_screen;
    Era current_era;
    
    /* Global Simulation Time */
    double simulated_days;
    int current_year;
    int current_month;
    int current_day;
    int research_points;

    /* Hardware under test */
    Computer current_design;
    ComponentDef selected_parts[COMP_TYPE_COUNT];
    
    /* Visual UI States */
    bool show_catalog_modal;
    ComponentType catalog_filter;
    
    /* The active Virtual Machine */
    VmState vm;
};

void game_state_init(GameState* state);
void game_simulate_tick(GameState* state, double dt);
void game_render(GameState* state, Renderer* r, UIContext* ui);

/* Input overrides from OS Keyboard */
void game_handle_text_input(GameState* state, const char* text, bool enter_pressed);
void game_handle_key_down(GameState* state, SDL_Keycode key);

#endif
