// * Lets create a minimal ram filesystem *

#include "filesystem.h"
#include "printk.h"
#include <assert.h>
#include <errno.h>
#include <stdarg.h> // va_list
#include <stddef.h> // size_t
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#define BUFSIZ 1024 /* size of buffer used by setbuf */
#define EOF (-1)

#define FOPEN_MAX 20
#define FILENAME_MAX 1024
#define TMP_MAX 1 /* todo */

#define SEEK_SET 0 /* set file offset to offset */
#define SEEK_CUR 1 /* set file offset to current plus offset */
#define SEEK_END 2 /* set file offset to EOF plus offset */

typedef struct FILE FILE;

struct FILE {
    uint64_t id; // Unique ID
    void *base;  // Memory base address

    char *filename; //
    uint64_t size;  // File size
    uint64_t pos;   // current position in file
};

FILE *stdout = NULL;
FILE *stderr = NULL;
FILE *stdin = NULL;

int fprintf(FILE *stream, const char *format, ...) {
    va_list vl;
    va_start(vl, format);
    vprintf(format, vl);
    va_end(vl);
    return 0;
}

int fputc(int c, FILE *stream) { return putchar(c); }

int putc(int c, FILE *stream) { return putchar(c); }

int vfprintf(FILE *stream, const char *format, va_list ap) {
    return vprintf(format, ap);
}

FILE *fopen(const char *filename, const char *mode) { return NULL; }
int fclose(FILE *stream) { return 0; }
int fflush(FILE *stream) { return 0; }

int fgetc(FILE *stream) { return EOF; }

FILE *fopen(const char *restrict pathname, const char *restrict mode);
int fclose(FILE *stream);

