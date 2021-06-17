#include <memory.h>
#include <proc.h>

static void *pf = NULL;

extern PCB *current;

void *new_page(size_t nr_page) {
    return memset(malloc(nr_page * PGSIZE), 0, nr_page * PGSIZE);
}


static inline void *pg_alloc(int n) {
    return new_page(n / PGSIZE);
}

void free_page(void *p) {
    panic("not implement yet");
}

/* The brk() system call handler. */
int mm_brk(uintptr_t brk) {
    if (current->max_brk == 0)
        current->max_brk = brk;
    if (brk > current->max_brk) {
        if ((brk >> 12) > (current->max_brk >> 12)) {
            int num = (brk >> 12) - (current->max_brk >> 12);
            for (int i = 1; i <= num; i++)
                map(&(current->as), (void *) ((current->max_brk & ~0xfff) + (PGSIZE * i)), new_page(1), 0);
        }
        current->max_brk = brk;
    }
    return 0;
}

void init_mm() {
    pf = (void *) ROUNDUP(heap.start, PGSIZE);
    Log("free physical pages starting from %p", pf);

#ifdef HAS_VME
    vme_init(pg_alloc, free_page);
#endif
}
