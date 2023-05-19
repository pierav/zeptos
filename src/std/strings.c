#include <ctype.h>
#include <strings.h>

int strcasecmp(const char *s1, const char *s2) {
    const unsigned char *us1 = (const unsigned char *)s1,
                        *us2 = (const unsigned char *)s2;

    while (tolower(*us1) == tolower(*us2++))
        if (*us1++ == '\0')
            return (0);
    return (tolower(*us1) - tolower(*--us2));
}

int strncasecmp(const char *s1, const char *s2, size_t n) {
    if (n != 0) {
        const unsigned char *us1 = (const unsigned char *)s1,
                            *us2 = (const unsigned char *)s2;

        do {
            if (tolower(*us1) != tolower(*us2++))
                return (tolower(*us1) - tolower(*--us2));
            if (*us1++ == '\0')
                break;
        } while (--n != 0);
    }
    return (0);
}

void bzero(void *b, size_t length) { memset((void *)b, 0, (size_t)length); }

void bcopy(const void *src, void *dest, size_t len) { memmove(dest, src, len); }