#include "game/game_state.h"
#include <stdio.h>
#include <math.h>

#define C_RGB(r,g,b) ((Color){(r), (g), (b), 255})

void game_state_init(GameState* state) {
    memset(state, 0, sizeof(*state));
    state->current_screen = SCREEN_TITLE;
    state->current_era = ERA_1960;
    
    state->current_year = 1960;
    state->current_month = 1;
    state->current_day = 1;
    state->simulated_days = 0;
    state->research_points = 50;

    hardware_init();
}

void game_simulate_tick(GameState* state, double dt) {
    state->simulated_days += dt;
    if (state->simulated_days >= 1.0) {
        state->simulated_days -= 1.0;
        state->current_day++;
        
        if(state->current_day % 5 == 0) state->research_points++;

        if (state->current_day > 30) {
            state->current_day = 1;
            state->current_month++;
            if (state->current_month > 12) {
                state->current_month = 1;
                state->current_year++;
            }
        }
    }

    if (state->current_design.is_powered_on) {
        vm_tick(&state->vm, dt);
    }
}

void game_handle_text_input(GameState* state, const char* text, bool enter_pressed) {
    if (state->current_screen == SCREEN_WORKBENCH && state->current_design.is_powered_on) {
        vm_handle_input(&state->vm, text, enter_pressed);
    }
}

void game_handle_key_down(GameState* state, SDL_Keycode key) {
    if (state->current_screen == SCREEN_WORKBENCH && state->current_design.is_powered_on) {
        if (key == SDLK_F3) {
            vm_intercept_f3(&state->vm);
        }
    }
    
    if (key == SDLK_ESCAPE && state->show_catalog_modal) {
        state->show_catalog_modal = false;
    }
}

static void render_hud(GameState* s, UIContext* ui) {
    Rect hud_rect = {0, 0, (f32)ui->renderer->screen_w, HUD_HEIGHT};
    draw_rect(ui->renderer, hud_rect, ui->theme.header_bg);
    
    char date_buf[128];
    snprintf(date_buf, sizeof(date_buf), "%02d / %02d / %4d  |  RP: %d", 
             s->current_day, s->current_month, s->current_year, s->research_points);
    draw_text(ui->renderer, date_buf, 10, 10, ui->theme.header_text, 2);
    
    if(ui_button(ui, UI_ID(1), "Workbench", RECT(250, 5, 120, 30))) s->current_screen = SCREEN_WORKBENCH;
    if(ui_button(ui, UI_ID(2), "Tech Tree", RECT(380, 5, 120, 30))) {
        s->current_screen = SCREEN_TECH_TREE;
        s->show_catalog_modal = false;
    }
}

static void draw_visual_part(Renderer* r, Rect area, ComponentType type, bool equipped) {
    Color outline = C_RGB(80, 80, 80);
    Color fill    = C_RGB(30, 30, 30);
    
    if (!equipped) {
        draw_rect(r, area, fill);
        draw_rect_outline(r, area, outline, 1);
        draw_text_centered(r, "EMPTY", area, outline, 2);
        return;
    }

    switch(type) {
        case COMP_CPU:
            draw_rect(r, area, C_RGB(40, 40, 50)); 
            draw_rect_outline(r, area, C_RGB(200, 200, 200), 2); 
            for(f32 p = area.x; p < area.x + area.w; p+=8) {
                draw_line(r, p, area.y - 4, p, area.y, C_RGB(180, 150, 50)); 
                draw_line(r, p, area.y + area.h, p, area.y + area.h + 4, C_RGB(180, 150, 50));
            }
            break;
        case COMP_RAM:
            draw_rect(r, area, C_RGB(20, 80, 20)); 
            for(int i=0; i<4; i++) {
                draw_rect(r, RECT(area.x + 8 + i*16, area.y + 4, 12, area.h - 8), C_RGB(20, 20, 20)); 
            }
            break;
        case COMP_STORAGE:
            draw_rect(r, area, C_RGB(50, 50, 50));
            draw_rect_rounded(r, RECT(area.x + area.w/2 - 15, area.y + area.h/2 - 15, 30, 30), C_RGB(100, 100, 100), 15); 
            break;
        case COMP_PSU:
            draw_rect(r, area, C_RGB(100, 20, 20)); 
            draw_rect_outline(r, area, C_RGB(255, 100, 100), 2);
            for(f32 yy = area.y + 10; yy < area.y + area.h - 10; yy += 6) {
                draw_line(r, area.x + 10, yy, area.x + area.w - 10, yy, C_RGB(50, 0, 0));
            }
            break;
        default:
            draw_rect(r, area, C_RGB(100, 100, 150));
            draw_rect_outline(r, area, C_RGB(200, 200, 255), 1);
            break;
    }
}

