#include <am.h>
#include <nemu.h>
#include <klib.h>

static AddrSpace kas = {};

static void *(*pgalloc_usr)(int) = NULL;

static void (*pgfree_usr)(void *) = NULL;

static int vme_enable = 0;

static Area segments[] = {      // Kernel memory mappings
        NEMU_PADDR_SPACE
};

#define USER_SPACE RANGE(0x40000000, 0xc0000000)

bool vme_init(void *(*pgalloc_f)(int), void (*pgfree_f)(void *)) {
    pgalloc_usr = pgalloc_f;
    pgfree_usr = pgfree_f;

    kas.ptr = pgalloc_f(PGSIZE);

    int i;
    for (i = 0; i < LENGTH(segments); i++) {

        void *va = segments[i].start;
        for (; va < segments[i].end; va += PGSIZE) {
            map(&kas, va, va, 0);
        }
//        printf("begin=%x  end=%x\n",segments[i].start,segments[i].end);
    }
    set_cr3(kas.ptr);
    set_cr0(get_cr0() | CR0_PG);

    vme_enable = 1;

    return true;
}

void protect(AddrSpace *as) {
    PTE *updir = (PTE * )(pgalloc_usr(PGSIZE));
    as->ptr = updir;
    as->area = USER_SPACE;
    as->pgsize = PGSIZE;
    // map kernel space
    memcpy(updir, kas.ptr, PGSIZE);
}

void unprotect(AddrSpace *as) {
}

void __am_get_cur_as(Context *c) {
    c->cr3 = (vme_enable ? (void *) get_cr3() : NULL);
}

void __am_switch(Context *c) {
    if (vme_enable && c->cr3 != NULL) {
        set_cr3(c->cr3);
    }
}

void map(AddrSpace *as, void *va, void *pa, int prot) {
//    printf("va=%p\n",(uintptr_t)va>>22);
    assert(((uintptr_t)pa & 0xfff)==0);
    if (((uintptr_t *)(as->ptr))[(uintptr_t)va >> 22] == 0) {
        ((uintptr_t *)(as->ptr))[(uintptr_t)va >> 22] = (uintptr_t)pgalloc_usr(PGSIZE);
    }
    uintptr_t* page_dir=(uintptr_t *)((uintptr_t *)(as->ptr))[(uintptr_t)va >> 22];
    page_dir[((uintptr_t)va >> 12) & (0x3ff)] = (uintptr_t)pa;

}

Context *ucontext(AddrSpace *as, Area kstack, void *entry) {
    Context save;
    save.eip = (uintptr_t) entry;
    save.cr3=as->ptr;
//    save.eax = (uintptr_t) arg;
    save.cs = 8;

    *(Context * )(kstack.end - sizeof(Context)) = save;
//    *(uintptr_t * )(kstack.end - sizeof(uintptr_t)) = (uintptr_t) arg;
    return kstack.end - sizeof(Context);
}
