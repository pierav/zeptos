// taken from
// https://elixir.bootlin.com/linux/v6.3-rc6/source/lib/vsprintf.c#L3410

#include "bitset.h"
#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <sys/types.h>

#define do_div(a, b) ((a) / (b))

#define KSTRTOX_OVERFLOW (1U << 31)

const char *_parse_integer_fixup_radix(const char *s, unsigned int *base) {
    if (*base == 0) {
        if (s[0] == '0') {
            if (tolower(s[1]) == 'x' && isxdigit(s[2]))
                *base = 16;
            else
                *base = 8;
        } else
            *base = 10;
    }
    if (*base == 16 && s[0] == '0' && tolower(s[1]) == 'x')
        s += 2;
    return s;
}

unsigned int _parse_integer_limit(const char *s, unsigned int base,
                                  unsigned long long *p, size_t max_chars) {
    unsigned long long res;
    unsigned int rv;

    res = 0;
    rv = 0;
    while (max_chars--) {
        unsigned int c = *s;
        unsigned int lc = c | 0x20; /* don't tolower() this line */
        unsigned int val;

        if ('0' <= c && c <= '9')
            val = c - '0';
        else if ('a' <= lc && lc <= 'f')
            val = lc - 'a' + 10;
        else
            break;

        if (val >= base)
            break;
        /*
         * Check for overflow only if we are within range of
         * it in the max base we support (16)
         */
        // if (res & (~0ull << 60)) {
        // 	if (res > div_uint64_t(ULLONG_MAX - val, base))
        // 		rv |= KSTRTOX_OVERFLOW;
        // }
        res = res * base + val;
        rv++;
        s++;
    }
    *p = res;
    return rv;
}

static unsigned long long simple_strntoull(const char *startp, size_t max_chars,
                                           char **endp, unsigned int base) {
    const char *cp;
    unsigned long long result = 0ULL;
    size_t prefix_chars;
    unsigned int rv;

    cp = _parse_integer_fixup_radix(startp, &base);
    prefix_chars = cp - startp;
    if (prefix_chars < max_chars) {
        rv = _parse_integer_limit(cp, base, &result, max_chars - prefix_chars);
        /* FIXME */
        cp += (rv & ~KSTRTOX_OVERFLOW);
    } else {
        /* Field too short for prefix + digit, skip over without converting */
        cp = startp + max_chars;
    }

    if (endp)
        *endp = (char *)cp;

    return result;
}

/**
 * simple_strtoull - convert a string to an unsigned long long
 * @cp: The start of the string
 * @endp: A pointer to the end of the parsed string will be placed here
 * @base: The number base to use
 *
 * This function has caveats. Please use kstrtoull instead.
 */

unsigned long long simple_strtoull(const char *cp, char **endp,
                                   unsigned int base) {
    return simple_strntoull(cp, INT_MAX, endp, base);
}

/**
 * simple_strtoul - convert a string to an unsigned long
 * @cp: The start of the string
 * @endp: A pointer to the end of the parsed string will be placed here
 * @base: The number base to use
 *
 * This function has caveats. Please use kstrtoul instead.
 */
unsigned long simple_strtoul(const char *cp, char **endp, unsigned int base) {
    return simple_strtoull(cp, endp, base);
}

/**
 * simple_strtol - convert a string to a signed long
 * @cp: The start of the string
 * @endp: A pointer to the end of the parsed string will be placed here
 * @base: The number base to use
 *
 * This function has caveats. Please use kstrtol instead.
 */
long simple_strtol(const char *cp, char **endp, unsigned int base) {
    if (*cp == '-')
        return -simple_strtoul(cp + 1, endp, base);

    return simple_strtoul(cp, endp, base);
}

static long long simple_strntoll(const char *cp, size_t max_chars, char **endp,
                                 unsigned int base) {
    /*
     * simple_strntoull() safely handles receiving max_chars==0 in the
     * case cp[0] == '-' && max_chars == 1.
     * If max_chars == 0 we can drop through and pass it to simple_strntoull()
     * and the content of *cp is irrelevant.
     */
    if (*cp == '-' && max_chars > 0)
        return -simple_strntoull(cp + 1, max_chars - 1, endp, base);

    return simple_strntoull(cp, max_chars, endp, base);
}

/**
 * simple_strtoll - convert a string to a signed long long
 * @cp: The start of the string
 * @endp: A pointer to the end of the parsed string will be placed here
 * @base: The number base to use
 *
 * This function has caveats. Please use kstrtoll instead.
 */
long long simple_strtoll(const char *cp, char **endp, unsigned int base) {
    return simple_strntoll(cp, INT_MAX, endp, base);
}

static int skip_atoi(const char **s) {
    int i = 0;

    do {
        i = i * 10 + *((*s)++) - '0';
    } while (isdigit(**s));

    return i;
}

char *skip_spaces(const char *str) {
    while (isspace(*str))
        ++str;
    return (char *)str;
}

const char hex_asc[] = "0123456789abcdef";
const char hex_asc_upper[] = "0123456789ABCDEF";

