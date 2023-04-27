#include "config.h"
#include "drivers/clint.h"
#include "printk.h"
#include <errno.h>
#include <pthread.h>
#include <stdint.h>
#include <stdlib.h>

#define MAX_THREADS CONFIG_NR_CPUS

static size_t _nr_cpus = 0;

int _pthread_init(size_t nr_cpus) {
    _nr_cpus = nr_cpus;
    return 0;
}

typedef enum _thread_state {
    THREAD_IDLE = 0,
    THREAD_ALLOCATED,
    THREAD_ACTIVE,
    THREAD_FINISHED
} _thread_state_t;

typedef volatile struct _thread_entry {
    int pid;
    _thread_state_t state;
    void *(*func)(void *);
    void *args;
    void *ret;
} _thread_entry_t;

// Unimplemented types
struct pthread_once {
    int _todo;
};
struct pthread_key {
    int _todo;
};
struct pthread_spinlock {
    int _todo;
};
struct pthread_barrierattr {
    int _todo;
};
struct pthread_rwlockattr {
    int _todo;
};
struct pthread_rwlock {
    int _todo;
};
struct pthread_barrier {
    int _todo;
};

_thread_entry_t _threads[MAX_THREADS];
int cpt_threads = 0;

int pthread_attr_init(pthread_attr_t *attr) {
    (void)attr;
    return 0;
}

int pthread_attr_destroy(pthread_attr_t *attr) {
    (void)attr;
    return 0;
}

int pthread_attr_setdetachstate(pthread_attr_t *attr, int detachstate) {
    return 0;
}
int pthread_attr_getdetachstate(const pthread_attr_t *attr, int *detachstate) {
    return 0;
}

int pthread_create(pthread_t *thread, const pthread_attr_t *attr,
                   void *(*f)(void *), void *args) {
    printk("%d : %x ( %x )\n", cpt_threads, f, args);
    (void)attr;

    // Allocate PID in reverse order
    uint64_t pid = _nr_cpus - cpt_threads - 1;
    cpt_threads++; // Increase thread counter
    if (pid >= MAX_THREADS) {
        //  A system-imposed limit on the number of threads was encountered.
        return EAGAIN;
    }
    if (pid == 0) {
        panic("Last PID TODO\n");
    }
    // Allocate thread
    _thread_entry_t *te = &_threads[pid];
    te->pid = pid;
    te->state = THREAD_ALLOCATED;
    te->ret = NULL; // Guard
    te->func = f;
    te->args = args;
    *thread = pid;

    // Run thread on core[PID]
    clint_mswi_set(te->pid);

    return 0;
}

void _pthread_attach(uint64_t pid) {
    printk("%d\n", pid);
    _thread_entry_t *te = &_threads[pid];
    if (te->state != THREAD_ALLOCATED) {
        panic("Thread %d is not allocated\n", pid);
    }
    te->state = THREAD_ACTIVE;
    te->ret = te->func(te->args);
    te->state = THREAD_FINISHED;
    return;
}

int pthread_join(pthread_t thread, void **retval) {
    /*
    EDEADLK A deadlock was detected (e.g., two threads tried to join with each
    other); or thread specifies the calling thread. EINVAL thread is not a
    joinable thread. EINVAL Another thread is already waiting to join with this
    thread. ESRCH  No thread with the ID thread could be found.
    */

    _thread_entry_t *te = &_threads[thread];
    printk("%d\n", te->pid);

    while (te->state != THREAD_FINISHED) {
        ; /* Wait FINISHED */
    }
    if (retval) { // copies the exit status of the target thread
        (*retval) = te->ret;
    }
    return 0;
}

/**
 *  MUTEX
 */
static inline int _atomic_xchg(int *v, int n) {
    register int c;
    __asm__ __volatile__("amoswap.w.aqrl %0, %2, %1"
                         : "=r"(c), "+A"(v)
                         : "r"(n));
    return c;
}

int pthread_mutex_init(pthread_mutex_t *mutex,
                       const pthread_mutexattr_t *attr) {
    mutex->counter = 0;
    return 0;
}

int pthread_mutex_lock(pthread_mutex_t *mutex) {
    while (_atomic_xchg(&mutex->counter, 1) == 1)
        ;
    return 0;
}

int pthread_mutex_unlock(pthread_mutex_t *mutex) {
    _atomic_xchg(&mutex->counter, 0);
    return 0;
}

// int pthread_mutex_trylock(pthread_mutex_t *mutex);
// int pthread_mutex_timedlock(pthread_mutex_t *mutex,
//                             const struct timespec *ts);

int pthread_mutex_destroy(pthread_mutex_t *mutex) { return 0; }

/**
 * COND
 */

// INVALID IMPLEMENTATION !

int pthread_cond_init(pthread_cond_t *cond, const pthread_condattr_t *attr) {
    printk("%x\n", cond);
    cond->users = 0;
    return 0;
}

int pthread_cond_destroy(pthread_cond_t *cond) {
    printk("%x\n", cond);
    // Nothing to do
    return 0;
}

int pthread_cond_wait(pthread_cond_t *cond, pthread_mutex_t *mutex) {
    printk("%x enter\n", cond);
    //    pthread_mutex_lock(mutex);
    cond->users += 1;
    //    pthread_mutex_unlock(mutex);
    while (cond->users) {
        ;
    }
    printk("%x leave\n", cond);
    return 0;
}

int pthread_cond_broadcast(pthread_cond_t *cond) {
    printk("%x\n", cond);
    cond->users = 0;
    return 0;
}

int pthread_cond_signal(pthread_cond_t *cond) { panic("Unimplemented"); }
