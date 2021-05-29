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
    FILE *fp = fopen("/dev/events", "r");
    int tmp = fread(buf, 1, len, fp) ? 1 : 0;
    fclose(fp);
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
    FILE *fp = fopen("/proc/dispinfo", "r");
    fscanf(fp, "WIDTH : %d\n HEIGHT : %d", &screen_w, &screen_h);
    fclose(fp);
    if (*w == 0 && *h == 0) {
        *w = screen_w;
        *h = screen_h;
    }
    Canvas_w = *w;
    Canvas_h = *h;
//    printf("%d %d\n", *w, *h);
}

void NDL_DrawRect(uint32_t *pixels, int x, int y, int w, int h) {
    FILE *fp = fopen("/dev/fb", "w");
    for (int i = 0; i < h; i++) {
        fseek(fp, (screen_w * (y + i) + x + 1) * 4, SEEK_SET);
        fwrite(pixels + Canvas_w * i, sizeof(uint32_t), w, fp);
//        printf("len %d\n",i);
    }
    fclose(fp);
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
