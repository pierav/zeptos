#include "filesystem.h"
#include "printk.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct FILE {
    fnode_t *fn;  // File node
    uint64_t pos; // current position in file
    uint64_t openmode;
};

FILE *stdout = (FILE *)0;
FILE *stderr = (FILE *)1;
FILE *stdin = (FILE *)2;

int fprintf(FILE *stream, const char *format, ...) {
    va_list vl;
    va_start(vl, format);
    vfprintf(stream, format, vl);
    va_end(vl);
    return 0;
}

int fputc(int c, FILE *stream) {
    if (stream == stdout || stream == stderr) {
        return putchar(c);
    } else {
        // TODO write
        panic("Unimplemented\n");
        return EOF;
    }
}

int putc(int c, FILE *stream) { return fputc(c, stream); }

int vfprintf(FILE *stream, const char *format, va_list ap) {
    if (stream == stdout || stream == stderr) {
        return vprintf(format, ap);
    } else {
        // TODO write
        panic("Unimplemented\n");
        return EOF;
    }
}

FILE *fopen(const char *filename, const char *mode) {
    if (strcmp(mode, "r") != 0) { // For now only read
        errno = EINVAL;
        return NULL;
    }

    fnode_t *fn = fs_get_node(filename);
    if (!fn) {
        errno = EINVAL;
        return NULL;
    }

    if (!S_ISREG(fn->metadata.stat.st_mode)) {
        errno = EISDIR;
        return NULL;
    }

    //
    // TODO Check permissions
    // EACCES

    // Allocate file pointer
    FILE *fp = malloc(sizeof(FILE));
    if (!fp) {
        return NULL;
    }
    fp->fn = fn;
    fp->pos = 0;
    fp->openmode = 0; // TODO READONLY
    return fp;
}

int fclose(FILE *stream) {
    // TODO: more checks
    if (!stream) {
        errno = EBADF;
        return EOF;
    }
    free(stream);
    return 0;
}
int fflush(FILE *stream) { panic("Unimplemented !\n"); }

int fgetc(FILE *stream) {
    if (stream == stdout || stream == stderr) {
        return EOF;
    } else if (stream == stdin) {
        // TODO write
        panic("Unimplemented\n");
        return EOF;
    }
    _fnode_f_t *fn = (_fnode_f_t *)stream->fn;
    off_t stream_size = fn->metadata.stat.st_size;
    if (stream->pos == stream_size) {
        return EOF;
    }

    char ret = ((char *)fn->base)[stream->pos];
    stream->pos += 1;
    return ret;
}

int getc(FILE *stream) { return fgetc(stream); }

char *fgets(char *s, int size, FILE *stream) {
    // TODO: optimise with stream private attrs
    int c;
    char *cs;
    cs = s;
    while (--size > 0 && (c = fgetc(stream)) != EOF)
        if ((*cs++ = c) == '\n')
            break;
    *cs = '\0';
    return (c == EOF && cs == s) ? NULL : s;
}

int fseek(FILE *stream, long offset, int whence) {
    uint64_t nextpos;
    _fnode_f_t *fn = (_fnode_f_t *)stream->fn;
    off_t stream_size = fn->metadata.stat.st_size;
    switch (whence) {
    case SEEK_CUR:
        nextpos = stream->pos + offset;
        break;
    case SEEK_SET:
        nextpos = offset;
        break;
    case SEEK_END:
        nextpos = stream_size;
        break;
    default:
        errno = EINVAL;
        return EOF;
    }

    if (nextpos > stream_size) {
        errno = EINVAL;
        return EOF;
    }

    stream->pos = nextpos;
    return 0;
}

long ftell(FILE *stream) { return stream->pos; }

void rewind(FILE *stream) { stream->pos = 0; }

// int fgetpos(FILE *stream, fpos_t *pos);
// int fsetpos(FILE *stream, const fpos_t *pos);
