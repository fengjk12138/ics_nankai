#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static int evtdev = -1;
static int fbdev = -1;
static int screen_w = 0, screen_h = 0;
int Canvas_w, Canvas_h;

uint32_t NDL_GetTicks() {
//  return 0;
    uint32_t out[2];
    gettimeofday(out, NULL);
    return out[1] / 1000;
}

int NDL_PollEvent(char *buf, int len) {
    int fp = open("/dev/events", 1);
    int tmp = read(fp, buf, len) ? 1 : 0;
    close(fp);
    return tmp;
}

void NDL_OpenCanvas(int *w, int *h) {
    if (getenv("NWM_APP")) {
        int fbctl = 4;
        fbdev = 5;
        screen_w = *w;
        screen_h = *h;
        char buf[64];
        int len = sprintf(buf, "%d %d", screen_w, screen_h);
        // let NWM resize the window and create the frame buffer
        write(fbctl, buf, len);
        while (1) {
            // 3 = evtdev
            int nread = read(3, buf, sizeof(buf) - 1);
            if (nread <= 0) continue;
            buf[nread] = '\0';
            if (strcmp(buf, "mmap ok") == 0) break;
        }
        close(fbctl);
    }

//    FILE *fp = fopen("/proc/dispinfo", "r");
//    fscanf(fp, "WIDTH: %d\nHEIGHT: %d\n", &screen_w, &screen_h);
//    printf("%d %d\n",screen_w,screen_h);
//    fclose(fp);

    int fd = open("/proc/dispinfo", 1);
    char buf[512];
    read(fd, buf, sizeof(buf));
    sscanf(buf, "WIDTH: %d\nHEIGHT: %d\n", &screen_w, &screen_h);
    close(fd);
//    printf("%d %d\n",screen_w,screen_h);
    if (*w == 0 && *h == 0) {
        *w = screen_w;
        *h = screen_h;
    }
    Canvas_w = *w;
    Canvas_h = *h;
}

void NDL_DrawRect(uint32_t *pixels, int x, int y, int w, int h) {
    int fd = open("/dev/fb", 1);
    if (h == 0 || h > Canvas_h)
        h = Canvas_h;
    if (w == 0 || w > Canvas_w)
        w = Canvas_w;
//    printf("xywh %d %d %d %d\n",x,y,w,h);
    for (int i = 0; i < h; i++) {
        lseek(fd, (screen_w * (y + i) + x) * 4, SEEK_SET);
        write(fd, pixels + Canvas_w * i, sizeof(uint32_t) * w);
//        printf("line=%d\n",i);

    }
    close(fd);
}

void NDL_OpenAudio(int freq, int channels, int samples) {
}

void NDL_CloseAudio() {
}

int NDL_PlayAudio(void *buf, int len) {
    return 0;
}

int NDL_QueryAudio() {
    return 0;
}

int NDL_Init(uint32_t flags) {
    if (getenv("NWM_APP")) {
        evtdev = 3;
    }
    return 0;
}

void NDL_Quit() {
}
