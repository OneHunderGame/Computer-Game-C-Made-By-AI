/*******************************************************************************
 * Silicon Legacy — Renderer (Implementation)
 * Handles all 2D drawing, text, and visual effects
 ******************************************************************************/
#include "engine/renderer.h"

/* ═══════════════════════════════════════════════════════════════════════════
 * Embedded 5x7 bitmap font data (ASCII 32-126)
 * Each character is 5 pixels wide stored in a byte per row (7 rows).
 * We render them scaled up to FONT_CHAR_W x FONT_CHAR_H for readability.
 * ═══════════════════════════════════════════════════════════════════════════ */
static const u8 FONT_5X7[95][7] = {
    {0x00,0x00,0x00,0x00,0x00,0x00,0x00}, /* space */
    {0x04,0x04,0x04,0x04,0x04,0x00,0x04}, /* ! */
    {0x0A,0x0A,0x00,0x00,0x00,0x00,0x00}, /* " */
    {0x0A,0x1F,0x0A,0x0A,0x1F,0x0A,0x00}, /* # */
    {0x04,0x0F,0x14,0x0E,0x05,0x1E,0x04}, /* $ */
    {0x19,0x19,0x02,0x04,0x08,0x13,0x13}, /* % */
    {0x0C,0x12,0x0C,0x0D,0x12,0x12,0x0D}, /* & */
    {0x04,0x04,0x00,0x00,0x00,0x00,0x00}, /* ' */
    {0x02,0x04,0x08,0x08,0x08,0x04,0x02}, /* ( */
    {0x08,0x04,0x02,0x02,0x02,0x04,0x08}, /* ) */
    {0x00,0x04,0x15,0x0E,0x15,0x04,0x00}, /* * */
    {0x00,0x04,0x04,0x1F,0x04,0x04,0x00}, /* + */
    {0x00,0x00,0x00,0x00,0x00,0x04,0x08}, /* , */
    {0x00,0x00,0x00,0x1F,0x00,0x00,0x00}, /* - */
    {0x00,0x00,0x00,0x00,0x00,0x00,0x04}, /* . */
    {0x01,0x01,0x02,0x04,0x08,0x10,0x10}, /* / */
    {0x0E,0x11,0x13,0x15,0x19,0x11,0x0E}, /* 0 */
    {0x04,0x0C,0x04,0x04,0x04,0x04,0x0E}, /* 1 */
    {0x0E,0x11,0x01,0x06,0x08,0x10,0x1F}, /* 2 */
    {0x0E,0x11,0x01,0x06,0x01,0x11,0x0E}, /* 3 */
    {0x02,0x06,0x0A,0x12,0x1F,0x02,0x02}, /* 4 */
    {0x1F,0x10,0x1E,0x01,0x01,0x11,0x0E}, /* 5 */
    {0x06,0x08,0x10,0x1E,0x11,0x11,0x0E}, /* 6 */
    {0x1F,0x01,0x02,0x04,0x08,0x08,0x08}, /* 7 */
    {0x0E,0x11,0x11,0x0E,0x11,0x11,0x0E}, /* 8 */
    {0x0E,0x11,0x11,0x0F,0x01,0x02,0x0C}, /* 9 */
    {0x00,0x00,0x04,0x00,0x00,0x04,0x00}, /* : */
    {0x00,0x00,0x04,0x00,0x00,0x04,0x08}, /* ; */
    {0x02,0x04,0x08,0x10,0x08,0x04,0x02}, /* < */
    {0x00,0x00,0x1F,0x00,0x1F,0x00,0x00}, /* = */
    {0x08,0x04,0x02,0x01,0x02,0x04,0x08}, /* > */
    {0x0E,0x11,0x01,0x06,0x04,0x00,0x04}, /* ? */
    {0x0E,0x11,0x17,0x15,0x17,0x10,0x0E}, /* @ */
    {0x0E,0x11,0x11,0x1F,0x11,0x11,0x11}, /* A */
    {0x1E,0x11,0x11,0x1E,0x11,0x11,0x1E}, /* B */
    {0x0E,0x11,0x10,0x10,0x10,0x11,0x0E}, /* C */
    {0x1E,0x11,0x11,0x11,0x11,0x11,0x1E}, /* D */
    {0x1F,0x10,0x10,0x1E,0x10,0x10,0x1F}, /* E */
    {0x1F,0x10,0x10,0x1E,0x10,0x10,0x10}, /* F */
    {0x0E,0x11,0x10,0x17,0x11,0x11,0x0E}, /* G */
    {0x11,0x11,0x11,0x1F,0x11,0x11,0x11}, /* H */
    {0x0E,0x04,0x04,0x04,0x04,0x04,0x0E}, /* I */
    {0x07,0x02,0x02,0x02,0x02,0x12,0x0C}, /* J */
    {0x11,0x12,0x14,0x18,0x14,0x12,0x11}, /* K */
    {0x10,0x10,0x10,0x10,0x10,0x10,0x1F}, /* L */
    {0x11,0x1B,0x15,0x15,0x11,0x11,0x11}, /* M */
    {0x11,0x19,0x15,0x13,0x11,0x11,0x11}, /* N */
    {0x0E,0x11,0x11,0x11,0x11,0x11,0x0E}, /* O */
    {0x1E,0x11,0x11,0x1E,0x10,0x10,0x10}, /* P */
    {0x0E,0x11,0x11,0x11,0x15,0x12,0x0D}, /* Q */
    {0x1E,0x11,0x11,0x1E,0x14,0x12,0x11}, /* R */
    {0x0E,0x11,0x10,0x0E,0x01,0x11,0x0E}, /* S */
    {0x1F,0x04,0x04,0x04,0x04,0x04,0x04}, /* T */
    {0x11,0x11,0x11,0x11,0x11,0x11,0x0E}, /* U */
    {0x11,0x11,0x11,0x11,0x0A,0x0A,0x04}, /* V */
    {0x11,0x11,0x11,0x15,0x15,0x1B,0x11}, /* W */
    {0x11,0x11,0x0A,0x04,0x0A,0x11,0x11}, /* X */
    {0x11,0x11,0x0A,0x04,0x04,0x04,0x04}, /* Y */
    {0x1F,0x01,0x02,0x04,0x08,0x10,0x1F}, /* Z */
    {0x0E,0x08,0x08,0x08,0x08,0x08,0x0E}, /* [ */
    {0x10,0x10,0x08,0x04,0x02,0x01,0x01}, /* \ */
    {0x0E,0x02,0x02,0x02,0x02,0x02,0x0E}, /* ] */
    {0x04,0x0A,0x11,0x00,0x00,0x00,0x00}, /* ^ */
    {0x00,0x00,0x00,0x00,0x00,0x00,0x1F}, /* _ */
    {0x08,0x04,0x00,0x00,0x00,0x00,0x00}, /* ` */
    {0x00,0x00,0x0E,0x01,0x0F,0x11,0x0F}, /* a */
    {0x10,0x10,0x1E,0x11,0x11,0x11,0x1E}, /* b */
    {0x00,0x00,0x0E,0x11,0x10,0x11,0x0E}, /* c */
    {0x01,0x01,0x0F,0x11,0x11,0x11,0x0F}, /* d */
    {0x00,0x00,0x0E,0x11,0x1F,0x10,0x0E}, /* e */
    {0x06,0x08,0x1E,0x08,0x08,0x08,0x08}, /* f */
    {0x00,0x00,0x0F,0x11,0x0F,0x01,0x0E}, /* g */
    {0x10,0x10,0x1E,0x11,0x11,0x11,0x11}, /* h */
    {0x04,0x00,0x0C,0x04,0x04,0x04,0x0E}, /* i */
    {0x02,0x00,0x06,0x02,0x02,0x12,0x0C}, /* j */
    {0x10,0x10,0x12,0x14,0x18,0x14,0x12}, /* k */
    {0x0C,0x04,0x04,0x04,0x04,0x04,0x0E}, /* l */
    {0x00,0x00,0x1A,0x15,0x15,0x15,0x15}, /* m */
    {0x00,0x00,0x1E,0x11,0x11,0x11,0x11}, /* n */
    {0x00,0x00,0x0E,0x11,0x11,0x11,0x0E}, /* o */
    {0x00,0x00,0x1E,0x11,0x1E,0x10,0x10}, /* p */
    {0x00,0x00,0x0F,0x11,0x0F,0x01,0x01}, /* q */
    {0x00,0x00,0x16,0x19,0x10,0x10,0x10}, /* r */
    {0x00,0x00,0x0F,0x10,0x0E,0x01,0x1E}, /* s */
    {0x08,0x08,0x1E,0x08,0x08,0x09,0x06}, /* t */
    {0x00,0x00,0x11,0x11,0x11,0x11,0x0F}, /* u */
    {0x00,0x00,0x11,0x11,0x11,0x0A,0x04}, /* v */
    {0x00,0x00,0x11,0x11,0x15,0x15,0x0A}, /* w */
    {0x00,0x00,0x11,0x0A,0x04,0x0A,0x11}, /* x */
    {0x00,0x00,0x11,0x11,0x0F,0x01,0x0E}, /* y */
    {0x00,0x00,0x1F,0x02,0x04,0x08,0x1F}, /* z */
    {0x02,0x04,0x04,0x08,0x04,0x04,0x02}, /* { */
    {0x04,0x04,0x04,0x04,0x04,0x04,0x04}, /* | */
    {0x08,0x04,0x04,0x02,0x04,0x04,0x08}, /* } */
    {0x00,0x00,0x08,0x15,0x02,0x00,0x00}, /* ~ */
};

