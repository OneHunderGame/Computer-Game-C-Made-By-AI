/*******************************************************************************
 * Silicon Legacy — UI System (Implementation)
 * Era-themed immediate-mode UI with animations
 ******************************************************************************/
#include "engine/ui.h"
#include <stdio.h>

/* ═══════════════════════════════════════════════════════════════════════════
 * Era Theme Definitions
 * Each era has a completely different visual identity
 * ═══════════════════════════════════════════════════════════════════════════ */
static const UITheme ERA_THEMES[ERA_COUNT] = {
    /* ── 1960s: Green phosphor CRT terminal ──────────────────────────────── */
    {
        .bg_primary    = {5, 10, 5, 255},
        .bg_secondary  = {10, 25, 10, 255},
        .bg_tertiary   = {15, 35, 15, 255},
        .accent        = {0, 255, 65, 255},
        .accent_hover  = {80, 255, 120, 255},
        .text_primary  = {0, 220, 50, 255},
        .text_secondary= {0, 150, 35, 200},
        .text_accent   = {0, 255, 65, 255},
        .success       = {0, 255, 100, 255},
        .warning       = {200, 255, 0, 255},
        .danger        = {255, 60, 60, 255},
        .border        = {0, 120, 30, 255},
        .button_bg     = {0, 80, 20, 255},
        .button_hover  = {0, 120, 35, 255},
        .button_active = {0, 160, 45, 255},
        .header_bg     = {0, 60, 15, 255},
        .header_text   = {0, 255, 65, 255},
        .corner_radius = 0,
        .text_scale    = 2,
        .era_label     = "1960s"
    },
    /* ── 1970s: Amber monochrome ─────────────────────────────────────────── */
    {
        .bg_primary    = {15, 10, 5, 255},
        .bg_secondary  = {30, 20, 8, 255},
        .bg_tertiary   = {45, 30, 12, 255},
        .accent        = {255, 176, 0, 255},
        .accent_hover  = {255, 200, 60, 255},
        .text_primary  = {255, 176, 0, 255},
        .text_secondary= {180, 120, 0, 200},
        .text_accent   = {255, 200, 60, 255},
        .success       = {255, 200, 0, 255},
        .warning       = {255, 150, 0, 255},
        .danger        = {255, 80, 0, 255},
        .border        = {120, 80, 0, 255},
        .button_bg     = {80, 55, 0, 255},
        .button_hover  = {120, 80, 0, 255},
        .button_active = {160, 110, 0, 255},
        .header_bg     = {60, 40, 0, 255},
        .header_text   = {255, 176, 0, 255},
        .corner_radius = 2,
        .text_scale    = 2,
        .era_label     = "1970s"
    },
    /* ── 1980s: Neon/synthwave ───────────────────────────────────────────── */
    {
        .bg_primary    = {10, 5, 20, 255},
        .bg_secondary  = {20, 10, 40, 255},
        .bg_tertiary   = {35, 15, 60, 255},
        .accent        = {0, 255, 255, 255},
        .accent_hover  = {100, 255, 255, 255},
        .text_primary  = {220, 220, 255, 255},
        .text_secondary= {140, 140, 180, 200},
        .text_accent   = {255, 0, 200, 255},
        .success       = {0, 255, 150, 255},
        .warning       = {255, 255, 0, 255},
        .danger        = {255, 0, 80, 255},
        .border        = {100, 0, 200, 255},
        .button_bg     = {60, 0, 120, 255},
        .button_hover  = {100, 0, 200, 255},
        .button_active = {140, 0, 255, 255},
        .header_bg     = {40, 0, 80, 255},
        .header_text   = {0, 255, 255, 255},
        .corner_radius = 4,
        .text_scale    = 2,
        .era_label     = "1980s"
    },
    /* ── 1990s: Windows 3.1 / early web ──────────────────────────────────── */
    {
        .bg_primary    = {0, 80, 80, 255},
        .bg_secondary  = {192, 192, 192, 255},
        .bg_tertiary   = {224, 224, 224, 255},
        .accent        = {0, 0, 128, 255},
        .accent_hover  = {0, 0, 180, 255},
        .text_primary  = {0, 0, 0, 255},
        .text_secondary= {80, 80, 80, 200},
        .text_accent   = {0, 0, 128, 255},
        .success       = {0, 128, 0, 255},
        .warning       = {200, 200, 0, 255},
        .danger        = {200, 0, 0, 255},
        .border        = {128, 128, 128, 255},
        .button_bg     = {192, 192, 192, 255},
        .button_hover  = {210, 210, 210, 255},
        .button_active = {160, 160, 160, 255},
        .header_bg     = {0, 0, 128, 255},
        .header_text   = {255, 255, 255, 255},
        .corner_radius = 2,
        .text_scale    = 2,
        .era_label     = "1990s"
    },
    /* ── 2000s: Glossy / skeuomorphic ────────────────────────────────────── */
    {
        .bg_primary    = {230, 235, 240, 255},
        .bg_secondary  = {255, 255, 255, 255},
        .bg_tertiary   = {240, 245, 250, 255},
        .accent        = {0, 120, 215, 255},
        .accent_hover  = {30, 150, 240, 255},
        .text_primary  = {30, 30, 30, 255},
        .text_secondary= {100, 100, 100, 200},
        .text_accent   = {0, 100, 200, 255},
        .success       = {50, 180, 50, 255},
        .warning       = {240, 180, 0, 255},
        .danger        = {220, 50, 50, 255},
        .border        = {180, 180, 190, 255},
        .button_bg     = {230, 240, 250, 255},
        .button_hover  = {200, 220, 250, 255},
        .button_active = {170, 200, 240, 255},
        .header_bg     = {40, 60, 90, 255},
        .header_text   = {255, 255, 255, 255},
        .corner_radius = 6,
        .text_scale    = 2,
        .era_label     = "2000s"
    },
    /* ── 2010s: Flat / Material Design ───────────────────────────────────── */
    {
        .bg_primary    = {250, 250, 250, 255},
        .bg_secondary  = {255, 255, 255, 255},
        .bg_tertiary   = {245, 245, 245, 255},
        .accent        = {33, 150, 243, 255},
        .accent_hover  = {66, 165, 245, 255},
        .text_primary  = {33, 33, 33, 255},
        .text_secondary= {117, 117, 117, 200},
        .text_accent   = {33, 150, 243, 255},
        .success       = {76, 175, 80, 255},
        .warning       = {255, 152, 0, 255},
        .danger        = {244, 67, 54, 255},
        .border        = {224, 224, 224, 255},
        .button_bg     = {33, 150, 243, 255},
        .button_hover  = {25, 118, 210, 255},
        .button_active = {21, 101, 192, 255},
        .header_bg     = {25, 118, 210, 255},
        .header_text   = {255, 255, 255, 255},
        .corner_radius = 8,
        .text_scale    = 2,
        .era_label     = "2010s"
    },
    /* ── 2020s: Dark mode / Glassmorphism ────────────────────────────────── */
    {
        .bg_primary    = {13, 17, 23, 255},
        .bg_secondary  = {22, 27, 34, 255},
        .bg_tertiary   = {33, 38, 45, 255},
        .accent        = {88, 166, 255, 255},
        .accent_hover  = {120, 190, 255, 255},
        .text_primary  = {230, 237, 243, 255},
        .text_secondary= {125, 133, 144, 200},
        .text_accent   = {88, 166, 255, 255},
        .success       = {63, 185, 80, 255},
        .warning       = {210, 153, 34, 255},
        .danger        = {248, 81, 73, 255},
        .border        = {48, 54, 61, 255},
        .button_bg     = {33, 38, 45, 255},
        .button_hover  = {48, 54, 61, 255},
        .button_active = {63, 68, 75, 255},
        .header_bg     = {22, 27, 34, 255},
        .header_text   = {230, 237, 243, 255},
        .corner_radius = 10,
        .text_scale    = 2,
        .era_label     = "2020s"
    },
};

