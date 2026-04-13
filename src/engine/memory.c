/*******************************************************************************
 * Silicon Legacy — Memory Management (Implementation)
 ******************************************************************************/
#include "engine/memory.h"

MemArena* arena_create(size_t capacity) {
    MemArena* a = (MemArena*)malloc(sizeof(MemArena));
    if (!a) return NULL;
    a->base = (u8*)malloc(capacity);
    if (!a->base) { free(a); return NULL; }
    a->capacity = capacity;
    a->used = 0;
    memset(a->base, 0, capacity);
    return a;
}

void arena_destroy(MemArena* a) {
    if (a) {
        free(a->base);
        free(a);
    }
}

void* arena_alloc(MemArena* a, size_t bytes) {
    /* Align to 8 bytes */
    size_t aligned = (bytes + 7) & ~(size_t)7;
    if (a->used + aligned > a->capacity) {
        fprintf(stderr, "[MEMORY] Arena overflow! Requested %zu, remaining %zu\n",
                bytes, a->capacity - a->used);
        return NULL;
    }
    void* ptr = a->base + a->used;
    a->used += aligned;
    return ptr;
}

void arena_reset(MemArena* a) {
    a->used = 0;
}

size_t arena_remaining(const MemArena* a) {
    return a->capacity - a->used;
}