/* ═══════════════════════════════════════════════════════════════════════════
 * Font texture creation
 * ═══════════════════════════════════════════════════════════════════════════ */
static bool create_font_texture(Renderer* r) {
    int tex_w = 5 * 95;  /* each char 5px wide, 95 glyphs */
    int tex_h = 7;
    SDL_Surface* surf = SDL_CreateRGBSurfaceWithFormat(0, tex_w, tex_h, 32,
                                                       SDL_PIXELFORMAT_RGBA8888);
    if (!surf) return false;

    SDL_LockSurface(surf);
    u32* pixels = (u32*)surf->pixels;
    memset(pixels, 0, (size_t)(tex_w * tex_h * 4));

    for (int ch = 0; ch < 95; ch++) {
        for (int row = 0; row < 7; row++) {
            u8 bits = FONT_5X7[ch][row];
            for (int col = 0; col < 5; col++) {
                if (bits & (0x10 >> col)) {
                    int px = ch * 5 + col;
                    int py = row;
                    pixels[py * tex_w + px] = 0xFFFFFFFF;
                }
            }
        }
    }

    SDL_UnlockSurface(surf);
    r->font_texture = SDL_CreateTextureFromSurface(r->sdl, surf);
    SDL_FreeSurface(surf);

    if (!r->font_texture) return false;
    SDL_SetTextureBlendMode(r->font_texture, SDL_BLENDMODE_BLEND);
    return true;
}

