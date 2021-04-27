#include <am.h>
#include <klib.h>
#include <klib-macros.h>
#include <stdarg.h>
#include "stdlib.c"

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

int printf(const char *fmt, ...) {
    panic_on(1, "no support printf--------\n");
    return 0;
}

int vsprintf(char *out, const char *fmt, va_list ap) {
    panic_on(1, "no support vsprintf--------\n");
    return 0;
}

int sprintf(char *out, const char *fmt, ...) {
//  return 0;
    int cnt=0;
    va_list ap;
    *out = '\0';
    va_start(ap, fmt);
    while (*fmt) {
        const char *tmp_char;
        int tmp_int;
        if (*fmt == '%') {
            fmt++;
            switch (*fmt) {
                case 's':
                    tmp_char = va_arg(ap, const char *);
                    strcat(out, tmp_char);
                    break;
                case 'd':
                    tmp_int = va_arg(ap, int);
                    char tmp[22];
                    strcat(out, itoa(tmp_int, tmp, 10));
                    break;
                default:
                    return -1;
            }
            for(;*out!='\0';out++,cnt++);
        } else {
            *out = *fmt;
            out++;
            *out = '\0';
            cnt+=1;
        }
        fmt++;
    }
    return cnt;
}

int snprintf(char *out, size_t n, const char *fmt, ...) {
    panic_on(1, "no support snprintf--------\n");
    return 0;
}

int vsnprintf(char *out, size_t n, const char *fmt, va_list ap) {
    panic_on(1, "no support vsnprintf--------\n");
    return 0;
}

#endif