/* ═══════════════════════════════════════════════════════════════════════════
 * Lifecycle
 * ═══════════════════════════════════════════════════════════════════════════ */
void ui_init(UIContext* ui, Renderer* r, InputState* input, AudioEngine* audio) {
    memset(ui, 0, sizeof(*ui));
    ui->renderer = r;
    ui->input = input;
    ui->audio = audio;
    ui_set_era(ui, ERA_1960);
}

void ui_set_era(UIContext* ui, Era era) {
    if (era >= ERA_COUNT) era = ERA_2020;
    ui->current_era = era;
    ui->theme = ERA_THEMES[era];
}

void ui_begin_frame(UIContext* ui, f32 dt) {
    ui->dt = dt;
    ui->time += dt;
    ui->has_tooltip = false;
    /* Reset hot widget — will be set by widgets this frame */
    ui->hot_id = 0;
}

void ui_end_frame(UIContext* ui) {
    /* Draw tooltip if any */
    if (ui->has_tooltip) {
        int tw = text_width(ui->tooltip_text, ui->theme.text_scale);
        int th = text_height(ui->theme.text_scale);
        f32 pad = 8;
        Rect bg = RECT(ui->tooltip_x + 12, ui->tooltip_y + 12,
                       tw + pad * 2, th + pad * 2);

        /* Keep on screen */
        if (bg.x + bg.w > ui->renderer->screen_w)
            bg.x = ui->renderer->screen_w - bg.w - 4;
        if (bg.y + bg.h > ui->renderer->screen_h)
            bg.y = ui->renderer->screen_h - bg.h - 4;

        draw_rect(ui->renderer, bg, color_alpha(ui->theme.bg_tertiary, 240));
        draw_rect_outline(ui->renderer, bg, ui->theme.border, 1);
        draw_text(ui->renderer, ui->tooltip_text,
                  bg.x + pad, bg.y + pad,
                  ui->theme.text_primary, ui->theme.text_scale);
    }

    /* Release active widget if mouse released */
    if (ui->input->mouse_released[0]) {
        ui->active_id = 0;
    }
}

