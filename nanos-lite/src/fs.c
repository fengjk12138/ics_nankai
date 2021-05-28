#include <fs.h>

typedef size_t (*ReadFn)(void *buf, size_t offset, size_t len);

typedef size_t (*WriteFn)(const void *buf, size_t offset, size_t len);

// 从ramdisk中`offset`偏移处的`len`字节读入到`buf`中
size_t ramdisk_read(void *buf, size_t offset, size_t len);

// 把`buf`中的`len`字节写入到ramdisk中`offset`偏移处
size_t ramdisk_write(const void *buf, size_t offset, size_t len);

// 返回ramdisk的大小, 单位为字节
size_t get_ramdisk_size();

typedef struct {
    char *name;
    size_t size;
    size_t disk_offset;
    ReadFn read;
    WriteFn write;
    size_t open_offset;
} Finfo;

enum {
    FD_STDIN, FD_STDOUT, FD_STDERR, FD_FB
};
size_t serial_write(const void *buf, size_t offset, size_t len);
size_t invalid_read(void *buf, size_t offset, size_t len) {
    panic("should not reach here");
    return 0;
}

size_t invalid_write(const void *buf, size_t offset, size_t len) {
    panic("should not reach here");
    return 0;
}

/* This is the information about all files in disk. */
static Finfo file_table[] __attribute__((used)) = {
        [FD_STDIN]  = {"stdin", 0, 0, invalid_read, invalid_write},
        [FD_STDOUT] = {"stdout", 0, 0, invalid_read, serial_write},
        [FD_STDERR] = {"stderr", 0, 0, invalid_read, serial_write},

#include "files.h"
};

void init_fs() {
    // TODO: initialize the size of /dev/fb

}

int fs_open(const char *path, int flags, int mode) {
    for (int i = 3; i < sizeof(file_table) / sizeof(Finfo); i++)
        if (strcmp(file_table[i].name, path) == 0) {
            file_table[i].open_offset = 0;
            return i;
        }
    panic("can't find this file");
    return -1;
}

int fs_read(int fd, void *buf, size_t count) {
    ramdisk_read(buf, file_table[fd].disk_offset + file_table[fd].open_offset, count);
    file_table[fd].open_offset += count;
    return count;
}

int fs_write(int fd, const void *buf, size_t len) {
    if (file_table[fd].write == NULL)
        ramdisk_write(buf, file_table[fd].disk_offset + file_table[fd].open_offset, len);
    else file_table[fd].write(buf, file_table[fd].disk_offset + file_table[fd].open_offset, len);
    file_table[fd].open_offset += len;
    return len;
}

int fs_lseek(int fd, size_t offset, int whence) {
    if (whence == SEEK_SET)
        file_table[fd].open_offset = offset;
    else if (whence == SEEK_CUR)
        file_table[fd].open_offset += offset;
    else if (whence == SEEK_END)
        file_table[fd].open_offset = file_table[fd].size - offset;
    if (file_table[fd].open_offset <= file_table[fd].size && file_table[fd].open_offset >= 0)
        return file_table[fd].open_offset;
    else return -1;
}

int fs_close(int fd) {
    file_table[fd].open_offset = 0;
    return 0;
}

