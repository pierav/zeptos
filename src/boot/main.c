/**
 * man pthread_create
 */

#include <ctype.h>
#include <errno.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define handle_error_en(en, msg)                                               \
    do {                                                                       \
        errno = en;                                                            \
        perror(msg);                                                           \
        exit(EXIT_FAILURE);                                                    \
    } while (0)

#define handle_error(msg)                                                      \
    do {                                                                       \
        perror(msg);                                                           \
        exit(EXIT_FAILURE);                                                    \
    } while (0)

struct thread_info {     /* Used as argument to thread_start() */
    pthread_t thread_id; /* ID returned by pthread_create() */
    int thread_num;      /* Application-defined thread # */
    char *argv_string;   /* From command-line argument */
};

/* Thread start function: display address near top of our stack,
   and return upper-cased copy of argv_string */

static void *thread_start(void *arg) {
    struct thread_info *tinfo = arg;
    char *uargv, *p;

    // printf("Thread %d: top of stack near %p; argv_string=%s\n",
    //        tinfo->thread_num, &p, tinfo->argv_string);

    uargv = strdup(tinfo->argv_string);
    if (uargv == NULL)
        handle_error("strdup");

    for (p = uargv; *p != '\0'; p++)
        *p = toupper(*p);

    printf("Thread %d: TOS~=%p; malloc[@%x] argv_string=%s->%s\n",
           tinfo->thread_num, &p, uargv, tinfo->argv_string, uargv);

    return uargv;
}

int main(int argc, char *argv[]) {
    (void)argc;
    (void)argv;
    int s, tnum, num_threads;
    struct thread_info *tinfo;
    pthread_attr_t attr;
    int stack_size;

    stack_size = -1;
    num_threads = 4;
    char *tnames[] = {"11aaaaa11", "22bbbbb22", "33ccccc33", "44ddddd44"};
    /* Initialize thread creation attributes */

    s = pthread_attr_init(&attr);
    if (s != 0)
        handle_error_en(s, "pthread_attr_init");

    if (stack_size > 0) {
        s = pthread_attr_setstacksize(&attr, stack_size);
        if (s != 0)
            handle_error_en(s, "pthread_attr_setstacksize");
    }

    /* Allocate memory for pthread_create() arguments */

    tinfo = malloc(num_threads * sizeof(struct thread_info));
    if (tinfo == NULL)
        handle_error("malloc");

    /* Create one thread for each command-line argument */

    for (tnum = 0; tnum < num_threads; tnum++) {
        tinfo[tnum].thread_num = tnum;
        tinfo[tnum].argv_string = tnames[tnum];
        /* The pthread_create() call stores the thread ID into
           corresponding element of tinfo[] */

        s = pthread_create(&tinfo[tnum].thread_id, &attr, &thread_start,
                           &tinfo[tnum]);
        if (s != 0)
            handle_error_en(s, "pthread_create");
    }

    /* Destroy the thread attributes object, since it is no
       longer needed */

    s = pthread_attr_destroy(&attr);
    if (s != 0)
        handle_error_en(s, "pthread_attr_destroy");

    /* Now join with each thread, and display its returned value */
    void *res[4];
    for (tnum = 0; tnum < num_threads; tnum++) {
        s = pthread_join(tinfo[tnum].thread_id, &res[tnum]);
        if (s != 0)
            handle_error_en(s, "pthread_join");
    }

    for (tnum = 0; tnum < num_threads; tnum++) {
        printf("Joined with thread %d; returned value was %s @%x\n",
               tinfo[tnum].thread_num, (char *)res[tnum], res[tnum]);
    }
    for (tnum = 0; tnum < num_threads; tnum++) {
        free(res[tnum]); /* Free memory allocated by thread */
    }
    free(tinfo);
    // exit(EXIT_SUCCESS);
}
