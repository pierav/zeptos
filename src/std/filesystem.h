#pragma once

#include <sys/stat.h>

typedef union fnode fnode_t;

// Internal
enum _fnode_type { FNODE_D = 0, FNODE_F };

typedef struct __fnode_polymorph {
    enum _fnode_type type;
    struct stat stat;
    char *path;
    fnode_t *_next;
} __fnode_polymorph_t;

typedef struct _fnode_dir {
    __fnode_polymorph_t metadata; // must be first
    fnode_t *subs;                // dirrectory content
} _fnode_d_t;

typedef struct _fnode_file {
    __fnode_polymorph_t metadata; // must be first
    void *base;
} _fnode_f_t;

union fnode {
    __fnode_polymorph_t metadata; // must be first
    _fnode_f_t file;
    _fnode_d_t dir;
};

#define FNODE_METADATA_INIT(type, name) ((__fnode_polymorph_t){type, name, 0})

#define FNODE_F_INIT(__name, __stat, __base, __next)                           \
    (fnode_t)(_fnode_f_t) {                                                    \
        .metadata = {.type = FNODE_F,                                          \
                     .stat = __stat,                                           \
                     .path = __name,                                           \
                     ._next = __next},                                         \
        .base = __base                                                         \
    }

#define FNODE_D_INIT(__name, __stat, __subs, __next)                           \
    (fnode_t)(_fnode_d_t) {                                                    \
        .metadata = {.type = FNODE_D,                                          \
                     .stat = __stat,                                           \
                     .path = __name,                                           \
                     ._next = __next},                                         \
        .subs = __subs                                                         \
    }

int fs_init();

typedef void *(*fs_visitor_f)(const fnode_t *fn, void *arg);
void *fnode_visit(const fnode_t *fn, fs_visitor_f f, void *args);

fnode_t *fs_get_node(const char *path);
