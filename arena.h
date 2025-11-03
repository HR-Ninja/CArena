#ifndef ARENA_H
#define ARENA_H

#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

#ifndef DEFAULT_ALIGNMENT
#define DEFAULT_ALIGNMENT (2*sizeof(void *))
#endif

#define ARENA_INIT_FAILED -1
#define ARENA_INIT_PASSED  1


typedef struct Arena Arena;

struct Arena {
    unsigned char* buffer;
    size_t buffer_len;
    size_t prev_offset;
    size_t curr_offset;
};

extern int arena_init(Arena* a, size_t buffer_size);

extern void* arena_alloc(Arena* a, size_t size);
extern void* arena_alloc_align(Arena* a, size_t size, size_t alignment);
extern void* arena_resize(Arena *a, void *old_memory, size_t old_size, size_t new_size);
extern void* arena_resize_align(Arena *a, void *old_memory, size_t old_size, size_t new_size, size_t alignment);

extern void arena_rest(Arena* a);
extern void arena_free(Arena* a);

extern void* align_forward(void* ptr, size_t alignment);

#endif