/*
 * Decimal conversion is by far the most typical, and is used for
 * /proc and /sys data. This directly impacts e.g. top performance
 * with many processes running. We optimize it for speed by emitting
 * two characters at a time, using a 200 byte lookup table. This
 * roughly halves the number of multiplications compared to computing
 * the digits one at a time. Implementation strongly inspired by the
 * previous version, which in turn used ideas described at
 * <http://www.cs.uiowa.edu/~jones/bcd/divide.html> (with permission
 * from the author, Douglas W. Jones).
 *
 * It turns out there is precisely one 26 bit fixed-point
 * approximation a of 64/100 for which x/100 == (x * (uint64_t)a) >> 32
 * holds for all x in [0, 10^8-1], namely a = 0x28f5c29. The actual
 * range happens to be somewhat larger (x <= 1073741898), but that's
 * irrelevant for our purpose.
 *
 * For dividing a number in the range [10^4, 10^6-1] by 100, we still
 * need a 32x32->64 bit multiply, so we simply use the same constant.
 *
 * For dividing a number in the range [100, 10^4-1] by 100, there are
 * several options. The simplest is (x * 0x147b) >> 19, which is valid
 * for all x <= 43698.
 */

// __attribute__((force))

static const uint16_t decpair[100] = {
#define _(x) (uint16_t)(((x % 10) | ((x / 10) << 8)) + 0x3030)
    _(0),  _(1),  _(2),  _(3),  _(4),  _(5),  _(6),  _(7),  _(8),  _(9),
    _(10), _(11), _(12), _(13), _(14), _(15), _(16), _(17), _(18), _(19),
    _(20), _(21), _(22), _(23), _(24), _(25), _(26), _(27), _(28), _(29),
    _(30), _(31), _(32), _(33), _(34), _(35), _(36), _(37), _(38), _(39),
    _(40), _(41), _(42), _(43), _(44), _(45), _(46), _(47), _(48), _(49),
    _(50), _(51), _(52), _(53), _(54), _(55), _(56), _(57), _(58), _(59),
    _(60), _(61), _(62), _(63), _(64), _(65), _(66), _(67), _(68), _(69),
    _(70), _(71), _(72), _(73), _(74), _(75), _(76), _(77), _(78), _(79),
    _(80), _(81), _(82), _(83), _(84), _(85), _(86), _(87), _(88), _(89),
    _(90), _(91), _(92), _(93), _(94), _(95), _(96), _(97), _(98), _(99),
#undef _
};

/*
 * This will print a single '0' even if r == 0, since we would
 * immediately jump to out_r where two 0s would be written but only
 * one of them accounted for in buf. This is needed by ip4_string
 * below. All other callers pass a non-zero value of r.
 */
static char *put_dec_trunc8(char *buf, unsigned r) {
    unsigned q;

    /* 1 <= r < 10^8 */
    if (r < 100)
        goto out_r;

    /* 100 <= r < 10^8 */
    q = (r * (uint64_t)0x28f5c29) >> 32;
    *((uint16_t *)buf) = decpair[r - 100 * q];
    buf += 2;

    /* 1 <= q < 10^6 */
    if (q < 100)
        goto out_q;

    /*  100 <= q < 10^6 */
    r = (q * (uint64_t)0x28f5c29) >> 32;
    *((uint16_t *)buf) = decpair[q - 100 * r];
    buf += 2;

    /* 1 <= r < 10^4 */
    if (r < 100)
        goto out_r;

    /* 100 <= r < 10^4 */
    q = (r * 0x147b) >> 19;
    *((uint16_t *)buf) = decpair[r - 100 * q];
    buf += 2;
out_q:
    /* 1 <= q < 100 */
    r = q;
out_r:
    /* 1 <= r < 100 */
    *((uint16_t *)buf) = decpair[r];
    buf += r < 10 ? 1 : 2;
    return buf;
}

#if BITS_PER_LONG == 64 && BITS_PER_LONG_LONG == 64
static char *put_dec_full8(char *buf, unsigned r) {
    unsigned q;

    /* 0 <= r < 10^8 */
    q = (r * (uint64_t)0x28f5c29) >> 32;
    *((uint16_t *)buf) = decpair[r - 100 * q];
    buf += 2;

    /* 0 <= q < 10^6 */
    r = (q * (uint64_t)0x28f5c29) >> 32;
    *((uint16_t *)buf) = decpair[q - 100 * r];
    buf += 2;

    /* 0 <= r < 10^4 */
    q = (r * 0x147b) >> 19;
    *((uint16_t *)buf) = decpair[r - 100 * q];
    buf += 2;

    /* 0 <= q < 100 */
    *((uint16_t *)buf) = decpair[q];
    buf += 2;
    return buf;
}

static char *put_dec(char *buf, unsigned long long n) {
    if (n >= 100 * 1000 * 1000)
        buf = put_dec_full8(buf, do_div(n, 100 * 1000 * 1000));
    /* 1 <= n <= 1.6e11 */
    if (n >= 100 * 1000 * 1000)
        buf = put_dec_full8(buf, do_div(n, 100 * 1000 * 1000));
    /* 1 <= n < 1e8 */
    return put_dec_trunc8(buf, n);
}

