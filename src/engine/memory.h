/*******************************************************************************
 * Silicon Legacy — Memory Management
 * Arena allocator for efficient, leak-free memory usage
 ******************************************************************************/
#ifndef ENGINE_MEMORY_H
#define ENGINE_MEMORY_H

#include "common.h"

/* ── Arena Allocator ─────────────────────────────────────────────────────── */
typedef struct {
    u8*    base;
    size_t capacity;
    size_t used;
} MemArena;

MemArena* arena_create(size_t capacity);
void      arena_destroy(MemArena* a);
void*     arena_alloc(MemArena* a, size_t bytes);
void      arena_reset(MemArena* a);
size_t    arena_remaining(const MemArena* a);

#endif /* ENGINE_MEMORY_H */
