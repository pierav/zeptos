#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <time.h>

#define YEAR0 1900
#define EPOCH_YR 1970
#define SECS_DAY (24L * 60L * 60L)
#define LEAPYEAR(year) (!((year) % 4) && (((year) % 100) || !((year) % 400)))
#define YEARSIZE(year) (LEAPYEAR(year) ? 366 : 365)
#define FIRSTSUNDAY(timp) (((timp)->tm_yday - (timp)->tm_wday + 420) % 7)
#define FIRSTDAYOF(timp) (((timp)->tm_wday - (timp)->tm_yday + 420) % 7)

#define TIME_MAX 2147483647L

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
clock_t clock(void) { return _rdcyc(); }

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

int _daylight = 0;  // Non-zero if daylight savings time is used
long _dstbias = 0;  // Offset for Daylight Saving Time
long _timezone = 0; // Difference in seconds between GMT and local time
char *_tzname[2] = {"GMT", "GMT"}; // Standard/daylight savings time zone names

const char *_days[] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};

const char *_months[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun",
                         "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};

const int _ytab[2][12] = {{31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31},
                          {31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31}};

struct tm *gmtime_r(const time_t *timer, struct tm *tmbuf) {
    time_t time = *timer;
    unsigned long dayclock, dayno;
    int year = EPOCH_YR;

    dayclock = (unsigned long)time % SECS_DAY;
    dayno = (unsigned long)time / SECS_DAY;

    tmbuf->tm_sec = dayclock % 60;
    tmbuf->tm_min = (dayclock % 3600) / 60;
    tmbuf->tm_hour = dayclock / 3600;
    tmbuf->tm_wday = (dayno + 4) % 7; // Day 0 was a thursday
    while (dayno >= (unsigned long)YEARSIZE(year)) {
        dayno -= YEARSIZE(year);
        year++;
    }
    tmbuf->tm_year = year - YEAR0;
    tmbuf->tm_yday = dayno;
    tmbuf->tm_mon = 0;
    while (dayno >= (unsigned long)_ytab[LEAPYEAR(year)][tmbuf->tm_mon]) {
        dayno -= _ytab[LEAPYEAR(year)][tmbuf->tm_mon];
        tmbuf->tm_mon++;
    }
    tmbuf->tm_mday = dayno + 1;
    tmbuf->tm_isdst = 0;
    tmbuf->tm_gmtoff = 0;
    tmbuf->tm_zone = "UTC";
    return tmbuf;
}

struct tm *localtime_r(const time_t *timer, struct tm *tmbuf) {
    time_t t;
    t = *timer - _timezone;
    return gmtime_r(&t, tmbuf);
}

time_t mktime(struct tm *tmbuf) {
    long day, year;
    int tm_year;
    int yday, month;
    /*unsigned*/ long seconds;
    int overflow;
    long dst;

    tmbuf->tm_min += tmbuf->tm_sec / 60;
    tmbuf->tm_sec %= 60;
    if (tmbuf->tm_sec < 0) {
        tmbuf->tm_sec += 60;
        tmbuf->tm_min--;
    }
    tmbuf->tm_hour += tmbuf->tm_min / 60;
    tmbuf->tm_min = tmbuf->tm_min % 60;
    if (tmbuf->tm_min < 0) {
        tmbuf->tm_min += 60;
        tmbuf->tm_hour--;
    }
    day = tmbuf->tm_hour / 24;
    tmbuf->tm_hour = tmbuf->tm_hour % 24;
    if (tmbuf->tm_hour < 0) {
        tmbuf->tm_hour += 24;
        day--;
    }
    tmbuf->tm_year += tmbuf->tm_mon / 12;
    tmbuf->tm_mon %= 12;
    if (tmbuf->tm_mon < 0) {
        tmbuf->tm_mon += 12;
        tmbuf->tm_year--;
    }
    day += (tmbuf->tm_mday - 1);
    while (day < 0) {
        if (--tmbuf->tm_mon < 0) {
            tmbuf->tm_year--;
            tmbuf->tm_mon = 11;
        }
        day += _ytab[LEAPYEAR(YEAR0 + tmbuf->tm_year)][tmbuf->tm_mon];
    }
    while (day >= _ytab[LEAPYEAR(YEAR0 + tmbuf->tm_year)][tmbuf->tm_mon]) {
        day -= _ytab[LEAPYEAR(YEAR0 + tmbuf->tm_year)][tmbuf->tm_mon];
        if (++(tmbuf->tm_mon) == 12) {
            tmbuf->tm_mon = 0;
            tmbuf->tm_year++;
        }
    }
    tmbuf->tm_mday = day + 1;
    year = EPOCH_YR;
    if (tmbuf->tm_year < year - YEAR0)
        return (time_t)-1;
    seconds = 0;
    day = 0; // Means days since day 0 now
    overflow = 0;

    // Assume that when day becomes negative, there will certainly
    // be overflow on seconds.
    // The check for overflow needs not to be done for leapyears
    // divisible by 400.
    // The code only works when year (1970) is not a leapyear.
    tm_year = tmbuf->tm_year + YEAR0;

    if (TIME_MAX / 365 < tm_year - year)
        overflow++;
    day = (tm_year - year) * 365;
    if (TIME_MAX - day < (tm_year - year) / 4 + 1)
        overflow++;
    day += (tm_year - year) / 4 + ((tm_year % 4) && tm_year % 4 < year % 4);
    day -= (tm_year - year) / 100 +
           ((tm_year % 100) && tm_year % 100 < year % 100);
    day += (tm_year - year) / 400 +
           ((tm_year % 400) && tm_year % 400 < year % 400);

    yday = month = 0;
    while (month < tmbuf->tm_mon) {
        yday += _ytab[LEAPYEAR(tm_year)][month];
        month++;
    }
    yday += (tmbuf->tm_mday - 1);
    if (day + yday < 0)
        overflow++;
    day += yday;

    tmbuf->tm_yday = yday;
    tmbuf->tm_wday = (day + 4) % 7; // Day 0 was thursday (4)

    seconds = ((tmbuf->tm_hour * 60L) + tmbuf->tm_min) * 60L + tmbuf->tm_sec;

    if ((TIME_MAX - seconds) / SECS_DAY < day)
        overflow++;
    seconds += day * SECS_DAY;

    // Now adjust according to timezone and daylight saving time
    if (((_timezone > 0) && (TIME_MAX - _timezone < seconds)) ||
        ((_timezone < 0) && (seconds < -_timezone))) {
        overflow++;
    }
    seconds += _timezone;

    if (tmbuf->tm_isdst) {
        dst = _dstbias;
    } else {
        dst = 0;
    }

    if (dst > seconds)
        overflow++;
    seconds -= dst;

    if (overflow)
        return (time_t)-1;

    if ((time_t)seconds != seconds)
        return (time_t)-1;
    return (time_t)seconds;
}

char *asctime_r_secure(const struct tm *tm, char *buf, size_t n) {
    snprintf(buf, n, "%4d %s %02d %02d:%02d", tm->tm_year + YEAR0,
             _months[tm->tm_mon], tm->tm_mday, tm->tm_hour, tm->tm_min);
    return buf;
}

char *asctime_r(const struct tm *tm, char *buf) {
    return asctime_r_secure(tm, buf, (size_t)-1);
}

char *ctime_r_secure(const time_t *timer, char *buf, size_t n) {
    struct tm tm;
    return asctime_r_secure(localtime_r(timer, &tm), buf, n);
}

char *ctime_r(const time_t *timer, char *buf) {
    return ctime_r_secure(timer, buf, (size_t)-1);
}

#define CTIME_BUFFER_SIZE 128
char *ctime(const time_t *timer) {
    static char buf[CTIME_BUFFER_SIZE];
    return ctime_r_secure(timer, buf, CTIME_BUFFER_SIZE);
}
