#include "config.h"
#include "drivers/clint.h"
#include "printk.h"
#include <errno.h>
#include <pthread.h>
#include <stdint.h>

#define MAX_THREADS CONFIG_NR_CPUS

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
    te->state = THREAD_ALLOCATED;
    te->ret = NULL; // Guard
    te->func = f;
    te->args = args;
    *thread = cpt_threads;

    // Run thread on core[PID]
    clint_mswi_set(te->pid);
    // TODO clint IT wakeup !

    cpt_threads++;
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