#elif BITS_PER_LONG == 32 && BITS_PER_LONG_LONG == 64

static void put_dec_full4(char *buf, unsigned r) {
    unsigned q;

    /* 0 <= r < 10^4 */
    q = (r * 0x147b) >> 19;
    *((uint16_t *)buf) = decpair[r - 100 * q];
    buf += 2;
    /* 0 <= q < 100 */
    *((uint16_t *)buf) = decpair[q];
}

/*
 * Call put_dec_full4 on x % 10000, return x / 10000.
 * The approximation x/10000 == (x * 0x346DC5D7) >> 43
 * holds for all x < 1,128,869,999.  The largest value this
 * helper will ever be asked to convert is 1,125,520,955.
 * (second call in the put_dec code, assuming n is all-ones).
 */
static noinline_for_stack unsigned put_dec_helper4(char *buf, unsigned x) {
    uint32_t q = (x * (uint64_t)0x346DC5D7) >> 43;

    put_dec_full4(buf, x - q * 10000);
    return q;
}

/* Based on code by Douglas W. Jones found at
 * <http://www.cs.uiowa.edu/~jones/bcd/decimal.html#sixtyfour>
 * (with permission from the author).
 * Performs no 64-bit division and hence should be fast on 32-bit machines.
 */
static char *put_dec(char *buf, unsigned long long n) {
    uint32_t d3, d2, d1, q, h;

    if (n < 100 * 1000 * 1000)
        return put_dec_trunc8(buf, n);

    d1 = ((uint32_t)n >> 16); /* implicit "& 0xffff" */
    h = (n >> 32);
    d2 = (h)&0xffff;
    d3 = (h >> 16); /* implicit "& 0xffff" */

    /* n = 2^48 d3 + 2^32 d2 + 2^16 d1 + d0
         = 281_4749_7671_0656 d3 + 42_9496_7296 d2 + 6_5536 d1 + d0 */
    q = 656 * d3 + 7296 * d2 + 5536 * d1 + ((uint32_t)n & 0xffff);
    q = put_dec_helper4(buf, q);

    q += 7671 * d3 + 9496 * d2 + 6 * d1;
    q = put_dec_helper4(buf + 4, q);

    q += 4749 * d3 + 42 * d2;
    q = put_dec_helper4(buf + 8, q);

    q += 281 * d3;
    buf += 12;
    if (q)
        buf = put_dec_trunc8(buf, q);
    else
        while (buf[-1] == '0')
            --buf;

    return buf;
}

#endif

/*
 * Convert passed number to decimal string.
 * Returns the length of string.  On buffer overflow, returns 0.
 *
 * If speed is not important, use snprintf(). It's easy to read the code.
 */
int num_to_str(char *buf, int size, unsigned long long num,
               unsigned int width) {
    /* put_dec requires 2-byte alignment of the buffer. */
    char tmp[sizeof(num) * 3] __attribute__((__aligned__(2)));
    int idx, len;

    /* put_dec() may work incorrectly for num = 0 (generate "", not "0") */
    if (num <= 9) {
        tmp[0] = '0' + num;
        len = 1;
    } else {
        len = put_dec(tmp, num) - tmp;
    }

    if (len > size || width > size)
        return 0;

    if (width > len) {
        width = width - len;
        for (idx = 0; idx < width; idx++)
            buf[idx] = ' ';
    } else {
        width = 0;
    }

    for (idx = 0; idx < len; ++idx)
        buf[idx + width] = tmp[len - idx - 1];

    return len + width;
}

#define SIGN 1     /* unsigned/signed, must be 1 */
#define LEFT 2     /* left justified */
#define PLUS 4     /* show plus */
#define SPACE 8    /* space if plus */
#define ZEROPAD 16 /* pad with zero, must be 16 == '0' - ' ' */
#define SMALL 32   /* use lowercase in hex (must be 32 == 0x20) */
#define SPECIAL 64 /* prefix hex with "0x", octal with "0" */

enum format_type {
    FORMAT_TYPE_NONE, /* Just a string part */
    FORMAT_TYPE_WIDTH,
    FORMAT_TYPE_PRECISION,
    FORMAT_TYPE_CHAR,
    FORMAT_TYPE_STR,
    FORMAT_TYPE_PTR,
    FORMAT_TYPE_PERCENT_CHAR,
    FORMAT_TYPE_INVALID,
    FORMAT_TYPE_LONG_LONG,
    FORMAT_TYPE_ULONG,
    FORMAT_TYPE_LONG,
    FORMAT_TYPE_UBYTE,
    FORMAT_TYPE_BYTE,
    FORMAT_TYPE_USHORT,
    FORMAT_TYPE_SHORT,
    FORMAT_TYPE_UINT,
    FORMAT_TYPE_INT,
    FORMAT_TYPE_SIZE_T,
    FORMAT_TYPE_PTRDIFF
};

