#include <limits.h>
#include <stdint.h>

static uint64_t next = 1;

int rand() {
    return ((next = next * 1103515245 + 12345) % ((uint64_t)ULONG_MAX + 1));
}

void srand(uint64_t seed) { next = seed; }