static void render_catalog_modal(GameState* s, UIContext* ui, Rect area) {
    draw_rect(ui->renderer, RECT(0, 0, (f32)ui->renderer->screen_w, (f32)ui->renderer->screen_h), color_alpha(COL_BLACK, 200));
    
    ui_panel_flat(ui, area, ui->theme.bg_secondary);
    draw_rect_outline(ui->renderer, area, ui->theme.border, 2);
    
    char title[128];
    snprintf(title, sizeof(title), "PARTS CATALOG: %s", component_type_name(s->catalog_filter));
    draw_text(ui->renderer, title, area.x + 15, area.y + 15, ui->theme.accent, 3);
    
    if (ui_button(ui, UI_ID(900), "X Close", RECT(area.x + area.w - 120, area.y + 10, 100, 30))) {
        s->show_catalog_modal = false;
    }
    
    ComponentDef list[30];
    int count = hardware_get_components(s->current_year, s->catalog_filter, list, 30);
    
    f32 cx = area.x + 20;
    f32 cy = area.y + 70;
    f32 slot_w = 200;
    f32 slot_h = 100;
    
    for (int i = 0; i < count; i++) {
        Rect s_rect = RECT(cx, cy, slot_w, slot_h);
        
        ui_panel_flat(ui, s_rect, ui->theme.bg_tertiary);
        draw_rect_outline(ui->renderer, s_rect, ui->theme.border, 1);
        
        Rect thumb = RECT(cx + 5, cy + 5, 40, 40);
        draw_visual_part(ui->renderer, thumb, s->catalog_filter, true);
        
        draw_text(ui->renderer, list[i].name, cx + 55, cy + 10, ui->theme.text_primary, 1);
        
        char stat_buf[64];
        if (s->catalog_filter == COMP_CPU) snprintf(stat_buf, sizeof(stat_buf), "%u Hz | %.0f W", list[i].clock_hz, list[i].power_draw);
        else if (s->catalog_filter == COMP_PSU) snprintf(stat_buf, sizeof(stat_buf), "+%.0f W Output", list[i].power_supply);
        else if (s->catalog_filter == COMP_RAM) snprintf(stat_buf, sizeof(stat_buf), "%u B | %.0f W", list[i].memory_bytes, list[i].power_draw);
        else snprintf(stat_buf, sizeof(stat_buf), "Power: %.0f W", list[i].power_draw);
        
        draw_text(ui->renderer, stat_buf, cx + 55, cy + 25, ui->theme.success, 1);
        
        if (ui_button(ui, UI_ID(1000 + i), "EQUIP", RECT(cx + 55, cy + 60, 100, 25))) {
            s->selected_parts[s->catalog_filter] = list[i];
            s->show_catalog_modal = false;
            if(ui->audio) audio_play_sfx(ui->audio, SFX_BUILD);
        }
        
        cx += slot_w + 15;
        if (cx + slot_w > area.x + area.w - 20) {
            cx = area.x + 20;
            cy += slot_h + 15;
        }
    }
}

struct SlotDef { ComponentType type; Rect rect; const char* label; };

