/*******************************************************************************
 * Silicon Legacy — Common Types & Definitions
 * Shared across the entire project
 ******************************************************************************/
#ifndef COMMON_H
#define COMMON_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>

/* ── Numeric Aliases ─────────────────────────────────────────────────────── */
typedef uint8_t   u8;
typedef uint16_t  u16;
typedef uint32_t  u32;
typedef uint64_t  u64;
typedef int8_t    i8;
typedef int16_t   i16;
typedef int32_t   i32;
typedef int64_t   i64;
typedef float     f32;
typedef double    f64;

/* ── Utility Macros ──────────────────────────────────────────────────────── */
#define ARRAY_LEN(a)      (sizeof(a) / sizeof((a)[0]))
#define MIN(a, b)         ((a) < (b) ? (a) : (b))
#define MAX(a, b)         ((a) > (b) ? (a) : (b))
#define CLAMP(x, lo, hi)  (MIN(MAX((x), (lo)), (hi)))
#define LERP(a, b, t)     ((a) + ((b) - (a)) * (t))

/* ── Color ───────────────────────────────────────────────────────────────── */
typedef struct {
    u8 r, g, b, a;
} Color;

#define COLOR(r, g, b, a) ((Color){(r), (g), (b), (a)})
#define COLOR_RGB(r, g, b) ((Color){(r), (g), (b), 255})

/* ── Commonly used colors ────────────────────────────────────────────────── */
#define COL_WHITE       COLOR_RGB(255, 255, 255)
#define COL_BLACK       COLOR_RGB(0, 0, 0)
#define COL_TRANSPARENT COLOR(0, 0, 0, 0)

/* ── Rect ────────────────────────────────────────────────────────────────── */
typedef struct {
    f32 x, y, w, h;
} Rect;

#define RECT(x, y, w, h) ((Rect){(x), (y), (w), (h)})

static inline bool rect_contains(Rect r, f32 px, f32 py) {
    return px >= r.x && px < r.x + r.w && py >= r.y && py < r.y + r.h;
}

/* ── Vec2 ────────────────────────────────────────────────────────────────── */
typedef struct {
    f32 x, y;
} Vec2;

#define VEC2(x, y) ((Vec2){(x), (y)})

/* ── Game Constants ──────────────────────────────────────────────────────── */
#define WINDOW_W            1280
#define WINDOW_H            720
#define TARGET_FPS          60
#define SIM_TICK_RATE       10
#define MAX_COMPUTERS       64
#define MAX_SOFTWARE        128
#define MAX_EMPLOYEES       256
#define MAX_COMPONENTS      512
#define MAX_TECH_NODES      256
#define MAX_EVENTS          32
#define MAX_COMPETITORS     8
#define MAX_NAME_LEN        64
#define MAX_DESC_LEN        256

/* ── Era Definitions ─────────────────────────────────────────────────────── */
typedef enum {
    ERA_1960 = 0,
    ERA_1970,
    ERA_1980,
    ERA_1990,
    ERA_2000,
    ERA_2010,
    ERA_2020,
    ERA_COUNT
} Era;

static const u16 ERA_YEARS[] = { 1960, 1970, 1980, 1990, 2000, 2010, 2020 };

static inline const char* era_name(Era e) {
    static const char* names[] = {
        "1960s", "1970s", "1980s", "1990s", "2000s", "2010s", "2020s"
    };
    return (e < ERA_COUNT) ? names[e] : "Unknown";
}

/* ── Screen IDs ──────────────────────────────────────────────────────────── */
typedef enum {
    SCREEN_TITLE = 0,
    SCREEN_WORKBENCH,
    SCREEN_OFFICE,
    SCREEN_MARKET,
    SCREEN_TECH_TREE,
    SCREEN_COUNT
} ScreenID;

/* ── Component Types ─────────────────────────────────────────────────────── */
typedef enum {
    COMP_CPU = 0,
    COMP_RAM,
    COMP_STORAGE,
    COMP_DISPLAY,
    COMP_MOTHERBOARD,
    COMP_PSU,
    COMP_CASE,
    COMP_TYPE_COUNT
} ComponentType;

static inline const char* component_type_name(ComponentType t) {
    static const char* names[] = {
        "CPU", "RAM", "Storage", "Display", "Motherboard", "PSU", "Case"
    };
    return (t < COMP_TYPE_COUNT) ? names[t] : "Unknown";
}

/* ── Forward Declarations ────────────────────────────────────────────────── */
typedef struct GameState GameState;
typedef struct Renderer  Renderer;
typedef struct UIContext  UIContext;
typedef struct InputState InputState;

#endif /* COMMON_H */