int fseek(FILE *stream, long offset, int whence) {
    uint64_t nextpos;
    switch (whence) {
    case SEEK_CUR:
        nextpos = stream->pos + offset;
        break;
    case SEEK_SET:
        nextpos = offset;
        break;
    case SEEK_END:
        nextpos = stream->size;
        break;
    default:
        errno = EINVAL;
        return EOF;
    }

    if (nextpos > stream->size) {
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

// External
// TODO: generic
// 16 MB for filesystem
#define FS_MEM_CHUNCK_SIZE 16 * 1024 * 1024
static uint8_t _mem_chunck[FS_MEM_CHUNCK_SIZE];
// TODO: generic
#define STRING_TABLE_SIZE 2048
#define FNODE_TABLE_SIZE 64

static char _string_table[STRING_TABLE_SIZE];
static uint32_t _string_table_indx = 0;

// Return the allocated string in the string table
// we must allocate size + 1 for '\0'
char *_string_table_allocate(char *str, size_t size) {
    if (_string_table_indx + size + 1 >= STRING_TABLE_SIZE) {
        panic("_string_table overflow\n");
    }
    char *ret = _string_table + _string_table_indx;
    memcpy(ret, str, size);
    ret[size] = '\0';
    _string_table_indx += size + 1;
    return ret;
}

// Root is _fnode_table[0]:
fnode_t _fnode_table[FNODE_TABLE_SIZE];
fnode_t *root = &_fnode_table[0];
size_t _fnode_table_index = 0;

fnode_t *_fnode_table_allocate() {
    if (_fnode_table_index >= FNODE_TABLE_SIZE) {
        panic("_fnode_table overflow\n");
    }
    return &_fnode_table[_fnode_table_index++];
}

// Use visitor

void *fnode_f_visit(const _fnode_f_t *fnf, fs_visitor_f f, void *args) {
    return args;
}

void *fnode_d_visit(const _fnode_d_t *fnd, fs_visitor_f f, void *args) {
    void *ret = args;
    for (fnode_t *fni = fnd->subs; fni != NULL; fni = fni->metadata._next) {
        ret = fnode_visit(fni, f, args);
    }
    return ret;
}

void *fnode_visit(const fnode_t *fn, fs_visitor_f f, void *args) {
    f(fn, args);
    switch (fn->metadata.type) {
    case FNODE_D:
        return fnode_d_visit((const _fnode_d_t *)fn, f, args);
    case FNODE_F:
        return fnode_f_visit((const _fnode_f_t *)fn, f, args);
    }

    return args;
}

#include <string.h>

char *_fnode_type_str[] = {"FNODE_D", "FNODE_F"};

void _fs_dump(const fnode_t *fn, void *args) {
    char *base_path = args;
    // append cur path to base path
    size_t size = strlen(args);
    strcat(args, " | ");
    strcat(args, fn->metadata.path);
    printf("fnode %x [%s]: %s\n", fn, _fnode_type_str[fn->metadata.type], args);
    switch (fn->metadata.type) {
    case FNODE_D: {
        for (fnode_t *fni = ((const _fnode_d_t *)fn)->subs; fni != NULL;
             fni = fni->metadata._next) {
            _fs_dump(fni, args);
        }
    } break;
    case FNODE_F:
        break;
    }

    // remove cur path to base path
    *((char *)args + size) = '\0';
}

void fs_tree(const fnode_t *fn) {
    char local_path[FILENAME_MAX];
    _fs_dump(fn, local_path);
}

fnode_t *fs_get_node(char *path) {
    fnode_t *cur = NULL, *next = root;
    char *begin = path;
    for (char *end = begin, *old_end = end; *old_end != '\0';
         old_end = end, end++) {
        if (*end == '/' || *end == '\0') {
            // Begin
            if (!next) {
                return NULL;
            }

            int size = end - begin + 1;
            // if(!cur->metadata.type != FNODE_D){
            //     return NULL;
            // }
            // Iterate over cur subs
            // fnode_t *s = ((_fnode_d_t*)cur)->subs;
            cur = next;
            // next = cur;

            while (next) {
                // printf("%.*s == %s ?\n", size, begin, next->metadata.path);
                if (memcmp(begin, next->metadata.path, size) == 0) {
                    next = ((_fnode_d_t *)next)->subs;
                    break;
                }
                next = next->metadata._next;
            }

            // End
            begin = end + 1;
        }
    }

    return cur;
}

/**
 * @brief Create fnode dir path at *cur
 *
 * @param path path string
 * @param cur root ptr of the generated fnode path
 * @return fnode_t*
 */
fnode_t *_fs_create_node_indep(char *path, fnode_t **cur) {
    fnode_t *new = NULL;

    assert(*cur == NULL);
    // printk("Create path %s \n", path);
    char *begin = path;
    for (char *end = begin, *old_end = end; *old_end != '\0';
         old_end = end, end++) {
        if ((*end == '/' || *end == '\0') && begin != end) {
            // Begin
            int size = end - begin + 1;

            // Create new node
            new = _fnode_table_allocate();
            char *name = _string_table_allocate(begin, size);
            *new = FNODE_D_INIT(name, NULL, NULL);
            // printf("Allocate %s\n", name);

            *cur = new;
            cur = &((_fnode_d_t *)(*cur))->subs;

            // End
            begin = end + 1;
        }
    }

    return new;
}

fnode_t *fs_create_node(char *path, fnode_t *_root) {
    fnode_t *next = _root;
    printk("%s\n", path);
    char *begin = path;
    for (char *end = begin, *old_end = end; *old_end != '\0';
         old_end = end, end++) {
        if (*end == '/' || *end == '\0') {
            // Begin
            int size = end - begin + 1;
            fnode_t *next_test;
            while (next) {
                // printf("%.*s == %s ? #%d\n", size, begin,
                // next->metadata.path, size);
                if (memcmp(begin, next->metadata.path, size) == 0) {
                    next_test = ((_fnode_d_t *)next)->subs;
                    if (!next_test) {
                        return _fs_create_node_indep(
                            begin + size, &((_fnode_d_t *)next)->subs);
                    }
                    next = next_test;
                    break;
                }
                next_test = next->metadata._next;
                if (!next_test) {
                    return _fs_create_node_indep(begin, &next->metadata._next);
                }

                next = next_test;
            }

            // End
            begin = end + 1;
        }
    }

    return NULL; // Node already exists
}

// The default cpio ramfs
extern char _binary_ramfs_cpio_start;
extern char _binary_ramfs_cpio_end;
// extern int* _binary_ramfs_cpio_size;

#include "cpio.h"

char *path_fixup(char *path, char *buff) {
    if (path[0] != '/') {
        sprintf(buff, "/%s", path);
    }
    return buff;
}

char *path_filename(char *path) {
    char *res = strrchr(path, '/');
    if (res) {
        return path + 1;
    }
    return path;
}

fnode_t *_fs_node_file_from_cpio(const struct cpio_header *header,
                                 struct cpio_header_info *header_info,
                                 fnode_t *root) {
#define PARSE_HEX(s) parse_hex_str(s, sizeof(s))

    ino_t st_ino = (ino_t)PARSE_HEX(header->c_ino);
    mode_t st_mode = (mode_t)PARSE_HEX(header->c_mode);
    nlink_t st_nlink = (nlink_t)PARSE_HEX(header->c_nlink);
    time_t st_mtime = (time_t)PARSE_HEX(header->c_mtime);

    char path[FILENAME_MAX];
    path_fixup(header_info->filename, path);
    char *filename = path_filename(path);

    // printk(" path = %s, filename = %s\n", path, filename);
    struct stat mstat = {.st_dev = 0,
                         .st_ino = st_ino,
                         .st_mode = st_mode,
                         .st_nlink = st_nlink,
                         .st_uid = (uid_t)0,
                         .st_gid = (gid_t)0,
                         .st_rdev = (dev_t)0,
                         .st_size = (off_t)header_info->filesize,
                         .st_blksize = (blksize_t)header_info->filesize,
                         .st_blocks = (blkcnt_t)1,
                         .st_atime = (time_t)0,
                         .st_mtime = st_mtime,
                         .st_ctime = (time_t)0};

    fnode_t *fn = fs_create_node(path, root);
    if (!fn) {
        panic("Cannot create %s\n", path);
    }

    if (S_ISDIR(mstat.st_mode)) {
        // Default is DIR
    } else if (S_ISREG(mstat.st_mode)) {
        // Initialise FILE
        *fn = FNODE_F_INIT(fn->metadata.path, mstat, header_info->data, NULL);
    } else {
        panic("Invalid file type mstat.st_mode=%x\n", mstat.st_mode);
    }
    return fn;
}

int fs_init() {
    // Setup root
    root = _fnode_table_allocate();
    *root = FNODE_D_INIT("/", NULL, NULL);

    // Create few nodes. By default nodes are dirs
    fs_create_node("/dev/", root);
    fs_create_node("/tmp/", root);
    fnode_t *fn = fs_create_node("/etc/hostname", root);
    *fn = FNODE_F_INIT("hostname", {}, NULL, NULL);

    // print tree tree
    fs_tree(root);

    fnode_t *hostname = fs_get_node("/etc/hostname");
    if (!hostname) {
        panic("Missing /etc/hostname\n");
    }
    if (hostname->metadata.type != FNODE_F) {
        panic("Bad file /etc/hostname\n");
    }
    printk("Hit hostname: %s\n", hostname->metadata.path);

    // Initialise CPIO
    char *archive = &_binary_ramfs_cpio_start;
    size_t len = &_binary_ramfs_cpio_end - &_binary_ramfs_cpio_start;
    printk("CPIO @ [%x: %x] #%x\n", &_binary_ramfs_cpio_start,
           &_binary_ramfs_cpio_end, len);

    // Parse CPIO infos
    struct cpio_info info;
    if (cpio_info(archive, len, &info) != 0) {
        panic("Invalid CPIO info\n");
    }
    printk("CPIO contains %d files, max_path = %d\n", info.file_count,
           info.max_path_sz);

    // Parse CPIO files metadata
    const struct cpio_header *header = (struct cpio_header *)archive;
    size_t cur_len = len;
    struct cpio_header_info header_info;
    while (cpio_parse_header(header, cur_len, &header_info) == 0) {
        printk("CPIO hit : %s #%d @%p\n", header_info.filename,
               header_info.filesize, header_info.data);
        _fs_node_file_from_cpio(header, &header_info, root);

        cur_len = cpio_len_next(cur_len, header, header_info.next);
        header = header_info.next;
    }
    fs_tree(root);

    return 0;
}
