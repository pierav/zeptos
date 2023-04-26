#pragma once
#include <limits.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>

/*
 * Run-time invariant values:
 */
#define PTHREAD_DESTRUCTOR_ITERATIONS 4
#define PTHREAD_KEYS_MAX 256
#define PTHREAD_STACK_MIN (2048)
#define PTHREAD_THREADS_MAX ULONG_MAX

/*
 * Flags for threads and thread attributes.
 */
#define PTHREAD_DETACHED 0x1
#define PTHREAD_SCOPE_SYSTEM 0x2
#define PTHREAD_INHERIT_SCHED 0x4
#define PTHREAD_NOFLOAT 0x8

#define PTHREAD_CREATE_DETACHED PTHREAD_DETACHED
#define PTHREAD_CREATE_JOINABLE 0
#define PTHREAD_SCOPE_PROCESS 0
#define PTHREAD_EXPLICIT_SCHED 0

/*
 * Flags for read/write lock attributes
 */
#define PTHREAD_PROCESS_PRIVATE 0
#define PTHREAD_PROCESS_SHARED 1

/*
 * Flags for cancelling threads
 */
#define PTHREAD_CANCEL_ENABLE 0
#define PTHREAD_CANCEL_DISABLE 1
#define PTHREAD_CANCEL_DEFERRED 0
#define PTHREAD_CANCEL_ASYNCHRONOUS 2
#define PTHREAD_CANCELED ((void *)1)

/*
 * Barrier flags
 */
#define PTHREAD_BARRIER_SERIAL_THREAD -1

// Implemented types

/*** pthread_t  ***/
typedef int pthread_t;
typedef struct pthread_attr {
    int unused;
} pthread_attr_t;

/*** pthread_mutex_t  ***/
#define PTHREAD_MUTEX_INITIALIZER                                              \
    { 0 }
typedef struct pthread_mutex {
    int counter;
} pthread_mutex_t;
typedef struct pthread_mutexattr {
    int unused;
} pthread_mutexattr_t;

/*** pthread_cond_t  ***/
typedef struct pthread_cond {
    uint64_t users;
} pthread_cond_t;
typedef struct pthread_condattr {
    int unused;
} pthread_condattr_t;

// static inline int atomic_xchg(atomic_t *v, int n) {
//     register int c;

//     __asm__ __volatile__("amoswap.w.aqrl %0, %2, %1"
//                          : "=r"(c), "+A"(v->counter)
//                          : "r"(n));
//     return c;
// }

// static inline void mb(void) {
//     // __asm__ __volatile__ ("fence");
// }

// void get_lock(atomic_t *lock) {
//     while (atomic_xchg(lock, 1) == 1)
//         ;
//     mb();
// }

// void put_lock(atomic_t *lock) {
//     mb();
//     atomic_xchg(lock, 0);
// }

// Unimplemented
struct sched_param {
    int unimplemented;
};
typedef struct pthread_once pthread_once_t;

typedef struct pthread_rwlock pthread_rwlock_t;
typedef struct pthread_rwlockattr pthread_rwlockattr_t;

typedef struct pthread_spinlock pthread_spinlock_t;

typedef struct pthread_barrier pthread_barrier_t;
typedef struct pthread_barrierattr pthread_barrierattr_t;

typedef struct pthread_key pthread_key_t;
typedef int clockid_t;

int pthread_create(pthread_t *__restrict, const pthread_attr_t *__restrict,
                   void *(*)(void *), void *__restrict);
int pthread_detach(pthread_t);
void pthread_exit(void *);
int pthread_join(pthread_t, void **);

pthread_t pthread_self(void);

int pthread_equal(pthread_t, pthread_t);
#define pthread_equal(x, y) ((x) == (y))

int pthread_setcancelstate(int, int *);
int pthread_setcanceltype(int, int *);
void pthread_testcancel(void);
int pthread_cancel(pthread_t);

int pthread_getschedparam(pthread_t, int *__restrict,
                          struct sched_param *__restrict);
