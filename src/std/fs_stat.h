#include <sys/stat.h>

#define FS_STRMODE_BUFFSIZE 11

char *strmode_r(mode_t mode, char *buf);
char *strmode(mode_t mode);

const char *path_iterator_r(const char *path, int *bufsize);