struct printf_spec {
    unsigned int type : 8;       /* format_type enum */
    signed int field_width : 24; /* width of output field */
    unsigned int flags : 8;      /* flags to number() */
    unsigned int base : 8;       /* number base, 8, 10 or 16 only */
    signed int precision : 16;   /* # of digits/chars */
} __packed;

#define FIELD_WIDTH_MAX ((1 << 23) - 1)
#define PRECISION_MAX ((1 << 15) - 1)

static char *number(char *buf, char *end, unsigned long long num,
                    struct printf_spec spec) {
    /* put_dec requires 2-byte alignment of the buffer. */
    char tmp[3 * sizeof(num)] __attribute__((__aligned__(2)));
    char sign;
    char locase;
    int need_pfx = ((spec.flags & SPECIAL) && spec.base != 10);
    int i;
    bool is_zero = num == 0LL;
    int field_width = spec.field_width;
    int precision = spec.precision;

    /* locase = 0 or 0x20. ORing digits or letters with 'locase'
     * produces same digits or (maybe lowercased) letters */
    locase = (spec.flags & SMALL);
    if (spec.flags & LEFT)
        spec.flags &= ~ZEROPAD;
    sign = 0;
    if (spec.flags & SIGN) {
        if ((signed long long)num < 0) {
            sign = '-';
            num = -(signed long long)num;
            field_width--;
        } else if (spec.flags & PLUS) {
            sign = '+';
            field_width--;
        } else if (spec.flags & SPACE) {
            sign = ' ';
            field_width--;
        }
    }
    if (need_pfx) {
        if (spec.base == 16)
            field_width -= 2;
        else if (!is_zero)
            field_width--;
    }

    /* generate full string in tmp[], in reverse order */
    i = 0;
    if (num < spec.base)
        tmp[i++] = hex_asc_upper[num] | locase;
    else if (spec.base != 10) { /* 8 or 16 */
        int mask = spec.base - 1;
        int shift = 3;

        if (spec.base == 16)
            shift = 4;
        do {
            tmp[i++] = (hex_asc_upper[((unsigned char)num) & mask] | locase);
            num >>= shift;
        } while (num);
    } else { /* base 10 */
        i = put_dec(tmp, num) - tmp;
    }

    /* printing 100 using %2d gives "100", not "00" */
    if (i > precision)
        precision = i;
    /* leading space padding */
    field_width -= precision;
    if (!(spec.flags & (ZEROPAD | LEFT))) {
        while (--field_width >= 0) {
            if (buf < end)
                *buf = ' ';
            ++buf;
        }
    }
    /* sign */
    if (sign) {
        if (buf < end)
            *buf = sign;
        ++buf;
    }
    /* "0x" / "0" prefix */
    if (need_pfx) {
        if (spec.base == 16 || !is_zero) {
            if (buf < end)
                *buf = '0';
            ++buf;
        }
        if (spec.base == 16) {
            if (buf < end)
                *buf = ('X' | locase);
            ++buf;
        }
    }
    /* zero or space padding */
    if (!(spec.flags & LEFT)) {
        char c = ' ' + (spec.flags & ZEROPAD);

        while (--field_width >= 0) {
            if (buf < end)
                *buf = c;
            ++buf;
        }
    }
    /* hmm even more zero padding? */
    while (i <= --precision) {
        if (buf < end)
            *buf = '0';
        ++buf;
    }
    /* actual digits of result */
    while (--i >= 0) {
        if (buf < end)
            *buf = tmp[i];
        ++buf;
    }
    /* trailing space padding */
    while (--field_width >= 0) {
        if (buf < end)
            *buf = ' ';
        ++buf;
    }

    return buf;
}

static void move_right(char *buf, char *end, unsigned len, unsigned spaces) {
    size_t size;
    if (buf >= end) /* nowhere to put anything */
        return;
    size = end - buf;
    if (size <= spaces) {
        memset(buf, ' ', size);
        return;
    }
    if (len) {
        if (len > size - spaces)
            len = size - spaces;
        memmove(buf + spaces, buf, len);
    }
    memset(buf, ' ', spaces);
}

static char *widen_string(char *buf, int n, char *end,
                          struct printf_spec spec) {
    unsigned spaces;

    if (n >= spec.field_width)
        return buf;
    /* we want to pad the sucker */
    spaces = spec.field_width - n;
    if (!(spec.flags & LEFT)) {
        move_right(buf - n, end, n, spaces);
        return buf + spaces;
    }
    while (spaces--) {
        if (buf < end)
            *buf = ' ';
        ++buf;
    }
    return buf;
}

/* Handle string from a well known address. */
static char *string_nocheck(char *buf, char *end, const char *s,
                            struct printf_spec spec) {
    int len = 0;
    int lim = spec.precision;

    while (lim--) {
        char c = *s++;
        if (!c)
            break;
        if (buf < end)
            *buf = c;
        ++buf;
        ++len;
    }
    return widen_string(buf, len, end, spec);
}