int pthread_setschedparam(pthread_t, int, const struct sched_param *);
int pthread_setschedprio(pthread_t, int);

int pthread_once(pthread_once_t *, void (*)(void));

int pthread_mutex_init(pthread_mutex_t *__restrict,
                       const pthread_mutexattr_t *__restrict);
int pthread_mutex_lock(pthread_mutex_t *);
int pthread_mutex_unlock(pthread_mutex_t *);
int pthread_mutex_trylock(pthread_mutex_t *);
int pthread_mutex_timedlock(pthread_mutex_t *__restrict,
                            const struct timespec *__restrict);
int pthread_mutex_destroy(pthread_mutex_t *);
int pthread_mutex_consistent(pthread_mutex_t *);

int pthread_mutex_getprioceiling(const pthread_mutex_t *__restrict,
                                 int *__restrict);
int pthread_mutex_setprioceiling(pthread_mutex_t *__restrict, int,
                                 int *__restrict);

int pthread_cond_init(pthread_cond_t *__restrict,
                      const pthread_condattr_t *__restrict);
int pthread_cond_destroy(pthread_cond_t *);
int pthread_cond_wait(pthread_cond_t *__restrict, pthread_mutex_t *__restrict);
int pthread_cond_timedwait(pthread_cond_t *__restrict,
                           pthread_mutex_t *__restrict,
                           const struct timespec *__restrict);
int pthread_cond_broadcast(pthread_cond_t *);
int pthread_cond_signal(pthread_cond_t *);

int pthread_rwlock_init(pthread_rwlock_t *__restrict,
                        const pthread_rwlockattr_t *__restrict);
int pthread_rwlock_destroy(pthread_rwlock_t *);
int pthread_rwlock_rdlock(pthread_rwlock_t *);
int pthread_rwlock_tryrdlock(pthread_rwlock_t *);
int pthread_rwlock_timedrdlock(pthread_rwlock_t *__restrict,
                               const struct timespec *__restrict);
int pthread_rwlock_wrlock(pthread_rwlock_t *);
int pthread_rwlock_trywrlock(pthread_rwlock_t *);
int pthread_rwlock_timedwrlock(pthread_rwlock_t *__restrict,
                               const struct timespec *__restrict);
int pthread_rwlock_unlock(pthread_rwlock_t *);

int pthread_spin_init(pthread_spinlock_t *, int);
int pthread_spin_destroy(pthread_spinlock_t *);
int pthread_spin_lock(pthread_spinlock_t *);
int pthread_spin_trylock(pthread_spinlock_t *);
int pthread_spin_unlock(pthread_spinlock_t *);

int pthread_barrier_init(pthread_barrier_t *__restrict,
                         const pthread_barrierattr_t *__restrict, unsigned);
int pthread_barrier_destroy(pthread_barrier_t *);
int pthread_barrier_wait(pthread_barrier_t *);

int pthread_key_create(pthread_key_t *, void (*)(void *));
int pthread_key_delete(pthread_key_t);
void *pthread_getspecific(pthread_key_t);
int pthread_setspecific(pthread_key_t, const void *);

int pthread_attr_init(pthread_attr_t *);
int pthread_attr_destroy(pthread_attr_t *);

int pthread_attr_getguardsize(const pthread_attr_t *__restrict,
                              size_t *__restrict);
int pthread_attr_setguardsize(pthread_attr_t *, size_t);
int pthread_attr_getstacksize(const pthread_attr_t *__restrict,
                              size_t *__restrict);
int pthread_attr_setstacksize(pthread_attr_t *, size_t);

int pthread_attr_setdetachstate(pthread_attr_t *attr, int detachstate);
int pthread_attr_getdetachstate(const pthread_attr_t *attr, int *detachstate);

int pthread_attr_getstack(const pthread_attr_t *__restrict, void **__restrict,
                          size_t *__restrict);
