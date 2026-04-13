/*******************************************************************************
 * Silicon Legacy — Renderer
 * 2D rendering via SDL2: primitives, gradients, text, sprites
 ******************************************************************************/
#ifndef ENGINE_RENDERER_H
#define ENGINE_RENDERER_H

#include "common.h"
#include <SDL.h>

/* ── Embedded Bitmap Font ────────────────────────────────────────────────── */
#define FONT_CHAR_W  8
#define FONT_CHAR_H  14
#define FONT_FIRST   32
#define FONT_LAST    126

/* ── Renderer State ──────────────────────────────────────────────────────── */
struct Renderer {
    SDL_Window*   window;
    SDL_Renderer* sdl;
    int           screen_w;
    int           screen_h;
    SDL_Texture*  font_texture;
    f32           ui_scale;
};

/* ── Lifecycle ───────────────────────────────────────────────────────────── */
bool renderer_init(Renderer* r, const char* title, int w, int h);
void renderer_shutdown(Renderer* r);
void renderer_begin_frame(Renderer* r);
void renderer_end_frame(Renderer* r);

/* ── Primitives ──────────────────────────────────────────────────────────── */
void draw_rect(Renderer* r, Rect rect, Color c);
void draw_rect_outline(Renderer* r, Rect rect, Color c, int thickness);
void draw_rect_rounded(Renderer* r, Rect rect, Color c, int radius);
void draw_rect_gradient_v(Renderer* r, Rect rect, Color top, Color bottom);
void draw_rect_gradient_h(Renderer* r, Rect rect, Color left, Color right);
void draw_line(Renderer* r, f32 x1, f32 y1, f32 x2, f32 y2, Color c);
void draw_circle(Renderer* r, f32 cx, f32 cy, f32 radius, Color c);
void draw_circle_outline(Renderer* r, f32 cx, f32 cy, f32 radius, Color c);

/* ── Text ────────────────────────────────────────────────────────────────── */
void draw_text(Renderer* r, const char* text, f32 x, f32 y, Color c, int scale);
void draw_text_centered(Renderer* r, const char* text, Rect area, Color c, int scale);
int  text_width(const char* text, int scale);
int  text_height(int scale);

/* ── Color Utilities ─────────────────────────────────────────────────────── */
Color color_lerp(Color a, Color b, f32 t);
Color color_alpha(Color c, u8 alpha);
Color color_brighten(Color c, f32 factor);
Color color_darken(Color c, f32 factor);

#endif /* ENGINE_RENDERER_H */
