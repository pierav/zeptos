#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <time.h>

static struct timespec _ts = {0, 0};

static inline uint64_t _rdcyc() {
    uint64_t val;
    asm volatile("rdcycle %0" : "=r"(val)::);
    return val;
}

time_t time(time_t *tloc) {
    time_t t = _rdcyc();
    if (tloc)
        *tloc = t;
    return t;
}

char *ctime(const time_t *timep) {
    // static char	result[26];
    // snprintf(result, 26, "today:%d\n", timep);
    return "ctime()\n";
}

int clock_gettime(clockid_t clk_id, struct timespec *tp) {
    *tp = _ts;
    tp->tv_sec = _rdcyc();
    return 0;
}

int clock_settime(clockid_t clk_id, const struct timespec *tp) {
    _ts = *tp;
    return 0;
}

int settimeofday(const struct timeval *tv, const struct timezone *tz) {
    if (!tv)
        return 0;
    if (tv->tv_usec >= 1000000ULL)
        return EINVAL;
    struct timespec ts = {.tv_sec = tv->tv_sec, .tv_nsec = tv->tv_usec * 1000};
    return clock_settime(CLOCK_REALTIME, &ts);
}

int gettimeofday(struct timeval *tv, struct timezone *tz) {
    struct timespec ts;
    if (!tv)
        return 0;
    clock_gettime(CLOCK_REALTIME, &ts);
    tv->tv_sec = ts.tv_sec;
    tv->tv_usec = (long)ts.tv_nsec / 1000;
    return 0;
}