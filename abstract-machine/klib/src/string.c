#include <klib.h>
#include <stdint.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

size_t strlen(const char *s) {
    const char* end = s;
    for( ; *end != '\0'; ++end)
        ;
    return end - s;
}

char *strcpy(char* dst,const char* src) {
    char* end = dst;
    for(; *src != '\0'; ++src) {
        *dst = *src;
        dst++;
    }
    *dst = *src;
    return end;
}

char* strncpy(char* dst, const char* src, size_t n) {
    char* end = dst;
    for(; *src != '\0' && n != 0; ++src, --n) {
        *dst = *src;
        dst++;
    }
    for(; n != 0; --n) {
        *dst = '\0';
        dst++;
    }
    return end;
}

char* strcat(char* dst, const char* src) {
    return NULL;
}

int strcmp(const char* s1, const char* s2) {
    return 0;
}

int strncmp(const char* s1, const char* s2, size_t n) {
    return 0;
}

void* memset(void* v,int c,size_t n) {
    return NULL;
}

void* memmove(void* dst,const void* src,size_t n) {
    return NULL;
}

void* memcpy(void* out, const void* in, size_t n) {
//    return NULL;
    
}

int memcmp(const void* s1, const void* s2, size_t n) {
    return 0;
}

#endif