int pthread_attr_setstack(pthread_attr_t *, void *, size_t);
int pthread_attr_getscope(const pthread_attr_t *__restrict, int *__restrict);
int pthread_attr_setscope(pthread_attr_t *, int);
int pthread_attr_getschedpolicy(const pthread_attr_t *__restrict,
                                int *__restrict);
int pthread_attr_setschedpolicy(pthread_attr_t *, int);
int pthread_attr_getschedparam(const pthread_attr_t *__restrict,
                               struct sched_param *__restrict);
int pthread_attr_setschedparam(pthread_attr_t *__restrict,
                               const struct sched_param *__restrict);
int pthread_attr_getinheritsched(const pthread_attr_t *__restrict,
                                 int *__restrict);
int pthread_attr_setinheritsched(pthread_attr_t *, int);

int pthread_mutexattr_destroy(pthread_mutexattr_t *);
int pthread_mutexattr_getprioceiling(const pthread_mutexattr_t *__restrict,
                                     int *__restrict);
int pthread_mutexattr_getprotocol(const pthread_mutexattr_t *__restrict,
                                  int *__restrict);
int pthread_mutexattr_getpshared(const pthread_mutexattr_t *__restrict,
                                 int *__restrict);
int pthread_mutexattr_getrobust(const pthread_mutexattr_t *__restrict,
                                int *__restrict);
int pthread_mutexattr_gettype(const pthread_mutexattr_t *__restrict,
                              int *__restrict);
int pthread_mutexattr_init(pthread_mutexattr_t *);
int pthread_mutexattr_setprioceiling(pthread_mutexattr_t *, int);
int pthread_mutexattr_setprotocol(pthread_mutexattr_t *, int);
int pthread_mutexattr_setpshared(pthread_mutexattr_t *, int);
int pthread_mutexattr_setrobust(pthread_mutexattr_t *, int);
int pthread_mutexattr_settype(pthread_mutexattr_t *, int);

int pthread_condattr_init(pthread_condattr_t *);
int pthread_condattr_destroy(pthread_condattr_t *);
int pthread_condattr_setclock(pthread_condattr_t *, clockid_t);
int pthread_condattr_setpshared(pthread_condattr_t *, int);
int pthread_condattr_getclock(const pthread_condattr_t *__restrict,
                              clockid_t *__restrict);
int pthread_condattr_getpshared(const pthread_condattr_t *__restrict,
                                int *__restrict);

int pthread_rwlockattr_init(pthread_rwlockattr_t *);
int pthread_rwlockattr_destroy(pthread_rwlockattr_t *);
int pthread_rwlockattr_setpshared(pthread_rwlockattr_t *, int);
int pthread_rwlockattr_getpshared(const pthread_rwlockattr_t *__restrict,
                                  int *__restrict);

int pthread_barrierattr_destroy(pthread_barrierattr_t *);
int pthread_barrierattr_getpshared(const pthread_barrierattr_t *__restrict,
                                   int *__restrict);
int pthread_barrierattr_init(pthread_barrierattr_t *);
int pthread_barrierattr_setpshared(pthread_barrierattr_t *, int);

int pthread_atfork(void (*)(void), void (*)(void), void (*)(void));

int pthread_getconcurrency(void);
int pthread_setconcurrency(int);

int pthread_getcpuclockid(pthread_t, clockid_t *);

struct __ptcb {
    void (*__f)(void *);
    void *__x;
    struct __ptcb *__next;
};

void _pthread_cleanup_push(struct __ptcb *, void (*)(void *), void *);
void _pthread_cleanup_pop(struct __ptcb *, int);

#define pthread_cleanup_push(f, x)                                             \
    do {                                                                       \
        struct __ptcb __cb;                                                    \
        _pthread_cleanup_push(&__cb, f, x);
#define pthread_cleanup_pop(r)                                                 \
    _pthread_cleanup_pop(&__cb, (r));                                          \
    }                                                                          \
    while (0)

void _pthread_attach(uint64_t pid);