/* ═══════════════════════════════════════════════════════════════════════════
 * Lifecycle
 * ═══════════════════════════════════════════════════════════════════════════ */
bool renderer_init(Renderer* r, const char* title, int w, int h) {
    memset(r, 0, sizeof(*r));
    r->screen_w = w;
    r->screen_h = h;
    r->ui_scale = 1.0f;

    r->window = SDL_CreateWindow(title,
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        w, h, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
    if (!r->window) {
        fprintf(stderr, "[RENDERER] Window creation failed: %s\n", SDL_GetError());
        return false;
    }

    r->sdl = SDL_CreateRenderer(r->window, -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!r->sdl) {
        fprintf(stderr, "[RENDERER] Renderer creation failed: %s\n", SDL_GetError());
        return false;
    }

    SDL_SetRenderDrawBlendMode(r->sdl, SDL_BLENDMODE_BLEND);

    if (!create_font_texture(r)) {
        fprintf(stderr, "[RENDERER] Font texture creation failed\n");
        return false;
    }

    printf("[RENDERER] Initialized %dx%d\n", w, h);
    return true;
}

void renderer_shutdown(Renderer* r) {
    if (r->font_texture) SDL_DestroyTexture(r->font_texture);
    if (r->sdl)          SDL_DestroyRenderer(r->sdl);
    if (r->window)       SDL_DestroyWindow(r->window);
    memset(r, 0, sizeof(*r));
}

void renderer_begin_frame(Renderer* r) {
    /* Update screen size for responsive layout */
    SDL_GetRendererOutputSize(r->sdl, &r->screen_w, &r->screen_h);
    SDL_SetRenderDrawColor(r->sdl, 0, 0, 0, 255);
    SDL_RenderClear(r->sdl);
}

void renderer_end_frame(Renderer* r) {
    SDL_RenderPresent(r->sdl);
}

/* ═══════════════════════════════════════════════════════════════════════════
 * Primitives
 * ═══════════════════════════════════════════════════════════════════════════ */
void draw_rect(Renderer* r, Rect rect, Color c) {
    SDL_SetRenderDrawColor(r->sdl, c.r, c.g, c.b, c.a);
    SDL_FRect fr = { rect.x, rect.y, rect.w, rect.h };
    SDL_RenderFillRectF(r->sdl, &fr);
}

void draw_rect_outline(Renderer* r, Rect rect, Color c, int thickness) {
    SDL_SetRenderDrawColor(r->sdl, c.r, c.g, c.b, c.a);
    for (int i = 0; i < thickness; i++) {
        SDL_FRect fr = { rect.x + i, rect.y + i, rect.w - 2*i, rect.h - 2*i };
        SDL_RenderDrawRectF(r->sdl, &fr);
    }
}

void draw_rect_rounded(Renderer* r, Rect rect, Color c, int radius) {
    /* Draw the main body (3 overlapping rects + corner circles) */
    SDL_SetRenderDrawColor(r->sdl, c.r, c.g, c.b, c.a);

    /* Center rect */
    SDL_FRect center = { rect.x + radius, rect.y, rect.w - 2*radius, rect.h };
    SDL_RenderFillRectF(r->sdl, &center);
    /* Left rect */
    SDL_FRect left = { rect.x, rect.y + radius, (f32)radius, rect.h - 2*radius };
    SDL_RenderFillRectF(r->sdl, &left);
    /* Right rect */
    SDL_FRect right = { rect.x + rect.w - radius, rect.y + radius, (f32)radius, rect.h - 2*radius };
    SDL_RenderFillRectF(r->sdl, &right);

    /* Corners (filled quarter circles) */
    f32 corners[4][2] = {
        { rect.x + radius,          rect.y + radius },           /* TL */
        { rect.x + rect.w - radius, rect.y + radius },           /* TR */
        { rect.x + radius,          rect.y + rect.h - radius },  /* BL */
        { rect.x + rect.w - radius, rect.y + rect.h - radius },  /* BR */
    };

    for (int corner = 0; corner < 4; corner++) {
        f32 cx = corners[corner][0];
        f32 cy = corners[corner][1];
        for (int dy = -radius; dy <= radius; dy++) {
            for (int dx = -radius; dx <= radius; dx++) {
                if (dx*dx + dy*dy <= radius*radius) {
                    SDL_RenderDrawPointF(r->sdl, cx + dx, cy + dy);
                }
            }
        }
    }
}

void draw_rect_gradient_v(Renderer* r, Rect rect, Color top, Color bottom) {
    for (int y = 0; y < (int)rect.h; y++) {
        f32 t = (f32)y / rect.h;
        Color c = color_lerp(top, bottom, t);
        SDL_SetRenderDrawColor(r->sdl, c.r, c.g, c.b, c.a);
        SDL_RenderDrawLineF(r->sdl, rect.x, rect.y + y,
                           rect.x + rect.w, rect.y + y);
    }
}

void draw_rect_gradient_h(Renderer* r, Rect rect, Color left, Color right) {
    for (int x = 0; x < (int)rect.w; x++) {
        f32 t = (f32)x / rect.w;
        Color c = color_lerp(left, right, t);
        SDL_SetRenderDrawColor(r->sdl, c.r, c.g, c.b, c.a);
        SDL_RenderDrawLineF(r->sdl, rect.x + x, rect.y,
                           rect.x + x, rect.y + rect.h);
    }
}

void draw_line(Renderer* r, f32 x1, f32 y1, f32 x2, f32 y2, Color c) {
    SDL_SetRenderDrawColor(r->sdl, c.r, c.g, c.b, c.a);
    SDL_RenderDrawLineF(r->sdl, x1, y1, x2, y2);
}

void draw_circle(Renderer* r, f32 cx, f32 cy, f32 radius, Color c) {
    SDL_SetRenderDrawColor(r->sdl, c.r, c.g, c.b, c.a);
    int rad = (int)radius;
    for (int dy = -rad; dy <= rad; dy++) {
        for (int dx = -rad; dx <= rad; dx++) {
            if (dx*dx + dy*dy <= rad*rad) {
                SDL_RenderDrawPointF(r->sdl, cx + dx, cy + dy);
            }
        }
    }
}

void draw_circle_outline(Renderer* r, f32 cx, f32 cy, f32 radius, Color c) {
    SDL_SetRenderDrawColor(r->sdl, c.r, c.g, c.b, c.a);
    int rad = (int)radius;
    for (int dy = -rad; dy <= rad; dy++) {
        for (int dx = -rad; dx <= rad; dx++) {
            int d = dx*dx + dy*dy;
            if (d >= (rad-1)*(rad-1) && d <= rad*rad) {
                SDL_RenderDrawPointF(r->sdl, cx + dx, cy + dy);
            }
        }
    }
}

/* ═══════════════════════════════════════════════════════════════════════════
 * Text Rendering
 * ═══════════════════════════════════════════════════════════════════════════ */
void draw_text(Renderer* r, const char* text, f32 x, f32 y, Color c, int scale) {
    if (!text || !r->font_texture) return;

    SDL_SetTextureColorMod(r->font_texture, c.r, c.g, c.b);
    SDL_SetTextureAlphaMod(r->font_texture, c.a);

    int char_w = 5 * scale;
    int char_h = 7 * scale;
    int spacing = 1 * scale;

    f32 cx = x;
    for (const char* p = text; *p; p++) {
        if (*p == '\n') {
            cx = x;
            y += char_h + spacing;
            continue;
        }
        int idx = *p - 32;
        if (idx < 0 || idx >= 95) { cx += char_w + spacing; continue; }

        SDL_Rect src = { idx * 5, 0, 5, 7 };
        SDL_FRect dst = { cx, y, (f32)char_w, (f32)char_h };
        SDL_RenderCopyF(r->sdl, r->font_texture, &src, &dst);
        cx += char_w + spacing;
    }
}

void draw_text_centered(Renderer* r, const char* text, Rect area, Color c, int scale) {
    int tw = text_width(text, scale);
    int th = text_height(scale);
    f32 x = area.x + (area.w - tw) / 2.0f;
    f32 y = area.y + (area.h - th) / 2.0f;
    draw_text(r, text, x, y, c, scale);
}

int text_width(const char* text, int scale) {
    if (!text) return 0;
    int max_w = 0, cur_w = 0;
    for (const char* p = text; *p; p++) {
        if (*p == '\n') {
            if (cur_w > max_w) max_w = cur_w;
            cur_w = 0;
        } else {
            cur_w += 5 * scale + 1 * scale;
        }
    }
    if (cur_w > max_w) max_w = cur_w;
    return max_w > 0 ? max_w - 1 * scale : 0;
}

int text_height(int scale) {
    return 7 * scale;
}

/* ═══════════════════════════════════════════════════════════════════════════
 * Color Utilities
 * ═══════════════════════════════════════════════════════════════════════════ */
Color color_lerp(Color a, Color b, f32 t) {
    t = CLAMP(t, 0.0f, 1.0f);
    return (Color){
        (u8)(a.r + (b.r - a.r) * t),
        (u8)(a.g + (b.g - a.g) * t),
        (u8)(a.b + (b.b - a.b) * t),
        (u8)(a.a + (b.a - a.a) * t),
    };
}

Color color_alpha(Color c, u8 alpha) {
    c.a = alpha;
    return c;
}

Color color_brighten(Color c, f32 factor) {
    return (Color){
        (u8)MIN(255, (int)(c.r * factor)),
        (u8)MIN(255, (int)(c.g * factor)),
        (u8)MIN(255, (int)(c.b * factor)),
        c.a
    };
}

Color color_darken(Color c, f32 factor) {
    return (Color){
        (u8)(c.r * factor),
        (u8)(c.g * factor),
        (u8)(c.b * factor),
        c.a
    };
}
