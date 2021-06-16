#include <isa.h>
#include <memory/vaddr.h>
#include <memory/paddr.h>

int isa_vaddr_check(vaddr_t vaddr, int type, int len) {
    if (cpu.cr0 & (1 << 31))
        return MEM_RET_NEED_TRANSLATE;
    else
        return MEM_RET_OK;
}

uintptr_t get_page(vaddr_t vaddr) {
    uintptr_t page_dir = paddr_read(cpu.cr3 + (vaddr >> 22) * 4, 4);
    uintptr_t pg_base = paddr_read(page_dir + ((vaddr >> 12) & 0x3ff)*4, 4);
//    printf("%d pgbase=%x\n", (vaddr >> 22),cpu.cr3 + (vaddr >> 22));
//    assert((pg_base & 0xfff) == 0);
    return pg_base;
}

paddr_t isa_mmu_translate(vaddr_t vaddr, int type, int len) {

    uintptr_t page1 = get_page(vaddr);
    uintptr_t page2 = get_page(vaddr + len - 1);
//    assert((uintptr_t)cpu.cr3==0x441000);
//    printf("%x\n", *cpu.cr3);
//    return 0;
//    uintptr_t page1 = ((uintptr_t *)(*(uintptr_t *) ((uintptr_t)cpu.cr3[vaddr >> 22])))[(vaddr >> 12) & (0x3ff)];
//    vaddr += len - 1;
//    uintptr_t page2 = ((uintptr_t *)(*(uintptr_t *) ((uintptr_t)cpu.cr3[vaddr >> 22])))[(vaddr >> 12) & (0x3ff)];
    if (page1 != page2)
        return MEM_RET_CROSS_PAGE | page1;
    else
        return MEM_RET_OK | page1;
}
