#include <errno.h>

// For now: use 1 global errno
static int errno_internal;

int __errno;

int *__errno_location(void) { return &__errno; }