static void render_workbench(GameState* s, UIContext* ui, Rect area) {
    ui_panel_flat(ui, area, ui->theme.bg_secondary);
    draw_text(ui->renderer, "GRAPHICAL HARDWARE BENCH", area.x + 10, area.y + 10, ui->theme.text_accent, 2);
    draw_line(ui->renderer, area.x, area.y + 35, area.x + area.w, area.y + 35, ui->theme.border);
    
    Rect mobo_rect = RECT(area.x + 20, area.y + 60, area.w - 40, area.h - 160);
    draw_rect(ui->renderer, mobo_rect, C_RGB(10, 40, 15)); 
    draw_rect_outline(ui->renderer, mobo_rect, C_RGB(50, 150, 50), 2);
    
    for(int i=0; i<5; i++) {
        draw_line(ui->renderer, mobo_rect.x + 50, mobo_rect.y + 50 + i*20, mobo_rect.x + 150, mobo_rect.y + 50 + i*20, C_RGB(20, 80, 20));
    }
    
    struct SlotDef slots[] = {
        { COMP_CPU, RECT(mobo_rect.x + 40, mobo_rect.y + 30, 80, 80), "CPU Socket" },
        { COMP_RAM, RECT(mobo_rect.x + 150, mobo_rect.y + 30, 100, 30), "RAM Slot 1" },
        { COMP_STORAGE, RECT(mobo_rect.x + mobo_rect.w - 120, mobo_rect.y + 30, 90, 90), "Data Drive" },
        { COMP_DISPLAY, RECT(mobo_rect.x + 40, mobo_rect.y + 140, 100, 50), "Video Out" },
        { COMP_PSU, RECT(mobo_rect.x + mobo_rect.w - 120, mobo_rect.y + mobo_rect.h - 80, 100, 60), "Power In" }
    };
    
    bool all_selected = true;
    for(size_t i = 0; i < sizeof(slots)/sizeof(slots[0]); i++) {
        bool is_equipped = (s->selected_parts[slots[i].type].id != 0);
        if(!is_equipped) all_selected = false;
        
        draw_visual_part(ui->renderer, slots[i].rect, slots[i].type, is_equipped);
        draw_text(ui->renderer, slots[i].label, slots[i].rect.x, slots[i].rect.y - 12, ui->theme.text_secondary, 1);
        
        if (is_equipped) {
            draw_text(ui->renderer, s->selected_parts[slots[i].type].name, slots[i].rect.x, slots[i].rect.y + slots[i].rect.h + 2, ui->theme.success, 1);
        }

        if (!s->current_design.is_powered_on) {
            if (ui_button(ui, UI_ID(300+(int)i), is_equipped?"CHANGE":"SELECT", RECT(slots[i].rect.x + slots[i].rect.w/2 - 25, slots[i].rect.y + slots[i].rect.h/2 - 10, 60, 20))) {
                s->catalog_filter = slots[i].type;
                s->show_catalog_modal = true;
            }
        }
    }
    
    f32 actions_y = mobo_rect.y + mobo_rect.h + 20;
    
    if(all_selected) {
        if (!s->current_design.is_assembled) {
            if(ui_button(ui, UI_ID(50), "ASSEMBLE SYSTEM", RECT(area.x + 20, actions_y, 200, 50))) {
                computer_assemble(&s->current_design, "Testing Rig Alpha", s->selected_parts);
                if(ui->audio) audio_play_sfx(ui->audio, SFX_BUILD);
            }
        } 
        else {
            Color led_color = s->current_design.is_powered_on ? ui->theme.danger : C_RGB(50, 0, 0);
            draw_rect_rounded(ui->renderer, RECT(area.x + 20, actions_y + 10, 30, 30), led_color, 15);
            
            char* power_lbl = s->current_design.is_powered_on ? "POWER: SHUTDOWN" : "POWER: BOOT";
            if(ui_button(ui, UI_ID(51), power_lbl, RECT(area.x + 70, actions_y, 180, 50))) {
                computer_power_toggle(&s->current_design);
                if(s->current_design.is_powered_on) {
                    vm_init(&s->vm, &s->current_design); 
                    if(ui->audio) audio_play_sfx(ui->audio, SFX_SUCCESS);
                } else {
                    if(ui->audio) audio_play_sfx(ui->audio, SFX_CLICK);
                    s->current_design.is_assembled = false; 
                }
            }
        }
    }
}

