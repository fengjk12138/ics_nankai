#include <common.h>
#include "syscall.h"

enum {
    SYS_exit,
    SYS_yield,
    SYS_open,
    SYS_read,
    SYS_write,
    SYS_kill,
    SYS_getpid,
    SYS_close,
    SYS_lseek,
    SYS_brk,
    SYS_fstat,
    SYS_time,
    SYS_signal,
    SYS_execve,
    SYS_fork,
    SYS_link,
    SYS_unlink,
    SYS_wait,
    SYS_times,
    SYS_gettimeofday
};

int sys_write(int fd, void *buf, size_t count) {
    int cnt = 0;
    for (int i = 0; i < count; i++) {
        putch(*(char *) (buf + i));
        cnt++;
    }
    return cnt;
}
extern char end;

void do_syscall(Context *c) {
    uintptr_t a[4];
    a[0] = c->GPR1;
    switch (a[0]) {

        case SYS_exit:
            halt(c->GPR2);

            break;
        case SYS_yield:
            c->GPRx = 0;
            yield();
            break;
        case SYS_write:
            c->GPRx = sys_write(c->GPR2, (void *)c->GPR3, c->GPR4);
            break;
        case SYS_brk:
            end=end+c->GPR2;
            c->GPRx=0;
            break;
        default:
            panic("Unhandled syscall ID = %d", a[0]);
    }

}

