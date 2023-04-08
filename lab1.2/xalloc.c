#include "xalloc.h"

void* xmalloc(size_t size) {
    void* ptr = malloc(size);
    if (ptr) return ptr;
    fprintf(stderr, "fatal: out of memory\n");
    exit(EXIT_FAILURE);
}

void* xcalloc(size_t num, size_t size) {
    void* ptr = calloc(num, size);
    if (ptr) return ptr;
    fprintf(stderr, "fatal: out of memory\n");
    exit(EXIT_FAILURE);
}

void* xrealloc(void *ptr, size_t size) {
    ptr = realloc(ptr, size);
    if (ptr) return ptr;
    fprintf(stderr, "fatal: out of memory\n");
    exit(EXIT_FAILURE);
}
