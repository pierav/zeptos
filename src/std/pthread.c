#include "config.h"
#include "printk.h"
#include <errno.h>
#include <pthread.h>
#include <stdint.h>

#define MAX_THREADS CONFIG_NR_CPUS

typedef enum _thread_state {
    THREAD_IDLE = 0,
    THREAD_ACTIVE = 1,
    THREAD_FINISHED = 2
} _thread_state_t;

typedef struct _thread_entry {
    int pid;
    _thread_state_t state;
    int ret;
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
struct pthread_mutexattr {
    int _todo;
};
struct pthread_condattr {
    int _todo;
};
struct pthread_barrierattr {
    int _todo;
};
struct pthread_rwlockattr {
    int _todo;
};
struct pthread_mutex {
    int _todo;
};
struct pthread_cond {
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

int pthread_create(pthread_t *thread, const pthread_attr_t *attr,
                   void *(*f)(void *), void *args) {
    printk("%d : %x ( %x )\n", cpt_threads, f, args);
    (void)attr;
    if (cpt_threads >= MAX_THREADS) {
        //  A system-imposed limit on the number of threads was encountered.
        return EAGAIN;
    }
    // Allocate thread
    _thread_entry_t *te = &_threads[cpt_threads];
    te->pid = cpt_threads;
    te->state = THREAD_ACTIVE;
    te->ret = -1; // Guard
    *thread = cpt_threads;

    // Run thread on core
    // TODO clint IT wakeup !

    cpt_threads++;
    return 0;
}

int pthread_join(pthread_t thread, void **retval) {
    // TODO
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
        *(uintptr_t *)(*retval) = te->ret;
    }
    return 0;
}