/* Be careful: error messages must fit into the given buffer. */
static char *error_string(char *buf, char *end, const char *s,
                          struct printf_spec spec) {
    /*
     * Hard limit to avoid a completely insane messages. It actually
     * works pretty well because most error messages are in
     * the many pointer format modifiers.
     */
    if (spec.precision == -1)
        spec.precision = 2 * sizeof(void *);

    return string_nocheck(buf, end, s, spec);
}

/*
 * Do not call any complex external code here. Nested printk()/vsprintf()
 * might cause infinite loops. Failures might break printk() and would
 * be hard to debug.
 */
static const char *check_pointer_msg(const void *ptr) {
    if (!ptr)
        return "(null)";
    return NULL;
}

static int check_pointer(char **buf, char *end, const void *ptr,
                         struct printf_spec spec) {
    const char *err_msg;
    err_msg = check_pointer_msg(ptr);
    if (err_msg) {
        *buf = error_string(*buf, end, err_msg, spec);
        return -EFAULT;
    }
    return 0;
}

static char *string(char *buf, char *end, const char *s,
                    struct printf_spec spec) {
    if (check_pointer(&buf, end, s, spec))
        return buf;

    return string_nocheck(buf, end, s, spec);
}

static char *pointer_string(char *buf, char *end, const void *ptr,
                            struct printf_spec spec) {
    spec.base = 16;
    spec.flags |= SMALL;
    if (spec.field_width == -1) {
        spec.field_width = 2 * sizeof(ptr);
        spec.flags |= ZEROPAD;
    }

    return number(buf, end, (unsigned long int)ptr, spec);
}

static const struct printf_spec default_str_spec = {
    .field_width = -1,
    .precision = -1,
};

static const struct printf_spec default_flag_spec = {
    .base = 16,
    .precision = -1,
    .flags = SPECIAL | SMALL,
};

static const struct printf_spec default_dec_spec = {
    .base = 10,
    .precision = -1,
};

static const struct printf_spec default_dec02_spec = {
    .base = 10,
    .field_width = 2,
    .precision = -1,
    .flags = ZEROPAD,
};

static const struct printf_spec default_dec04_spec = {
    .base = 10,
    .field_width = 4,
    .precision = -1,
    .flags = ZEROPAD,
};

static char *pointer(const char *fmt, char *buf, char *end, void *ptr,
                     struct printf_spec spec) {
    return pointer_string(buf, end, ptr, spec);
}

static int format_decode(const char *fmt, struct printf_spec *spec) {
    const char *start = fmt;
    char qualifier;

    /* we finished early by reading the field width */
    if (spec->type == FORMAT_TYPE_WIDTH) {
        if (spec->field_width < 0) {
            spec->field_width = -spec->field_width;
            spec->flags |= LEFT;
        }
        spec->type = FORMAT_TYPE_NONE;
        goto precision;
    }

    /* we finished early by reading the precision */
    if (spec->type == FORMAT_TYPE_PRECISION) {
        if (spec->precision < 0)
            spec->precision = 0;

        spec->type = FORMAT_TYPE_NONE;
        goto qualifier;
    }

    /* By default */
    spec->type = FORMAT_TYPE_NONE;

    for (; *fmt; ++fmt) {
        if (*fmt == '%')
            break;
    }

    /* Return the current non-format string */
    if (fmt != start || !*fmt)
        return fmt - start;

    /* Process flags */
    spec->flags = 0;

    while (1) { /* this also skips first '%' */
        bool found = true;

        ++fmt;

        switch (*fmt) {
        case '-':
            spec->flags |= LEFT;
            break;
        case '+':
            spec->flags |= PLUS;
            break;
        case ' ':
            spec->flags |= SPACE;
            break;
        case '#':
            spec->flags |= SPECIAL;
            break;
        case '0':
            spec->flags |= ZEROPAD;
            break;
        default:
            found = false;
        }

        if (!found)
            break;
    }

    /* get field width */
    spec->field_width = -1;

    if (isdigit(*fmt))
        spec->field_width = skip_atoi(&fmt);
    else if (*fmt == '*') {
        /* it's the next argument */
        spec->type = FORMAT_TYPE_WIDTH;
        return ++fmt - start;
    }

precision:
    /* get the precision */
    spec->precision = -1;
    if (*fmt == '.') {
        ++fmt;
        if (isdigit(*fmt)) {
            spec->precision = skip_atoi(&fmt);
            if (spec->precision < 0)
                spec->precision = 0;
        } else if (*fmt == '*') {
            /* it's the next argument */
            spec->type = FORMAT_TYPE_PRECISION;
            return ++fmt - start;
        }
    }

qualifier:
    /* get the conversion qualifier */
    qualifier = 0;
    if (*fmt == 'h' || tolower(*fmt) == 'l' || *fmt == 'z' || *fmt == 't') {
        qualifier = *fmt++;
        if (qualifier == *fmt) {
            if (qualifier == 'l') {
                qualifier = 'L';
                ++fmt;
            } else if (qualifier == 'h') {
                qualifier = 'H';
                ++fmt;
            }
        }
    }

    /* default base */
    spec->base = 10;
    switch (*fmt) {
    case 'c':
        spec->type = FORMAT_TYPE_CHAR;
        return ++fmt - start;

    case 's':
        spec->type = FORMAT_TYPE_STR;
        return ++fmt - start;

    case 'p':
        spec->type = FORMAT_TYPE_PTR;
        return ++fmt - start;

    case '%':
        spec->type = FORMAT_TYPE_PERCENT_CHAR;
        return ++fmt - start;

    /* integer number formats - set up the flags and "break" */
    case 'o':
        spec->base = 8;
        break;

    case 'x':
        spec->flags |= SMALL;
        // fall through
    case 'X':
        spec->base = 16;
        break;

    case 'd':
    case 'i':
        spec->flags |= SIGN;
        break;
    case 'u':
        break;

    case 'n':
        /*
         * Since %n poses a greater security risk than
         * utility, treat it as any other invalid or
         * unsupported format specifier.
         */
        // fallthrough

    default:
        spec->type = FORMAT_TYPE_INVALID;
        return fmt - start;
    }

    if (qualifier == 'L')
        spec->type = FORMAT_TYPE_LONG_LONG;
    else if (qualifier == 'l') {
        spec->type = FORMAT_TYPE_ULONG + (spec->flags & SIGN);
    } else if (qualifier == 'z') {
        spec->type = FORMAT_TYPE_SIZE_T;
    } else if (qualifier == 't') {
        spec->type = FORMAT_TYPE_PTRDIFF;
    } else if (qualifier == 'H') {
        spec->type = FORMAT_TYPE_UBYTE + (spec->flags & SIGN);
    } else if (qualifier == 'h') {
        spec->type = FORMAT_TYPE_USHORT + (spec->flags & SIGN);
    } else {
        spec->type = FORMAT_TYPE_UINT + (spec->flags & SIGN);
    }

    return ++fmt - start;
}

