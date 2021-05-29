#include <common.h>

#if defined(MULTIPROGRAM) && !defined(TIME_SHARING)
# define MULTIPROGRAM_YIELD() yield()
#else
# define MULTIPROGRAM_YIELD()
#endif

#define NAME(key) \
  [AM_KEY_##key] = #key,

static const char *keyname[256] __attribute__((used)) = {
        [AM_KEY_NONE] = "NONE",
        AM_KEYS(NAME)
};

size_t serial_write(const void *buf, size_t offset, size_t len) {
    int cnt = 0;
    for (int i = 0; i < len; i++) {
        putch(*(char *) (buf + i));
        cnt++;
    }
    return cnt;
}

size_t events_read(void *buf, size_t offset, size_t len) {
    AM_INPUT_KEYBRD_T ev = io_read(AM_INPUT_KEYBRD);
    if (ev.keycode == AM_KEY_NONE)
        return 0;
    sprintf(buf, "k%s %s\n", ev.keydown ? "d" : "u", keyname[ev.keycode]);
//    printf("%s", buf);
    return strlen(buf) + 1;
}

size_t dispinfo_read(void *buf, size_t offset, size_t len) {
    int w = io_read(AM_GPU_CONFIG).width;
    int h = io_read(AM_GPU_CONFIG).height;
    sprintf(buf, "WIDTH : %d\n HEIGHT : %d", w, h);
    return strlen(buf) + 1;
}

size_t fb_write(const void *buf, size_t offset, size_t len) {
    int w = io_read(AM_GPU_CONFIG).width;
//    int h = io_read(AM_GPU_CONFIG).height;
    int x, y;
    if (offset % (w * 4) == 0)
        y = offset / (w * 4) - 1, x = w - 1;
    else x = offset % (w * 4) - 1, y = offset / (w * 4);
    for (int i = 0; i < len; i++) {
        io_write(AM_GPU_FBDRAW, x, y, (void *)buf + i, 1, 1, false);
        if (x == w - 1)
            x = 0, y++;
        else x++;
    }
    io_write(AM_GPU_FBDRAW, 0, 0, NULL, 0, 0, true);
    return len;
}

void init_device() {
    Log("Initializing devices...");
    ioe_init();
}
