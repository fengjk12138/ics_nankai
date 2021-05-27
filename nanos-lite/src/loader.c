#include <proc.h>
#include <elf.h>
#include <fs.h>
#ifdef __LP64__
# define Elf_Ehdr Elf64_Ehdr
# define Elf_Phdr Elf64_Phdr
#else
# define Elf_Ehdr Elf32_Ehdr
# define Elf_Phdr Elf32_Phdr
#endif
extern uint8_t ramdisk_start;
extern uint8_t ramdisk_end;

// 从ramdisk中`offset`偏移处的`len`字节读入到`buf`中
size_t ramdisk_read(void *buf, size_t offset, size_t len);

// 把`buf`中的`len`字节写入到ramdisk中`offset`偏移处
size_t ramdisk_write(const void *buf, size_t offset, size_t len);

// 返回ramdisk的大小, 单位为字节
size_t get_ramdisk_size();


int fs_open(const char *path, int flags, int mode);

int fs_lseek(int fd, size_t offset, int whence);

int fs_close(int fd);

int fs_read(int fd, void *buf, size_t count);

int fs_write(int fd, const void *buf, size_t len);

static uintptr_t loader(PCB *pcb, const char *filename) {
//  TODO();
    int file = fs_open(filename, 0, 0);


    Elf_Ehdr ehdr;
    fs_read(file, &ehdr, sizeof(Elf_Ehdr));
    assert(*(uint32_t *) ehdr.e_ident == 0x464c457f);
    Elf_Phdr phdr[90];
    unsigned char data[1000];
    for (int j = 0; j < 1000; j++)
        data[j] = 0;
    fs_lseek(file, ehdr.e_phoff, SEEK_SET);
    fs_read(file, phdr, sizeof(Elf_Phdr) * ehdr.e_phnum);
    for (int i = 0; i < ehdr.e_phnum; i++)
        if (phdr[i].p_type == PT_LOAD) {
            fs_lseek(file, phdr[i].p_offset, SEEK_SET);
            fs_read(file, (void *) phdr[i].p_vaddr, phdr[i].p_filesz);
//            printf("%d---\n",phdr[i].p_memsz - phdr[i].p_filesz);
            if (phdr[i].p_filesz != phdr[i].p_memsz)
                memmove((void *) (phdr[i].p_vaddr + phdr[i].p_filesz), (void *) data,
                       phdr[i].p_memsz - phdr[i].p_filesz);
        }
    return ehdr.e_entry;
}

void naive_uload(PCB *pcb, const char *filename) {
    uintptr_t entry = loader(pcb, filename);
    Log("Jump to entry = %p", entry);
    ((void (*)()) entry)();
}