static void set_field_width(struct printf_spec *spec, int width) {
    spec->field_width = width;
    // if (WARN_ONCE(spec->field_width != width, "field width %d too large",
    // width)) { 	spec->field_width = clamp(width, -FIELD_WIDTH_MAX,
    // FIELD_WIDTH_MAX);
    // }
}

static void set_precision(struct printf_spec *spec, int prec) {
    spec->precision = prec;
    // if (WARN_ONCE(spec->precision != prec, "precision %d too large", prec)) {
    // 	spec->precision = clamp(prec, 0, PRECISION_MAX);
    // }
}

int vsnprintf(char *buf, size_t size, const char *fmt, va_list args) {
    unsigned long long num;
    char *str, *end;
    struct printf_spec spec = {0};

    /* Reject out-of-range values early.  Large positive sizes are
       used for unknown buffer sizes. */
    if (size > INT_MAX)
        return 0;

    str = buf;
    end = buf + size;

    /* Make sure end is always >= buf */
    if (end < buf) {
        end = ((void *)-1);
        size = end - buf;
    }

    while (*fmt) {
        const char *old_fmt = fmt;
        int read = format_decode(fmt, &spec);

        fmt += read;

        switch (spec.type) {
        case FORMAT_TYPE_NONE: {
            int copy = read;
            if (str < end) {
                if (copy > end - str)
                    copy = end - str;
                memcpy(str, old_fmt, copy);
            }
            str += read;
            break;
        }

        case FORMAT_TYPE_WIDTH:
            set_field_width(&spec, va_arg(args, int));
            break;

        case FORMAT_TYPE_PRECISION:
            set_precision(&spec, va_arg(args, int));
            break;

        case FORMAT_TYPE_CHAR: {
            char c;

            if (!(spec.flags & LEFT)) {
                while (--spec.field_width > 0) {
                    if (str < end)
                        *str = ' ';
                    ++str;
                }
            }
            c = (unsigned char)va_arg(args, int);
            if (str < end)
                *str = c;
            ++str;
            while (--spec.field_width > 0) {
                if (str < end)
                    *str = ' ';
                ++str;
            }
            break;
        }

        case FORMAT_TYPE_STR:
            str = string(str, end, va_arg(args, char *), spec);
            break;

        case FORMAT_TYPE_PTR:
            str = pointer(fmt, str, end, va_arg(args, void *), spec);
            while (isalnum(*fmt))
                fmt++;
            break;

        case FORMAT_TYPE_PERCENT_CHAR:
            if (str < end)
                *str = '%';
            ++str;
            break;

        case FORMAT_TYPE_INVALID:
            /*
             * Presumably the arguments passed gcc's type
             * checking, but there is no safe or sane way
             * for us to continue parsing the format and
             * fetching from the va_list; the remaining
             * specifiers and arguments would be out of
             * sync.
             */
            goto out;

        default:
            switch (spec.type) {
            case FORMAT_TYPE_LONG_LONG:
                num = va_arg(args, long long);
                break;
            case FORMAT_TYPE_ULONG:
                num = va_arg(args, unsigned long);
                break;
            case FORMAT_TYPE_LONG:
                num = va_arg(args, long);
                break;
            case FORMAT_TYPE_SIZE_T:
                if (spec.flags & SIGN)
                    num = va_arg(args, ssize_t);
                else
                    num = va_arg(args, size_t);
                break;
            case FORMAT_TYPE_PTRDIFF:
                num = va_arg(args, ptrdiff_t);
                break;
            case FORMAT_TYPE_UBYTE:
                num = (unsigned char)va_arg(args, int);
                break;
            case FORMAT_TYPE_BYTE:
                num = (signed char)va_arg(args, int);
                break;
            case FORMAT_TYPE_USHORT:
                num = (unsigned short)va_arg(args, int);
                break;
            case FORMAT_TYPE_SHORT:
                num = (short)va_arg(args, int);
                break;
            case FORMAT_TYPE_INT:
                num = (int)va_arg(args, int);
                break;
            default:
                num = va_arg(args, unsigned int);
            }

            str = number(str, end, num, spec);
        }
    }

out:
    if (size > 0) {
        if (str < end)
            *str = '\0';
        else
            end[-1] = '\0';
    }

    /* the trailing null byte doesn't count towards the total */
    return str - buf;
}

