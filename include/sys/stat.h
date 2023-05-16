#pragma once

#include <sys/types.h>

#define S_IFMT 00170000
#define S_IFSOCK 0140000
#define S_IFLNK 0120000
#define S_IFREG 0100000
#define S_IFBLK 0060000
#define S_IFDIR 0040000
#define S_IFCHR 0020000
#define S_IFIFO 0010000
#define S_ISUID 0004000
#define S_ISGID 0002000
#define S_ISVTX 0001000

#define S_ISLNK(m) (((m)&S_IFMT) == S_IFLNK)
#define S_ISREG(m) (((m)&S_IFMT) == S_IFREG)
#define S_ISDIR(m) (((m)&S_IFMT) == S_IFDIR)
#define S_ISCHR(m) (((m)&S_IFMT) == S_IFCHR)
#define S_ISBLK(m) (((m)&S_IFMT) == S_IFBLK)
#define S_ISFIFO(m) (((m)&S_IFMT) == S_IFIFO)
#define S_ISSOCK(m) (((m)&S_IFMT) == S_IFSOCK)

#define S_IRWXU 00700
#define S_IRUSR 00400
#define S_IWUSR 00200
#define S_IXUSR 00100

#define S_IRWXG 00070
#define S_IRGRP 00040
#define S_IWGRP 00020
#define S_IXGRP 00010

#define S_IRWXO 00007
#define S_IROTH 00004
#define S_IWOTH 00002
#define S_IXOTH 00001

//    dev_t st_dev            Device ID of device containing file.
//    ino_t st_ino            File serial number.
//    mode_t st_mode          Mode of file (see below).
//    nlink_t st_nlink        Number of hard links to the file.
//    uid_t st_uid            User ID of file.
//    gid_t st_gid            Group ID of file.
//    dev_t st_rdev           Device ID (if file is character or block special).
//    off_t st_size           For regular files, the file size in bytes.
//                            For symbolic links, the length in bytes of the
//                            pathname contained in the symbolic link.
//                            For a shared memory object, the length in bytes.
//                            For a typed memory object, the length in bytes.
//                            For other file types, the use of this field is
//                            unspecified.
//    struct timespec st_atim Last data access timestamp.
//    struct timespec st_mtim Last data modification timestamp.
//    struct timespec st_ctim Last file status change timestamp.
//    blksize_t st_blksize    A file system-specific preferred I/O block size
//                            for this object. In some file system types, this
//                            may vary from file to file.
//    blkcnt_t st_blocks      Number of blocks allocated for this object.

struct stat {
    dev_t st_dev;
    ino_t st_ino;
    mode_t st_mode;
    nlink_t st_nlink;
    uid_t st_uid;
    gid_t st_gid;
    dev_t st_rdev;
    off_t st_size;
    blksize_t st_blksize;
    blkcnt_t st_blocks;
    time_t st_atime;
    time_t st_mtime;
    time_t st_ctime;
};
#if 0

int stat(const char *path, struct stat *buf);
int fstat(int fd, struct stat *buf);
int lstat(const char *path, struct stat *buf);

#endif
