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
    sprintf(buf, "k%s %s\n", keyname[ev.keycode], ev.keycode, ev.keydown ? "d" : "u");
    printf("%s", buf);
    return strlen(buf) + 1;
}

#define N 32

size_t dispinfo_read(void *buf, size_t offset, size_t len) {
    int w = io_read(AM_GPU_CONFIG).width / N;
    int h = io_read(AM_GPU_CONFIG).height / N;
    sprintf(buf, "WIDTH : %d\n HEIGHT : %d", w, h);
    return strlen(buf) + 1;
}

size_t fb_write(const void *buf, size_t offset, size_t len) {
    return 0;
}

void init_device() {
    Log("Initializing devices...");
    ioe_init();
}
