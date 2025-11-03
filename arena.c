#include "arena.h"


int arena_init(Arena* a, size_t buffer_size) 
{
    unsigned char* buff = (unsigned char*)malloc(buffer_size);
    if (buff == NULL) {
        return ARENA_INIT_FAILED;
    }

    a->buffer = buff;
    a->buffer_len = buffer_size;
	a->curr_offset = 0;
	a->prev_offset = 0;
    return ARENA_INIT_PASSED;
}

void* arena_alloc(Arena *a, size_t size)
{
    return arena_alloc_align(a, size, DEFAULT_ALIGNMENT);
}

void *arena_alloc_align(Arena *a, size_t size, size_t alignment)
{
    uint8_t* curr_ptr = (uint8_t*)a->buffer + a->curr_offset;
    uint8_t* aligned_ptr = align_forward(curr_ptr, alignment);
    size_t offset = (size_t)(aligned_ptr - (uint8_t *)a->buffer);

    if (offset + size > a->buffer_len) {
        return NULL;
    }

    void* ptr = &a->buffer[offset];
    a->prev_offset = offset;
    a->curr_offset = offset + size;

    memset(ptr, 0, size);
    return ptr;
}

void* arena_resize(Arena *a, void *old_memory, size_t old_size, size_t new_size)
{
    return arena_resize_align(a, old_memory, old_size, new_size, DEFAULT_ALIGNMENT);;
}

void* arena_resize_align(Arena *a, void *old_memory, size_t old_size, size_t new_size, size_t alignment)
{
    assert((alignment & (alignment-1)) == 0);

    uint8_t* old_mem = (uint8_t*)old_memory; 
    if (old_mem == NULL || old_size == 0) {
		return arena_alloc_align(a, new_size, alignment);
	}
    
    if (old_mem < a->buffer || old_mem >= a->buffer + a->buffer_len) {
        assert(0 && "Memory is out of bounds of the buffer in this arena");
		return NULL;
    }

    if (a->buffer + a->prev_offset == old_mem) {
        a->curr_offset = a->prev_offset + new_size;
        if (new_size > old_size) {
            memset(&a->buffer[a->prev_offset + old_size], 0, new_size - old_size);
        }
        return old_memory;
    }
    
    void* new_memory = arena_alloc_align(a, new_size, alignment);
    size_t copy_size = old_size < new_size ? old_size : new_size;
    memmove(new_memory, old_memory, copy_size);
    return new_memory;
}

void arena_reset(Arena* a) 
{
    memset(a->buffer, 0, a->buffer_len);
    a->prev_offset = 0;
    a->curr_offset = 0;
}

void arena_free(Arena* a) 
{
    free(a->buffer);
    a->buffer_len = 0;
    a->prev_offset = 0;
    a->curr_offset = 0;
}

void* align_forward(void* ptr, size_t alignment) 
{
    assert((alignment & (alignment-1)) == 0);
    
    uintptr_t p = (uintptr_t)ptr;
    uintptr_t a = (uintptr_t)alignment;
    uintptr_t modulo = p & (a - 1);

    if (modulo != 0) {
        p += a - modulo;
    }

    return (void*)p;
}
