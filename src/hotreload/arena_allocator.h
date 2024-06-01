#ifndef ARENA_ALLOCATOR_H
#define ARENA_ALLOCATOR_H

#include <cstdlib>
#include <cstring>

struct ArenaAllocator {
    char* base;
    size_t capacity;
    size_t offset;

    ArenaAllocator(size_t size) {
        base = (char*)malloc(size);
        capacity = size;
        offset = 0;
    }

    ~ArenaAllocator() {
        free(base);
    }

    void* allocate(size_t size) {
        if (offset + size > capacity) {
            return nullptr;
        }
        void* ptr = base + offset;
        offset += size;
        return ptr;
    }

    void reset() {
        offset = 0;
    }
};

#endif // ARENA_ALLOCATOR_H
