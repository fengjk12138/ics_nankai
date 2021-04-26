#include <klib.h>
#include <stdint.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

size_t strlen(const char *s) {
    const char *end = s;
    for (; *end != '\0'; ++end);
    return end - s;
}

char *strcpy(char *dst, const char *src) {
    char *end = dst;
    for (; *src != '\0'; ++src) {
        *dst = *src;
        dst++;
    }
    *dst = *src;
    return end;
}

char *strncpy(char *dst, const char *src, size_t n) {
    char *end = dst;
    for (; *src != '\0' && n != 0; ++src, --n) {
        *dst = *src;
        dst++;
    }
    for (; n != 0; --n) {
        *dst = '\0';
        dst++;
    }
    return end;
}

char *strcat(char *dst, const char *src) {
    char *end = dst;
    for (; *dst != '\0'; dst++);
    for (; *src != '\0'; dst++, src++)
        *dst = *src;
    *dst = *src;
    return end;
}

int strcmp(const char *s1, const char *s2) {
    for (; *s1 != '\0' && *s2 != '\0'; s1++, s2++)
        if ((unsigned char) (*s1) > (unsigned char) (*s2))
            return 1;
        else if ((unsigned char) (*s1) < (unsigned char) (*s2))
            return -1;
    if (*s1 == *s2)
        return 0;
    else if ((unsigned char) (*s1) > (unsigned char) (*s2))
        return 1;
    else
        return -1;
}

int strncmp(const char *s1, const char *s2, size_t n) {
    for (; *s1 != '\0' && *s2 != '\0' && n != 0; s1++, s2++, n--)
        if ((unsigned char) (*s1) > (unsigned char) (*s2))
            return 1;
        else if ((unsigned char) (*s1) < (unsigned char) (*s2))
            return -1;
    if (n == 0)
        return 0;
    if (*s1 == *s2)
        return 0;
    else if ((unsigned char) (*s1) > (unsigned char) (*s2))
        return 1;
    else return -1;
}

void *memset(void *v, int c, size_t n) {
    unsigned char *tmp_v = (unsigned char *) v;
    for (; n != 0; n--, tmp_v++)
        *tmp_v = (unsigned char) c;
    return v;
}

void *memmove(void *dst, const void *src, size_t n) {
    unsigned char *std = (unsigned char *) dst;
    const unsigned char *crs = (const unsigned char *) src;
    if (dst < src) {
        for (; n != 0; n--, std++, crs++)
            *std = *crs;
    } else if (dst > src) {
        std += n;
        crs += n;
        for (; n != 0; std--, crs--, n--)
            *std = *crs;
    }
    return dst;
}

void *memcpy(void *out, const void *in, size_t n) {
    unsigned char *std = (unsigned char *) out;
    const unsigned char *crs = (const unsigned char *) in;
    for (; n != 0; n--, std++, crs++)
        *std = *crs;
    return out;
}

int memcmp(const void *s1, const void *s2, size_t n) {
    const unsigned char *std = (const unsigned char *) s1;
    const unsigned char *crs = (const unsigned char *) s2;
    for (; n != 0; n--, std++, crs++)
        if (*std < *crs)
            return -1;
        else if (*std > *crs)
            return 1;
    return 0;
}

#endif
