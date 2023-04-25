#pragma once

#include <stdio.h>
#include <stdlib.h>

#ifdef NDEBUG
#define assert(e) ((void)0)
#else

#define assert(e) ((void)((e) ? 0 : __assert(#e, __FILE__, __LINE__)))
#define __assert(e, file, line)                                                \
    ((void)printf("%s:%u: failed assertion `%s'\n", file, line, e), abort())

#endif