/* ═══════════════════════════════════════════════════════════════════════════
 * Widget: Button
 * ═══════════════════════════════════════════════════════════════════════════ */
bool ui_button(UIContext* ui, u32 id, const char* label, Rect rect) {
    Renderer* r = ui->renderer;
    InputState* in = ui->input;
    UITheme* t = &ui->theme;

    bool hovered = mouse_in_rect(in, rect);
    bool pressed = false;

    if (hovered) {
        ui->hot_id = id;
        if (in->mouse_pressed[0]) {
            ui->active_id = id;
        }
    }

    bool is_active = (ui->active_id == id);
    bool is_hot = (ui->hot_id == id);

    /* Determine visual state */
    Color bg_color = t->button_bg;
    Color text_color = t->text_primary;

    if (is_active && hovered) {
        bg_color = t->button_active;
        /* Slight scale down effect */
    } else if (is_hot) {
        bg_color = t->button_hover;
    }

    /* ── Draw button ─────────────────────────────────────────────────── */

    /* Shadow (subtle depth) */
    if (t->corner_radius > 0 && !is_active) {
        draw_rect_rounded(r, RECT(rect.x + 1, rect.y + 2, rect.w, rect.h),
                          color_alpha(COL_BLACK, 40), t->corner_radius);
    }

    /* Background */
    if (t->corner_radius > 0) {
        draw_rect_rounded(r, rect, bg_color, t->corner_radius);
    } else {
        draw_rect(r, rect, bg_color);
        /* 1960s/70s style border */
        draw_rect_outline(r, rect, t->border, 1);
    }

    /* Hover glow for modern eras */
    if (is_hot && ui->current_era >= ERA_2000) {
        Color glow = color_alpha(t->accent, 30);
        draw_rect_rounded(r, rect, glow, t->corner_radius);
    }

    /* Label */
    draw_text_centered(r, label, rect, text_color, t->text_scale);

    /* Click detection */
    if (is_active && hovered && in->mouse_released[0]) {
        pressed = true;
        if (ui->audio) audio_play_sfx(ui->audio, SFX_CLICK);
    }

    return pressed;
}

bool ui_button_icon(UIContext* ui, u32 id, const char* label, const char* icon_char, Rect rect) {
    /* Draw icon on left + label */
    bool result = ui_button(ui, id, "", rect);

    int icon_scale = ui->theme.text_scale + 1;
    int label_scale = ui->theme.text_scale;
    int iw = text_width(icon_char, icon_scale);
    int lw = text_width(label, label_scale);
    int total = iw + 8 + lw;
    f32 start_x = rect.x + (rect.w - total) / 2.0f;
    f32 cy = rect.y + (rect.h - text_height(icon_scale)) / 2.0f;

    Color tc = ui->theme.text_primary;
    draw_text(ui->renderer, icon_char, start_x, cy, ui->theme.accent, icon_scale);
    draw_text(ui->renderer, label, start_x + iw + 8,
              rect.y + (rect.h - text_height(label_scale)) / 2.0f, tc, label_scale);

    return result;
}

/* ═══════════════════════════════════════════════════════════════════════════
 * Widget: Label
 * ═══════════════════════════════════════════════════════════════════════════ */
void ui_label(UIContext* ui, const char* text, f32 x, f32 y, Color c) {
    draw_text(ui->renderer, text, x, y, c, ui->theme.text_scale);
}

void ui_label_scaled(UIContext* ui, const char* text, f32 x, f32 y, Color c, int scale) {
    draw_text(ui->renderer, text, x, y, c, scale);
}

/* ═══════════════════════════════════════════════════════════════════════════
 * Widget: Panel
 * ═══════════════════════════════════════════════════════════════════════════ */
