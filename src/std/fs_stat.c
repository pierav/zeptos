// buf must have at least 10 bytes
#include "fs_stat.h"
#include <stddef.h>

char *strmode(mode_t mode) {
    static char buf[FS_STRMODE_BUFFSIZE];
    return strmode_r(mode, buf);
}

char *strmode_r(mode_t mode, char *buf) {
    const char chars[] = "rwx";
    buf[0] = S_ISDIR(mode)
                 ? 'd'
                 : S_ISREG(mode)
                       ? 'f'
                       : S_ISCHR(mode)
                             ? 'c'
                             : S_ISBLK(mode) ? 'b' : S_ISFIFO(mode) ? 'f' : '-';
    for (size_t i = 0; i < 9; i++) {
        buf[i + 1] = (mode & (1 << (8 - i))) ? chars[i % 3] : '-';
    }
    buf[10] = '\0';
    return buf;
}

const char *path_iterator_r(const char *path, int *bufsize) {
    const char *begin = path;
    // Remove leading /
    while (*begin == '/') {
        begin++;
    }
    for (const char *end = begin, *old_end = end; *old_end != '\0';
         old_end = end, end++) {
        if (*end == '/' || *end == '\0') {
            *bufsize = end - begin + 1;
            // printf("path_iterator_r #%d %.*s\n", *bufsize, *bufsize, begin);
            return begin;
        }
    }
    return NULL;
}