/**
 * vscnprintf - Format a string and place it in a buffer
 * @buf: The buffer to place the result into
 * @size: The size of the buffer, including the trailing null space
 * @fmt: The format string to use
 * @args: Arguments for the format string
 *
 * The return value is the number of characters which have been written into
 * the @buf not including the trailing '\0'. If @size is == 0 the function
 * returns 0.
 *
 * If you're not already dealing with a va_list consider using scnprintf().
 *
 * See the vsnprintf() documentation for format string extensions over C99.
 */
int vscnprintf(char *buf, size_t size, const char *fmt, va_list args) {
    int i;

    if (!size)
        return 0;

    i = vsnprintf(buf, size, fmt, args);

    if (i < size)
        return i;

    return size - 1;
}

/**
 * snprintf - Format a string and place it in a buffer
 * @buf: The buffer to place the result into
 * @size: The size of the buffer, including the trailing null space
 * @fmt: The format string to use
 * @...: Arguments for the format string
 *
 * The return value is the number of characters which would be
 * generated for the given input, excluding the trailing null,
 * as per ISO C99.  If the return is greater than or equal to
 * @size, the resulting string is truncated.
 *
 * See the vsnprintf() documentation for format string extensions over C99.
 */
int snprintf(char *buf, size_t size, const char *fmt, ...) {
    va_list args;
    int i;

    va_start(args, fmt);
    i = vsnprintf(buf, size, fmt, args);
    va_end(args);

    return i;
}

/**
 * scnprintf - Format a string and place it in a buffer
 * @buf: The buffer to place the result into
 * @size: The size of the buffer, including the trailing null space
 * @fmt: The format string to use
 * @...: Arguments for the format string
 *
 * The return value is the number of characters written into @buf not including
 * the trailing '\0'. If @size is == 0 the function returns 0.
 */

int scnprintf(char *buf, size_t size, const char *fmt, ...) {
    va_list args;
    int i;

    va_start(args, fmt);
    i = vscnprintf(buf, size, fmt, args);
    va_end(args);

    return i;
}

int vsprintf(char *buf, const char *fmt, va_list args) {
    return vsnprintf(buf, INT_MAX, fmt, args);
}

int sprintf(char *buf, const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    int i = vsnprintf(buf, INT_MAX, fmt, args);
    va_end(args);
    return i;
}

