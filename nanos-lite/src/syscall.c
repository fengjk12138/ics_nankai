#include <common.h>
#include "syscall.h"

enum {
    FD_STDIN, FD_STDOUT, FD_STDERR, FD_FB
};

int fs_open(const char *path, int flags, int mode);

int fs_lseek(int fd, size_t offset, int whence);

int fs_close(int fd);

int fs_read(int fd, void *buf, size_t count);

int fs_write(int fd, const void *buf, size_t len);

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
    a[1] = c->GPR2;
    a[2] = c->GPR3;
    a[3] = c->GPR4;
    switch (a[0]) {

        case SYS_exit:
            halt(a[1]);

            break;
        case SYS_yield:
            c->GPRx = 0;
            yield();
            break;
        case SYS_open:
            c->GPRx = fs_open((const char *) a[1], a[2], a[3]);
            break;
        case SYS_read:
            c->GPRx = fs_read(a[1], (void *) a[2], a[3]);
            break;
        case SYS_write:
            if (a[1] == FD_STDOUT || a[1] == FD_STDERR)
                c->GPRx = sys_write(a[1], (void *) a[2], a[3]);
            else if (a[1] == FD_STDIN)
                panic("can't write to stdin");
            else
                c->GPRx = fs_write(a[1], (const void *) a[2], a[3]);
            break;
        case SYS_close:
            c->GPRx = fs_close(a[1]);
            break;
        case SYS_lseek:
            c->GPRx = fs_lseek(a[1], a[2], a[3]);
            break;
        case SYS_brk:
            end = end + a[1];
            c->GPRx = 0;
            break;

        default:
            panic("Unhandled syscall ID = %d", a[0]);
    }

}