static void render_bios_gui(GameState* s, UIContext* ui, Rect area) {
    Color bios_bg = C_RGB(0, 0, 170);      
    Color bios_fg = C_RGB(170, 170, 170);  
    Color bios_highlight = C_RGB(255, 255, 255);
    Color bios_red = C_RGB(170, 0, 0);     
    
    ui_panel_flat(ui, area, bios_bg);
    draw_rect_outline(ui->renderer, area, bios_fg, 4);
    
    Rect top_bar = RECT(area.x, area.y, area.w, 40);
    draw_rect(ui->renderer, top_bar, bios_red);
    draw_text_centered(ui->renderer, "THEO BIOS SETUP UTILITY V1.0", top_bar, bios_highlight, 3);
    
    f32 cx = area.x + 20;
    f32 cy = area.y + 60;
    int s_scale = 3;
    
    draw_text(ui->renderer, "Standard CMOS Features", cx, cy, bios_fg, s_scale); cy += 30;
    draw_text(ui->renderer, "Advanced BIOS Features", cx, cy, bios_fg, s_scale); cy += 30;
    draw_text(ui->renderer, "Integrated Peripherals", cx, cy, bios_fg, s_scale); cy += 60;
    
    char buf[128];
    draw_text(ui->renderer, "System Information:", cx, cy, bios_highlight, s_scale); cy += 25;
    snprintf(buf, sizeof(buf), "Processor:  %s", s->current_design.parts[COMP_CPU].name);
    draw_text(ui->renderer, buf, cx, cy, bios_fg, s_scale); cy += 25;
    snprintf(buf, sizeof(buf), "Speed:      %u Hz", s->current_design.sys_clock_hz);
    draw_text(ui->renderer, buf, cx, cy, bios_fg, s_scale); cy += 25;
    snprintf(buf, sizeof(buf), "Base Mem:   %u B", s->current_design.sys_memory_bytes);
    draw_text(ui->renderer, buf, cx, cy, bios_fg, s_scale); cy += 50;

    if (ui_button(ui, UI_ID(999), "Save & Exit Setup", RECT(cx, cy, 300, 40))) {
        vm_exit_bios(&s->vm);
        if(ui->audio) audio_play_sfx(ui->audio, SFX_CLICK);
    }
}

static void render_virtual_monitor(GameState* s, UIContext* ui, Rect area) {
    if (s->vm.mode == VM_BIOS_MENU) { render_bios_gui(s, ui, area); return; }

    ui_panel_flat(ui, area, COL_BLACK);
    draw_rect_outline(ui->renderer, area, ui->theme.border, 2);
    draw_text(ui->renderer, "VIRTUAL MONITOR (tty0)", area.x + 10, area.y - 20, ui->theme.text_secondary, 2);
    
    if (s->current_design.is_crashed) {
        draw_text_centered(ui->renderer, "CRITICAL POWER FAILURE.\nSYSTEM HALTED.", area, ui->theme.danger, 3);
        return;
    }
    if (!s->current_design.is_powered_on) {
        draw_text_centered(ui->renderer, "NO SIGNAL", area, ui->theme.text_secondary, 3);
        return;
    }
    
    VirtualMonitor* m = &s->vm.monitor;
    f32 cx = area.x + 10;
    f32 cy = area.y + 10;
    for (int row = 0; row < TERM_ROWS; ++row) {
        char lineBuf[TERM_COLS + 1];
        strncpy(lineBuf, m->chars[row], TERM_COLS);
        lineBuf[TERM_COLS] = '\0';
        for(int x=0; x<TERM_COLS; x++) {
            if (lineBuf[x] == '\b') lineBuf[x] = ' ';
        }
        draw_text(ui->renderer, lineBuf, cx, cy + (row * text_height(3)), ui->theme.success, 3);
    }
    
    if (s->vm.mode == VM_OS && fmodf((f32)s->simulated_days * 1.5f, 1.0f) < 0.5f) {
        f32 cursor_px = cx + (m->cursor_x * 15);
        f32 cursor_py = cy + (m->cursor_y * 21);
        draw_rect(ui->renderer, RECT(cursor_px, cursor_py, 15, 21), ui->theme.success);
    }
}