void ui_panel(UIContext* ui, Rect rect, const char* title) {
    Renderer* r = ui->renderer;
    UITheme* t = &ui->theme;

    /* Background */
    if (t->corner_radius > 0) {
        draw_rect_rounded(r, rect, t->bg_secondary, t->corner_radius);
    } else {
        draw_rect(r, rect, t->bg_secondary);
    }

    /* Border */
    draw_rect_outline(r, rect, t->border, 1);

    /* Title bar */
    if (title) {
        Rect title_bar = RECT(rect.x, rect.y, rect.w, 30);
        if (t->corner_radius > 0) {
            draw_rect(r, RECT(title_bar.x, title_bar.y, title_bar.w, title_bar.h),
                      t->header_bg);
        } else {
            draw_rect(r, title_bar, t->header_bg);
        }
        draw_text(r, title, rect.x + 10, rect.y + 7,
                  t->header_text, t->text_scale);
        draw_line(r, rect.x, rect.y + 30, rect.x + rect.w, rect.y + 30, t->border);
    }
}

void ui_panel_flat(UIContext* ui, Rect rect, Color bg) {
    if (ui->theme.corner_radius > 0) {
        draw_rect_rounded(ui->renderer, rect, bg, ui->theme.corner_radius);
    } else {
        draw_rect(ui->renderer, rect, bg);
    }
}

/* ═══════════════════════════════════════════════════════════════════════════
 * Widget: Progress Bar
 * ═══════════════════════════════════════════════════════════════════════════ */
void ui_progress_bar(UIContext* ui, Rect rect, f32 progress, Color fill_color, const char* label) {
    Renderer* r = ui->renderer;
    UITheme* t = &ui->theme;

    progress = CLAMP(progress, 0.0f, 1.0f);

    /* Track */
    draw_rect(r, rect, t->bg_tertiary);
    draw_rect_outline(r, rect, t->border, 1);

    /* Fill */
    if (progress > 0.01f) {
        Rect fill = RECT(rect.x + 1, rect.y + 1,
                         (rect.w - 2) * progress, rect.h - 2);
        draw_rect(r, fill, fill_color);

        /* Animated shine effect */
        f32 shine_x = fmodf(ui->time * 80.0f, fill.w + 40.0f) - 20.0f;
        if (shine_x > 0 && shine_x < fill.w) {
            Rect shine = RECT(fill.x + shine_x, fill.y, 20, fill.h);
            draw_rect(r, shine, color_alpha(COL_WHITE, 30));
        }
    }

    /* Label */
    if (label) {
        draw_text_centered(r, label, rect, t->text_primary, t->text_scale);
    }
}

/* ═══════════════════════════════════════════════════════════════════════════
 * Widget: Separator
 * ═══════════════════════════════════════════════════════════════════════════ */
void ui_separator(UIContext* ui, f32 x, f32 y, f32 width) {
    draw_line(ui->renderer, x, y, x + width, y, ui->theme.border);
}

/* ═══════════════════════════════════════════════════════════════════════════
 * Widget: Tooltip
 * ═══════════════════════════════════════════════════════════════════════════ */
void ui_tooltip(UIContext* ui, const char* text) {
    strncpy(ui->tooltip_text, text, sizeof(ui->tooltip_text) - 1);
    ui->has_tooltip = true;
    ui->tooltip_x = ui->input->mouse_x;
    ui->tooltip_y = ui->input->mouse_y;
}

/* ═══════════════════════════════════════════════════════════════════════════
 * Formatted displays
 * ═══════════════════════════════════════════════════════════════════════════ */
void ui_money_label(UIContext* ui, f32 x, f32 y, double amount) {
    char buf[64];
    if (amount >= 1e9) {
        snprintf(buf, sizeof(buf), "$%.2fB", amount / 1e9);
    } else if (amount >= 1e6) {
        snprintf(buf, sizeof(buf), "$%.2fM", amount / 1e6);
    } else if (amount >= 1e3) {
        snprintf(buf, sizeof(buf), "$%.1fK", amount / 1e3);
    } else {
        snprintf(buf, sizeof(buf), "$%.0f", amount);
    }

    Color c = (amount >= 0) ? ui->theme.success : ui->theme.danger;
    ui_label(ui, buf, x, y, c);
}

void ui_stat_bar(UIContext* ui, const char* label, f32 value, Rect rect) {
    UITheme* t = &ui->theme;
    f32 label_w = (f32)text_width(label, t->text_scale) + 10;
    draw_text(ui->renderer, label, rect.x, rect.y + 2, t->text_secondary, t->text_scale);

    Rect bar = RECT(rect.x + label_w, rect.y, rect.w - label_w, rect.h);
    Color fill = color_lerp(t->danger, t->success, value);
    ui_progress_bar(ui, bar, value, fill, NULL);
}
