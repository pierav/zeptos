#pragma once

#include <stdint.h>
#include <sys/stat.h>

/*

F_DUPFD           Duplicate file descriptor.
F_DUPFD_CLOEXEC   Duplicate file descriptor with the close-on-exec flag
FD_CLOEXEC set. F_GETFD           Get file descriptor flags. F_SETFD Set file
descriptor flags. F_GETFL           Get file status flags and file access modes.
F_SETFL           Set file status flags.
F_GETLK           Get record locking information.
F_SETLK           Set record locking information.
F_SETLKW          Set record locking information; wait if blocked.
F_GETOWN          Get process or process group ID to receive SIGURG signals.
F_SETOWN          Set process or process group ID to receive SIGURG signals.

FD_CLOEXEC        Close the file descriptor upon execution of an exec family
function.

F_RDLCK           Shared or read lock.
F_UNLCK           Unlock.
F_WRLCK           Exclusive or write lock.

O_CLOEXEC   The FD_CLOEXEC flag associated with the new descriptor shall be set
to close the file descriptor upon execution of an exec family function. O_CREAT
Create file if it does not exist. O_DIRECTORY Fail if not a directory. O_EXCL
Exclusive use flag. O_NOCTTY    Do not assign controlling terminal. O_NOFOLLOW
Do not follow symbolic links. O_TRUNC     Truncate flag. O_TTY_INIT  Set the
termios structure terminal parameters to a state that provides conforming
behavior.

O_APPEND    Set append mode.
O_DSYNC     Write according to synchronized I/O data integrity completion.
O_NONBLOCK  Non-blocking mode.
O_RSYNC     Synchronized read I/O operations.
O_SYNC      Write according to synchronized I/O file integrity completion.
O_ACCMODE   Mask for file access modes.

O_EXEC      Open for execute only (non-directory files). The result is
unspecified if this flag is applied to a directory. O_RDONLY    Open for reading
only. O_RDWR      Open for reading and writing. O_SEARCH    Open directory for
search only. The result is unspecified if this flag is applied to a
non-directory file. O_WRONLY    Open for writing only.

AT_FDCWD    Use the current working directory to determine the target of
relative file paths. AT_EACCESS  Check access using effective user and group ID.
AT_SYMLINK_NOFOLLOW Do not follow symbolic links.
AT_SYMLINK_FOLLOW   Follow symbolic link.
AT_REMOVEDIR        Remove directory instead of file.
*/

#include <sys/types.h>

/* File access modes for `open' and `fcntl'.  */
#define O_RDONLY 0 /* Open read-only.  */
#define O_WRONLY 1 /* Open write-only.  */
#define O_RDWR 2   /* Open read/write.  */
/* Bits OR'd into the second argument to open.  */
#define O_CREAT 0x0200  /* Create file if it doesn't exist.  */
#define O_EXCL 0x0800   /* Fail if file already exists.  */
#define O_TRUNC 0x0400  /* Truncate file to zero length.  */
#define O_NOCTTY 0x8000 /* Don't assign a controlling terminal.  */
#define O_ASYNC 0x0040  /* Send SIGIO to owner when data is ready.  */
#define O_FSYNC 0x0080  /* Synchronous writes.  */
#define O_SYNC O_FSYNC
#define O_SHLOCK 0x0010        /* Open with shared file lock.  */
#define O_EXLOCK 0x0020        /* Open with shared exclusive lock.  */
#define O_DIRECTORY 0x00200000 /* Must be a directory.	 */
#define O_NOFOLLOW 0x00000100  /* Do not follow links.	 */
#define O_CLOEXEC 0x00400000   /* Set close_on_exec.  */
#define O_DSYNC 0x00010000     /* Synchronize data.  */
#define O_RSYNC 0x00020000     /* Synchronize read operations.	 */
#define O_LARGEFILE 0
/* File status flags for `open' and `fcntl'.  */
#define O_APPEND 0x0008   /* Writes append to the file.  */
#define O_NONBLOCK 0x0004 /* Non-blocking I/O.  */
#define O_NDELAY O_NONBLOCK

/* Flags for TIOCFLUSH.  */
#define FREAD 1
#define FWRITE 2
/* Traditional BSD names the O_* bits.  */
#define FASYNC O_ASYNC
#define FFSYNC O_FSYNC
#define FSYNC O_SYNC
#define FAPPEND O_APPEND
#define FNDELAY O_NDELAY

/* Mask for file access modes.  This is system-dependent in case
   some system ever wants to define some other flavor of access.  */
#define O_ACCMODE (O_RDONLY | O_WRONLY | O_RDWR)
/* Values for the second argument to `fcntl'.  */
#define F_DUPFD 0 /* Duplicate file descriptor.  */
#define F_GETFD 1 /* Get file descriptor flags.  */
#define F_SETFD 2 /* Set file descriptor flags.  */
#define F_GETFL 3 /* Get file status flags.  */
#define F_SETFL 4 /* Set file status flags.  */

#define F_GETLK 7  /* Get record locking info.  */
#define F_SETLK 8  /* Set record locking info (non-blocking).  */
#define F_SETLKW 9 /* Set record locking info (blocking).  */

/* File descriptor flags used with F_GETFD and F_SETFD.  */
#define FD_CLOEXEC 1 /* Close on exec.  */

/* Values for the `l_type' field of a `struct flock'.  */
#define F_RDLCK 1 /* Read lock.  */
#define F_WRLCK 2 /* Write lock.  */
#define F_UNLCK 3 /* Remove lock.  */

#define POSIX_FADV_NORMAL 0     /* No further special treatment.  */
#define POSIX_FADV_RANDOM 1     /* Expect random page references.  */
#define POSIX_FADV_SEQUENTIAL 2 /* Expect sequential page references.  */
#define POSIX_FADV_WILLNEED 3   /* Will need these pages.  */
#define POSIX_FADV_DONTNEED 4   /* Don't need these pages.  */
#define POSIX_FADV_NOREUSE 5    /* Data will be accessed once.  */

struct flock {
    short l_type;   // Type of lock; F_RDLCK, F_WRLCK, F_UNLCK.
    short l_whence; // Flag for starting offset.
    off_t l_start;  // Relative offset in bytes.
    off_t l_len;    // Size; if 0 then until EOF.
    pid_t l_pid;    // Process ID of the process holding the lock; returned with
                    // F_GETLK.
};

int creat(const char *, mode_t);
int fcntl(int, int, ...);
int open(const char *, int, ...);
int openat(int, const char *, int, ...);
int posix_fadvise(int, off_t, off_t, int);
int posix_fallocate(int, off_t, off_t);
