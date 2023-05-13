#pragma once

#include <stdint.h>

typedef int __unimplemented_t;

typedef __unimplemented_t blkcnt_t;
// Used for file block counts.

typedef __unimplemented_t blksize_t;
// Used for block sizes.

typedef __unimplemented_t clock_t;
// Used for system times in clock ticks or CLOCKS_PER_SEC; see <time.h> .

typedef __unimplemented_t clockid_t;
// Used for clock ID type in the clock and timer functions.

typedef __unimplemented_t dev_t;
// Used for device IDs.

typedef __unimplemented_t fsblkcnt_t;
// Used for file system block counts.

typedef __unimplemented_t fsfilcnt_t;
// Used for file system file counts

typedef __unimplemented_t gid_t;
//  Used for group IDs.

typedef __unimplemented_t id_t;
// Used  as  a  general identifier; can be used to contain
// at least a pid_t, uid_t, or gid_t.

typedef __unimplemented_t ino_t;
// Used for file serial numbers.

typedef __unimplemented_t key_t; //
// Used for XSI interprocess communication.

typedef __unimplemented_t mode_t;
// Used for some file attributes.

typedef __unimplemented_t nlink_t;
// Used for link counts.

typedef __unimplemented_t off_t;
//  Used for file sizes.

// TODO: uintmax_t
typedef __unimplemented_t pid_t;
// Used for process IDs and process group IDs.

typedef int pthread_t;
typedef struct pthread_attr pthread_attr_t;
typedef struct pthread_mutex pthread_mutex_t;
typedef struct pthread_mutexattr pthread_mutexattr_t;
typedef struct pthread_cond pthread_cond_t;
typedef struct pthread_condattr pthread_condattr_t;
typedef struct pthread_once pthread_once_t;
typedef struct pthread_rwlock pthread_rwlock_t;
typedef struct pthread_rwlockattr pthread_rwlockattr_t;
typedef struct pthread_spinlock pthread_spinlock_t;
typedef struct pthread_barrier pthread_barrier_t;
typedef struct pthread_barrierattr pthread_barrierattr_t;
typedef struct pthread_key pthread_key_t;

// size_t
// ssize_t
// suseconds_t
// ?
typedef __SIZE_TYPE__ ssize_t;

typedef long long time_t;
// time_t Used for time in seconds. Seconds since the Epoch.

// timer_t
// trace_attr_t
// trace_event_id_t
// trace_event_set_t
// trace_id_t

typedef intmax_t uid_t;
// useconds_t

// Additionally:
// * mode_t shall be an integer type.
// * nlink_t, uid_t, gid_t, and id_t shall be integer types.
// * blkcnt_t and off_t shall be signed integer types.
// * fsblkcnt_t, fsfilcnt_t,   and ino_t shall be defined as unsigned integer
// types.
// * size_t shall be an unsigned integer type.
// * blksize_t, pid_t, and ssize_t shall be signed integer types.
// * time_t and clock_t shall be integer or real-floating types.
