#include <common.h>
#include "syscall.h"

enum {
    FD_STDIN, FD_STDOUT, FD_STDERR, FD_FB
};

Context *schedule(Context *prev);

void naive_uload(void *pcb, const char *filename);

void context_uload(PCB *start, const char *filename, char *const argv[], char *const envp[]);

int fs_open(const char *path, int flags, int mode);

int fs_lseek(int fd, size_t offset, int whence);

int fs_close(int fd);

int fs_read(int fd, void *buf, size_t count);

int fs_write(int fd, const void *buf, size_t len);

int sys_gettimeofday(unsigned int *tv, unsigned int *tz) {
    tv[1] = io_read(AM_TIMER_UPTIME).us;
    AM_TIMER_RTC_T rtc;
    rtc = io_read(AM_TIMER_RTC);
    tv[0] = rtc.second;
    tz[0] = rtc.minute;
    tz[1] = rtc.minute + 60;
    return 1;
}


void do_syscall(Context *c) {
    uintptr_t a[4];
    a[0] = c->GPR1;
    a[1] = c->GPR2;
    a[2] = c->GPR3;
    a[3] = c->GPR4;
    switch (a[0]) {
        case SYS_exit:
            halt(a[1]);
            break;
        case SYS_yield:
            yield();
            c->GPRx = 0;
            break;
        case SYS_open:
            c->GPRx = fs_open((const char *) a[1], a[2], a[3]);
            break;
        case SYS_read:
            c->GPRx = fs_read(a[1], (void *) a[2], a[3]);
            break;
        case SYS_write:
            c->GPRx = fs_write(a[1], (const void *) a[2], a[3]);
            break;
        case SYS_close:
            c->GPRx = fs_close(a[1]);
            break;
        case SYS_lseek:
            c->GPRx = fs_lseek(a[1], a[2], a[3]);
            break;
        case SYS_brk:
            c->GPRx = 0;
            break;
        case SYS_execve:
//            naive_uload(NULL, (void *) a[1]);
            context_uload(, (void *) a[1], (void *) a[2], (void *) a[3]);
            break;
        case SYS_gettimeofday:
            c->GPRx = sys_gettimeofday((void *) a[1], (void *) a[2]);
            break;
        default:
            panic("Unhandled syscall ID = %d", a[0]);
    }

}

