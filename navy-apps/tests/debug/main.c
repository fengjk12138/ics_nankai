#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <NDL.h>
#include <BMP.h>

struct BitmapHeader {
    uint16_t type;
    uint32_t filesize;
    uint32_t resv_1;
    uint32_t offset;
    uint32_t ih_size;
    uint32_t width;
    uint32_t height;
    uint16_t planes;
    uint16_t bitcount; // 1, 4, 8, or 24
    uint32_t compression;
    uint32_t sizeimg;
    uint32_t xres, yres;
    uint32_t clrused, clrimportant;
} __attribute__((packed));

void *my_BMP_Load(const char *filename, int *width, int *height) {
    FILE *fp = fopen(filename, "r");
    if (!fp) return NULL;

    int line_off = 384;
    int offset = 138;
    int w = 128, h = 128;
    uint32_t *pixels = malloc(w * h * sizeof(uint32_t));
    uint32_t *pixels2 = malloc(w * h * sizeof(uint32_t));
//    printf("%d\n", w * h * sizeof(uint32_t));
//    pixels[1] = 1;
//    pixels2[1] = 0;
//    printf("pixels1 = %x %x\n", (int)&pixels[1],(int)&pixels2[1]);
//    printf("%p %p", pixels, pixels2);
//    printf("offset=%d %d %d\n",offset, line_off,h);
    printf("%d %d\n", offset + (h - 1 - 0) * line_off, w*3);
    fseek(fp, 48906, SEEK_SET);
    printf("就这1\n");
    int nread = fread(&pixels[0], 3, w, fp);
    printf("就这2\n");
//    fseek(fp, offset + (h - 1 - 1) * line_off, SEEK_SET);
//    fseek(fp, 49290-1, SEEK_SET);
//    for (int i = 1; i < h; i ++) {
//
//        fseek(fp, offset + (h - 1 - i) * line_off, SEEK_SET);
//        printf("fw\n");
//        int nread = fread(&pixels[w * i], 3, w, fp);
//
//    }
    fclose(fp);
    printf("\n就这3\n");
    return pixels;
}


int main() {
    printf("new\n");
    NDL_Init(0);
    int w=1, h=2;
    void *bmp = my_BMP_Load("/share/pictures/projectn.bmp", &w, &h);
    printf("\n就这4\n");
    return 0;
}