static void render_diagnostics(GameState* s, UIContext* ui, Rect area) {
    ui_panel_flat(ui, area, ui->theme.bg_secondary);
    draw_text(ui->renderer, "QUADRANT 3: LIVE DIAGNOSTICS", area.x + 10, area.y + 10, ui->theme.text_accent, 2);
    draw_line(ui->renderer, area.x, area.y + 35, area.x + area.w, area.y + 35, ui->theme.border);
    
    if (!s->current_design.is_powered_on) return;
    
    f32 y = area.y + 50;
    char buf[128];
    
    snprintf(buf, sizeof(buf), "PWR DRAW: %.0fW / %.0fW", s->current_design.total_power_draw, s->current_design.max_power_supply);
    ui_stat_bar(ui, buf, s->current_design.total_power_draw / s->current_design.max_power_supply, RECT(area.x + 10, y, area.w - 20, 20));
    y += 35;
    
    snprintf(buf, sizeof(buf), "CPU LOAD:");
    ui_stat_bar(ui, buf, s->vm.cpu.current_load, RECT(area.x + 10, y, area.w - 20, 20));
    y += 35;

    snprintf(buf, sizeof(buf), "CLOCK:  %u Hz", s->current_design.sys_clock_hz);
    ui_label(ui, buf, area.x + 10, y, ui->theme.text_primary);
    y += 25;
    
    snprintf(buf, sizeof(buf), "CYCLES: %llu", s->vm.cpu.total_cycles);
    ui_label(ui, buf, area.x + 10, y, ui->theme.text_primary);
}

static void render_tech_tree(GameState* s, UIContext* ui) {
    render_hud(s, ui);
    
    f32 w = (f32)ui->renderer->screen_w;
    f32 h = (f32)ui->renderer->screen_h;
    
    ui_panel_flat(ui, RECT(50, 80, w - 100, h - 130), ui->theme.bg_secondary);
    draw_text(ui->renderer, "ENGINEERING RESEARCH & TECH TREE", 70, 100, ui->theme.accent, 4);
    
    char buf[128];
    snprintf(buf, sizeof(buf), "Available Research Points (RP): %d", s->research_points);
    draw_text(ui->renderer, buf, 70, 150, ui->theme.success, 2);
    
    struct { Rect rect; const char* text; int cost; bool unlocked; } nodes[] = {
        { RECT(100, 250, 200, 80), "Transistor Theory", 0, true },
        { RECT(100, 400, 200, 80), "Magnetic Storage", 100, false },
        { RECT(400, 250, 200, 80), "Silicon Wafers", 500, false },
        { RECT(400, 400, 200, 80), "Microprocessors", 2000, false },
    };
    
    for (int i=0; i<4; i++) {
        Color border = nodes[i].unlocked ? ui->theme.success : ui->theme.border;
        draw_rect_outline(ui->renderer, nodes[i].rect, border, 2);
        draw_text_centered(ui->renderer, nodes[i].text, nodes[i].rect, ui->theme.text_primary, 2);
        
        snprintf(buf, sizeof(buf), "Cost: %d RP", nodes[i].cost);
        draw_text(ui->renderer, buf, nodes[i].rect.x + 10, nodes[i].rect.y + nodes[i].rect.h - 20, ui->theme.text_secondary, 1);
        
        if (!nodes[i].unlocked) {
            if (ui_button(ui, UI_ID(4000+i), "RESEARCH", RECT(nodes[i].rect.x + 50, nodes[i].rect.y + nodes[i].rect.h + 5, 100, 30))) {
                if (s->research_points >= nodes[i].cost) {
                    s->research_points -= nodes[i].cost;
                    if(ui->audio) audio_play_sfx(ui->audio, SFX_RESEARCH);
                } else {
                    if(ui->audio) audio_play_sfx(ui->audio, SFX_ERROR);
                }
            }
        }
    }
    
    draw_line(ui->renderer, 300, 290, 400, 290, ui->theme.border);
    draw_line(ui->renderer, 200, 330, 200, 400, ui->theme.border);
    draw_line(ui->renderer, 300, 440, 400, 440, ui->theme.border);
}