int vsscanf(const char *buf, const char *fmt, va_list args) {
    const char *str = buf;
    char *next;
    char digit;
    int num = 0;
    uint8_t qualifier;
    unsigned int base;
    union {
        long long s;
        unsigned long long u;
    } val;
    int16_t field_width;
    bool is_sign;

    while (*fmt) {
        /* skip any white space in format */
        /* white space in format matches any amount of
         * white space, including none, in the input.
         */
        if (isspace(*fmt)) {
            fmt = skip_spaces(++fmt);
            str = skip_spaces(str);
        }

        /* anything that is not a conversion must match exactly */
        if (*fmt != '%' && *fmt) {
            if (*fmt++ != *str++)
                break;
            continue;
        }

        if (!*fmt)
            break;
        ++fmt;

        /* skip this conversion.
         * advance both strings to next white space
         */
        if (*fmt == '*') {
            if (!*str)
                break;
            while (!isspace(*fmt) && *fmt != '%' && *fmt) {
                /* '%*[' not yet supported, invalid format */
                if (*fmt == '[')
                    return num;
                fmt++;
            }
            while (!isspace(*str) && *str)
                str++;
            continue;
        }

        /* get field width */
        field_width = -1;
        if (isdigit(*fmt)) {
            field_width = skip_atoi(&fmt);
            if (field_width <= 0)
                break;
        }

        /* get conversion qualifier */
        qualifier = -1;
        if (*fmt == 'h' || tolower(*fmt) == 'l' || *fmt == 'z') {
            qualifier = *fmt++;
            if (qualifier == *fmt) {
                if (qualifier == 'h') {
                    qualifier = 'H';
                    fmt++;
                } else if (qualifier == 'l') {
                    qualifier = 'L';
                    fmt++;
                }
            }
        }

        if (!*fmt)
            break;

        if (*fmt == 'n') {
            /* return number of characters read so far */
            *va_arg(args, int *) = str - buf;
            ++fmt;
            continue;
        }

        if (!*str)
            break;

        base = 10;
        is_sign = false;

        switch (*fmt++) {
        case 'c': {
            char *s = (char *)va_arg(args, char *);
            if (field_width == -1)
                field_width = 1;
            do {
                *s++ = *str++;
            } while (--field_width > 0 && *str);
            num++;
        }
            continue;
        case 's': {
            char *s = (char *)va_arg(args, char *);
            if (field_width == -1)
                field_width = SHRT_MAX;
            /* first, skip leading white space in buffer */
            str = skip_spaces(str);

            /* now copy until next white space */
            while (*str && !isspace(*str) && field_width--)
                *s++ = *str++;
            *s = '\0';
            num++;
        }
            continue;
        /*
         * Warning: This implementation of the '[' conversion specifier
         * deviates from its glibc counterpart in the following ways:
         * (1) It does NOT support ranges i.e. '-' is NOT a special
         *     character
         * (2) It cannot match the closing bracket ']' itself
         * (3) A field width is required
         * (4) '%*[' (discard matching input) is currently not supported
         *
         * Example usage:
         * ret = sscanf("00:0a:95","%2[^:]:%2[^:]:%2[^:]",
         *		buf1, buf2, buf3);
         * if (ret < 3)
         *    // etc..
         */
        case '[': {
            char *s = (char *)va_arg(args, char *);
            DECLARE_BITMAP(set, 256) = {0};
            unsigned int len = 0;
            bool negate = (*fmt == '^');

            /* field width is required */
            if (field_width == -1)
                return num;

            if (negate)
                ++fmt;

            for (; *fmt && *fmt != ']'; ++fmt, ++len)
                __set_bit((uint8_t)*fmt, set);

            /* no ']' or no character set found */
            if (!*fmt || !len)
                return num;
            ++fmt;

            if (negate) {
                bitmap_complement(set, set, 256);
                /* exclude null '\0' byte */
                __clear_bit(0, set);
            }

            /* match must be non-empty */
            if (!test_bit((uint8_t)*str, set))
                return num;

            while (test_bit((uint8_t)*str, set) && field_width--)
                *s++ = *str++;
            *s = '\0';
            ++num;
        }
            continue;
        case 'o':
            base = 8;
            break;
        case 'x':
        case 'X':
            base = 16;
            break;
        case 'i':
            base = 0;
            // fallthrough
        case 'd':
            is_sign = true;
            // fallthrough
        case 'u':
            break;
        case '%':
            /* looking for '%' in str */
            if (*str++ != '%')
                return num;
            continue;
        default:
            /* invalid format; stop here */
            return num;
        }

        /* have some sort of integer conversion.
         * first, skip white space in buffer.
         */
        str = skip_spaces(str);

        digit = *str;
        if (is_sign && digit == '-') {
            if (field_width == 1)
                break;

            digit = *(str + 1);
        }

        if (!digit || (base == 16 && !isxdigit(digit)) ||
            (base == 10 && !isdigit(digit)) ||
            (base == 8 && (!isdigit(digit) || digit > '7')) ||
            (base == 0 && !isdigit(digit)))
            break;

        if (is_sign)
            val.s = simple_strntoll(
                str, field_width >= 0 ? field_width : INT_MAX, &next, base);
        else
            val.u = simple_strntoull(
                str, field_width >= 0 ? field_width : INT_MAX, &next, base);

        switch (qualifier) {
        case 'H': /* that's 'hh' in format */
            if (is_sign)
                *va_arg(args, signed char *) = val.s;
            else
                *va_arg(args, unsigned char *) = val.u;
            break;
        case 'h':
            if (is_sign)
                *va_arg(args, short *) = val.s;
            else
                *va_arg(args, unsigned short *) = val.u;
            break;
        case 'l':
            if (is_sign)
                *va_arg(args, long *) = val.s;
            else
                *va_arg(args, unsigned long *) = val.u;
            break;
        case 'L':
            if (is_sign)
                *va_arg(args, long long *) = val.s;
            else
                *va_arg(args, unsigned long long *) = val.u;
            break;
        case 'z':
            *va_arg(args, size_t *) = val.u;
            break;
        default:
            if (is_sign)
                *va_arg(args, int *) = val.s;
            else
                *va_arg(args, unsigned int *) = val.u;
            break;
        }
        num++;

        if (!next)
            break;
        str = next;
    }

    return num;
}

int sscanf(const char *buf, const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    int i = vsscanf(buf, fmt, args);
    va_end(args);
    return i;
}
