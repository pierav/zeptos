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

int feof(FILE *stream) {
    panic("Unimplemented\n");
    return 0;
}

int fprintf(FILE *stream, const char *format, ...) {
    va_list vl;
    va_start(vl, format);
    int ret = vfprintf(stream, format, vl);
    va_end(vl);
    return ret;
}

int fscanf(FILE *stream, const char *format, ...) {
    va_list args;
    va_start(args, format);
    int i = vfscanf(stream, format, args);
    va_end(args);
    return i;
}

int vfscanf(FILE *stream, const char *format, va_list ap) {
    if (stream == stdout || stream == stderr) {
        return EOF;
    }
    if (stream == stdin) {
        panic("Unimplemented\n");
        return EOF;
    }
    _fnode_f_t *fn = (_fnode_f_t *)stream->fn;
    // off_t stream_size = fn->metadata.stat.st_size;
    // if (stream->pos == stream_size) {
    //     return EOF;
    // }
    char *buf = &((char *)fn->base)[stream->pos];
    char *endptr;
    int delta = vsscanf_internal(buf, format, ap, &endptr);
    stream->pos += buf - endptr;
    return delta;
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

    // TODO open()
    fnode_t *fn = fs_get_node(filename);
    if (!fn) {
        errno = EINVAL;
        return NULL;
    }

    if (!S_ISREG(fn->metadata.stat.st_mode)) {
        errno = EISDIR;
        printk("Failure open %s: EISDIR\n", filename);
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
int fflush(FILE *stream) {
    // We only have unbuffered streams
    return 0;
}

int _fgetc(FILE *stream) {
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

int fgetc(FILE *stream) {
    int ret = _fgetc(stream);
    // printk("(%x) -> %c\n", stream, ret);
    return ret;
}

int getc(FILE *stream) { return fgetc(stream); }

char *fgets(char *s, int size, FILE *stream) {
    // TODO: optimise with stream private attrs
    int c;
    char *cs;
    cs = s;
    while (--size > 0 && (c = _fgetc(stream)) != EOF)
        if ((*cs++ = c) == '\n')
            break;
    *cs = '\0';
    char *ret = (c == EOF && cs == s) ? NULL : s;
    printk("%x #%d -> %s\n", stream, size, ret);
    return ret;
}

int fputs(const char *s, FILE *stream) {
    panic("Unimplemented\n");
    return 0;
}

_fnode_f_t *_file_check_readeable(FILE *stream) {
    _fnode_f_t *fn = (_fnode_f_t *)stream->fn;
    return fn;
}

#define MIN(a, b) ((a) < (b) ? (a) : (b))

size_t fread(void *ptr, size_t size, size_t nmemb, FILE *stream) {
    _fnode_f_t *fn = _file_check_readeable(stream);
    if (!fn) {
        return 0;
    }
    off_t stream_size = fn->metadata.stat.st_size;
    off_t stream_pos = stream->pos;

    off_t delta_bytes = stream_size - stream_pos;
    off_t delta_block = delta_bytes / size;

    // Max block
    off_t eff_delta_block = MIN(delta_block, nmemb);
    off_t eff_delta_bytes = eff_delta_block * size;

    // Block read and copy
    char *cur_base = &((char *)fn->base)[stream->pos];
    stream->pos += eff_delta_bytes;
    memcpy(ptr, cur_base, eff_delta_bytes);

    size_t ret = eff_delta_bytes;
    printk("(%x, %d, %d, %x) -> %d\n", ptr, size, nmemb, stream, ret);
    return ret;
}

size_t fwrite(const void *ptr, size_t size, size_t nmemb, FILE *stream) {
    panic("Unimplemented\n");
    return 0;
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

void rewind(FILE *stream) {
    stream->pos = 0;
    printk("(%x)\n", stream);
}

// int fgetpos(FILE *stream, fpos_t *pos);
// int fsetpos(FILE *stream, const fpos_t *pos);

int rename(const char *old, const char *new) {
    panic("Unimplemented\n");
    return 0;
}

int unlink(const char *path) {
    panic("Unimplemented\n");
    return 0;
}
