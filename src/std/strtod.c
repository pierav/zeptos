/* Copyright (C) 1991-1992, 1997, 1999, 2003, 2006, 2008-2016 Free Software
   Foundation, Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

#include <config.h>

#include <stdlib.h>

#include "printk.h"
#include <ctype.h>
#include <errno.h>
#include <float.h>
#include <limits.h>
#include <math.h>
#include <stdbool.h>
#include <string.h>

/* Return true if C is a space in the current locale, avoiding
   problems with signed char and isspace.  */
static bool locale_isspace(char c) {
    unsigned char uc = c;
    return isspace(uc) != 0;
}

/* Return X * BASE**EXPONENT.  Return an extreme value and set errno
   to ERANGE if underflow or overflow occurs.  */
static double scale_radix_exp(double x, int radix, long int exponent) {
    /* If RADIX == 10, this code is neither precise nor fast; it is
       merely a straightforward and relatively portable approximation.
       If N == 2, this code is precise on a radix-2 implementation,
       albeit perhaps not fast if ldexp is not in libc.  */

    long int e = exponent;
    double r = x;

    if (r != 0) {
        if (e < 0) {
            while (e++ != 0) {
                r /= radix;
                if (r == 0 && x != 0) {
                    errno = ERANGE;
                    break;
                }
            }
        } else {
            while (e-- != 0) {
                if (r < -DBL_MAX / radix) {
                    errno = ERANGE;
                    return -HUGE_VAL;
                } else if (DBL_MAX / radix < r) {
                    errno = ERANGE;
                    return HUGE_VAL;
                } else
                    r *= radix;
            }
        }
    }

    return r;
}

/* Parse a number at NPTR; this is a bit like strtol (NPTR, ENDPTR)
   except there are no leading spaces or signs or "0x", and ENDPTR is
   nonnull.  The number uses a base BASE (either 10 or 16) fraction, a
   radix RADIX (either 10 or 2) exponent, and exponent character
   EXPCHAR.  To convert from a number of digits to a radix exponent,
   multiply by RADIX_MULTIPLIER (either 1 or 4).  */
static double parse_number(const char *nptr, int base, int radix,
                           int radix_multiplier, char expchar, char **endptr) {
    const char *s = nptr;
    bool got_dot = false;
    long int exponent = 0;
    double num = 0;

    for (;; ++s) {
        int digit;
        if (isdigit(*s))
            digit = *s - '0';
        else if (base == 16 && isxdigit(*s))
            digit = tolower(*s) - ('a' - 10);
        else if (!got_dot && *s == '.') {
            /* Record that we have found the decimal point.  */
            got_dot = true;
            continue;
        } else
            /* Any other character terminates the number.  */
            break;

        /* Make sure that multiplication by base will not overflow.  */
        if (num <= DBL_MAX / base)
            num = num * base + digit;
        else {
            /* The value of the digit doesn't matter, since we have already
               gotten as many digits as can be represented in a 'double'.
               This doesn't necessarily mean the result will overflow.
               The exponent may reduce it to within range.

               We just need to record that there was another
               digit so that we can multiply by 10 later.  */
            exponent += radix_multiplier;
        }

        /* Keep track of the number of digits after the decimal point.
           If we just divided by base here, we might lose precision.  */
        if (got_dot)
            exponent -= radix_multiplier;
    }
    if (tolower(*s) == expchar && !locale_isspace(s[1])) {
        /* Add any given exponent to the implicit one.  */
        int save = errno;
        char *end;
        long int value = strtol(s + 1, &end, 10);
        errno = save;

        if (s + 1 != end) {
            /* Skip past the exponent, and add in the implicit exponent,
               resulting in an extreme value on overflow.  */
            s = end;
            exponent = (exponent < 0
                            ? (value < LONG_MIN - exponent ? LONG_MIN
                                                           : exponent + value)
                            : (LONG_MAX - exponent < value ? LONG_MAX
                                                           : exponent + value));
        }
    }

    *endptr = (char *)s;
    // printk("num=%d, exp=%d\n", (int)num, exponent);
    return scale_radix_exp(num, radix, exponent);
}

double minus_zero = -0.0;

/* Convert NPTR to a double.  If ENDPTR is not NULL, a pointer to the
   character after the last one used in the number is put in *ENDPTR.  */
double strtod(const char *nptr, char **endptr) {
    // return parse_number (nptr, 10, 10, 1, 'e', endptr);

    bool negative = false;

    /* The number so far.  */
    double num;

    const char *s = nptr;
    const char *end;
    char *endbuf;
    int saved_errno;

    /* Eat whitespace.  */
    while (locale_isspace(*s))
        ++s;

    /* Get the sign.  */
    negative = *s == '-';
    if (*s == '-' || *s == '+')
        ++s;

    saved_errno = errno;

    num = 0;
    end = s;

    if (isdigit(s[*s == '.'])) {
        /* If a hex float was converted incorrectly, do it ourselves.
           If the string starts with "0x" but does not contain digits,
           consume the "0" ourselves.  If a hex float is followed by a
           'p' but no exponent, then adjust the end pointer.  */
        if (*s == '0' && tolower(s[1]) == 'x') {
            if (!isxdigit(s[2 + (s[2] == '.')]))
                end = s + 1;
            else if (end <= s + 2) {
                num = parse_number(s + 2, 16, 2, 4, 'p', &endbuf);
                end = endbuf;
            } else {
                const char *p = s + 2;
                while (p < end && tolower(*p) != 'p')
                    p++;
                if (p < end && !isdigit(p[1 + (p[1] == '-' || p[1] == '+')]))
                    end = p;
            }
        } else {
            num = parse_number(s, 10, 10, 1, 'e', &endbuf);
            end = endbuf;
        }
        s = end;
    }

    /* Check for infinities and NaNs.  */
    else if (tolower(*s) == 'i' && tolower(s[1]) == 'n' &&
             tolower(s[2]) == 'f') {
        s += 3;
        if (tolower(*s) == 'i' && tolower(s[1]) == 'n' &&
            tolower(s[2]) == 'i' && tolower(s[3]) == 't' &&
            tolower(s[4]) == 'y')
            s += 5;
        num = HUGE_VAL;
        errno = saved_errno;
    } else if (tolower(*s) == 'n' && tolower(s[1]) == 'a' &&
               tolower(s[2]) == 'n') {
        s += 3;
        if (*s == '(') {
            const char *p = s + 1;
            while (isalnum(*p))
                p++;
            if (*p == ')')
                s = p + 1;
        }

        /* If the underlying implementation misparsed the NaN, assume
           its result is incorrect, and return a NaN.  Normally it's
           better to use the underlying implementation's result, since a
           nice implementation populates the bits of the NaN according
           to interpreting n-char-sequence as a hexadecimal number.  */
        if (s != end)
            num = NAN;
        errno = saved_errno;
    } else {
        /* No conversion could be performed.  */
        errno = EINVAL;
        s = nptr;
    }

    if (endptr != NULL)
        *endptr = (char *)s;
    if (!num && negative)
        return minus_zero;

    double ret = negative ? -num : num;
    // printk("%.*s -> %f\n", s - nptr, nptr, ret);
    return ret;
}
