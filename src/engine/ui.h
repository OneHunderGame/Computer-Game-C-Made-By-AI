/*******************************************************************************
 * Silicon Legacy — UI System
 * Immediate-mode UI with era-themed styling
 ******************************************************************************/
#ifndef ENGINE_UI_H
#define ENGINE_UI_H

#include "common.h"
#include "engine/renderer.h"
#include "engine/input.h"
#include "engine/audio.h"

/* ── Era Theme ───────────────────────────────────────────────────────────── */
typedef struct {
    Color bg_primary;        /* main background */
    Color bg_secondary;      /* panel background */
    Color bg_tertiary;       /* card/item background */
    Color accent;            /* primary accent */
    Color accent_hover;      /* accent on hover */
    Color text_primary;      /* main text */
    Color text_secondary;    /* subdued text */
    Color text_accent;       /* highlighted text */
    Color success;           /* green */
    Color warning;           /* amber/yellow */
    Color danger;            /* red */
    Color border;            /* panel outlines */
    Color button_bg;         /* button fill */
    Color button_hover;      /* button hover fill */
    Color button_active;     /* button pressed */
    Color header_bg;         /* header/topbar */
    Color header_text;       /* header text */
    int   corner_radius;     /* rounded corner size */
    int   text_scale;        /* default text size */
    const char* era_label;   /* "1960s", "1970s", etc. */
} UITheme;

/* ── UI Context ──────────────────────────────────────────────────────────── */
struct UIContext {
    Renderer*   renderer;
    InputState* input;
    AudioEngine* audio;
    UITheme     theme;
    Era         current_era;

    /* Hot/active widget tracking */
    u32  hot_id;
    u32  active_id;

    /* Tooltip */
    char tooltip_text[256];
    bool has_tooltip;
    f32  tooltip_x, tooltip_y;

    /* Animation timer */
    f32  time;
    f32  dt;

    /* Layout helpers */
    f32  cursor_x, cursor_y;
    f32  panel_x, panel_y, panel_w, panel_h;
};

/* ── Lifecycle ───────────────────────────────────────────────────────────── */
void ui_init(UIContext* ui, Renderer* r, InputState* input, AudioEngine* audio);
void ui_set_era(UIContext* ui, Era era);
void ui_begin_frame(UIContext* ui, f32 dt);
void ui_end_frame(UIContext* ui);

/* ── Widgets ─────────────────────────────────────────────────────────────── */
bool ui_button(UIContext* ui, u32 id, const char* label, Rect rect);
bool ui_button_icon(UIContext* ui, u32 id, const char* label, const char* icon_char, Rect rect);

void ui_label(UIContext* ui, const char* text, f32 x, f32 y, Color c);
void ui_label_scaled(UIContext* ui, const char* text, f32 x, f32 y, Color c, int scale);

void ui_panel(UIContext* ui, Rect rect, const char* title);
void ui_panel_flat(UIContext* ui, Rect rect, Color bg);

void ui_progress_bar(UIContext* ui, Rect rect, f32 progress, Color fill_color, const char* label);
void ui_separator(UIContext* ui, f32 x, f32 y, f32 width);

void ui_tooltip(UIContext* ui, const char* text);

/* ── Formatted text ──────────────────────────────────────────────────────── */
void ui_money_label(UIContext* ui, f32 x, f32 y, double amount);
void ui_stat_bar(UIContext* ui, const char* label, f32 value, Rect rect);

/* ── HUD ─────────────────────────────────────────────────────────────────── */
#define HUD_HEIGHT 40

/* ── ID Generation ───────────────────────────────────────────────────────── */
#define UI_ID(n) ((u32)(__LINE__ * 1000 + (n)))

#endif /* ENGINE_UI_H */