void game_render(GameState* s, Renderer* r, UIContext* ui) {
    f32 w = (f32)r->screen_w;
    f32 h = (f32)r->screen_h;
    
    if (s->current_screen == SCREEN_TITLE) {
        ui_panel_flat(ui, RECT(0, 0, w, h), ui->theme.bg_primary);
        draw_text_centered(r, "SILICON LEGACY", RECT(0, 50, w, 100), ui->theme.accent, 8);
        draw_text_centered(r, "HARDWARE ENGINEERING TERMINAL", RECT(0, 150, w, 40), ui->theme.text_secondary, 3);
        
        f32 btn_w = 400; f32 btn_cx = (w - btn_w) / 2.0f; f32 btn_cy = 300;
        if (ui_button(ui, UI_ID(10), "New Game (Sandbox)", RECT(btn_cx, btn_cy, btn_w, 60))) {
            s->current_screen = SCREEN_WORKBENCH;
            if(ui->audio) audio_play_sfx(ui->audio, SFX_SUCCESS);
        }
        btn_cy += 80;
        if (ui_button(ui, UI_ID(11), "Continue Game (Disabled)", RECT(btn_cx, btn_cy, btn_w, 60))) {
            if(ui->audio) audio_play_sfx(ui->audio, SFX_ERROR);
        }
        btn_cy += 80;
        if (ui_button(ui, UI_ID(12), "Settings", RECT(btn_cx, btn_cy, btn_w, 60))) {
            if(ui->audio) audio_play_sfx(ui->audio, SFX_CLICK);
        }
        btn_cy += 80;
        if (ui_button(ui, UI_ID(13), "Quit to Desktop", RECT(btn_cx, btn_cy, btn_w, 60))) {
            s->should_quit = true;
        }
        return;
    }
    
    if (s->current_screen == SCREEN_TECH_TREE) {
        render_tech_tree(s, ui);
        return;
    }

    render_hud(s, ui);
    
    f32 padding = 10;
    f32 mid_x = w * 0.40f; 
    f32 mid_y = h * 0.65f;
    f32 header_off = HUD_HEIGHT + padding;
    
    Rect q1 = RECT(padding, header_off, mid_x - padding * 1.5f, mid_y - padding * 1.5f);
    Rect q2 = RECT(mid_x + padding * 0.5f, header_off + 25.0f, w - mid_x - padding * 1.5f, mid_y - padding * 1.5f);
    Rect q3 = RECT(padding, header_off + mid_y + padding * 0.5f, mid_x - padding * 1.5f, h - mid_y - header_off - padding * 1.5f);
    
    render_workbench(s, ui, q1);
    render_virtual_monitor(s, ui, q2);
    render_diagnostics(s, ui, q3);
    
    Rect q4 = RECT(mid_x + padding * 0.5f, header_off + mid_y + padding * 0.5f, w - mid_x - padding * 1.5f, h - mid_y - header_off - padding * 1.5f);
    ui_panel_flat(ui, q4, ui->theme.bg_tertiary);
    draw_text(ui->renderer, "Available Terminal Commands: 'sysinfo', 'benchmark', 'clear'", q4.x + 10, q4.y + 10, ui->theme.text_secondary, 2);
    draw_text(ui->renderer, "INTERRUPT: Press 'F3' during POST sequence to enter Theo Bios Setup.", q4.x + 10, q4.y + 35, ui->theme.warning, 2);
    
    if (s->show_catalog_modal) {
        render_catalog_modal(s, ui, RECT(50, 50, w - 100, h - 100));
    }
}
