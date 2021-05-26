#include <proc.h>
#include <elf.h>

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

static uintptr_t loader(PCB *pcb, const char *filename) {
//  TODO();
    Elf_Ehdr ehdr;
    ramdisk_read(&ehdr, 0, sizeof(Elf_Ehdr));
    assert(*(uint32_t *) ehdr.e_ident == 0x464c457f);
    Elf_Phdr phdr[2048];
    unsigned char data[4096];
    for (int j = 0; j < 4096; j++)
        data[j] = 0;
    ramdisk_read(phdr, ehdr.e_phoff, sizeof(Elf_Phdr) * ehdr.e_phnum);
    for (int i = 0; i < ehdr.e_phnum; i++)
        if (phdr[i].p_type == PT_LOAD) {
            ramdisk_read((void *) phdr[i].p_vaddr, phdr[i].p_offset, phdr[i].p_filesz);
            if (phdr[i].p_filesz != phdr[i].p_memsz)
                memcpy((void *) (phdr[i].p_vaddr + phdr[i].p_filesz), (void *)data, phdr[i].p_memsz - phdr[i].p_filesz);
        }
    return ehdr.e_entry;
}

void naive_uload(PCB *pcb, const char *filename) {
    uintptr_t entry = loader(pcb, filename);
    Log("Jump to entry = %p", entry);
    ((void (*)()) entry)();
}

