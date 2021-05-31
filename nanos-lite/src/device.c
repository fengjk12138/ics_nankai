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
    sprintf(buf, "WIDTH: %d\nHEIGHT: %d\n", w, h);
    return strlen(buf);
}

size_t fb_write(const void *buf, size_t offset, size_t len) {
//    printf("%d--- %d---\n", offset, len);
    int w = io_read(AM_GPU_CONFIG).width;
//    int h = io_read(AM_GPU_CONFIG).height;
//    printf("w=%d h=%d\n", w, h);

    int x, y;
    offset /= 4;
    offset++;
    if (offset % (w) == 0)
        y = offset / (w) - 1, x = w - 1;
    else x = offset % (w) - 1, y = offset / (w);
//    printf("x=%d y=%d\n", x, y);
    for (int i = 0; i < len / 4; i++) {
        io_write(AM_GPU_FBDRAW, x, y, (void *) buf + i * 4, 1, 1, false);
        if (x == w - 1)
            x = 0, y++;
        else x++;
    }
    io_write(AM_GPU_FBDRAW, 0, 0, NULL, 0, 0, true);
//    printf("write over\n");
    return len;


//    offset /= sizeof(uint32_t); // 4 bytes per pixel
//    int w = io_read(AM_GPU_CONFIG).width;
//    int h = io_read(AM_GPU_CONFIG).height;
//    int x = offset % w;
//    int y = offset / w;
//    if (offset + len > w * h * sizeof(uint32_t))
//        len = w * h * sizeof(uint32_t) - offset;
//    io_write(AM_GPU_FBDRAW, x, y, (void *)buf, len / sizeof(uint32_t), 1, true); // one line
//
//    return len;

}

void init_device() {
    Log("Initializing devices...");
    ioe_init();
}
