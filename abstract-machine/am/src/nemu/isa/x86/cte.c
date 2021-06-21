#include <am.h>
#include <x86.h>
#include <klib.h>

#define NR_IRQ         256     // IDT size
#define SEG_KCODE      1
#define SEG_KDATA      2


static Context *(*user_handler)(Event, Context *) = NULL;

void __am_irq0();

void __am_vecsys();

void __am_vectrap();

void __am_vecnull();

void __am_get_cur_as(Context *c);

void __am_switch(Context *c);

Context *__am_irq_handle(Context *c) {
    __am_get_cur_as(c);
    if (user_handler) {
        Event ev = {0};
        switch (c->irq) {
            case 32:
                ev.event = EVENT_IRQ_TIMER;
                break;
            case 0x80:
                ev.event = EVENT_SYSCALL;
                break;
            case 0x81:
                ev.event = EVENT_YIELD;
                break;
            default:
                ev.event = EVENT_ERROR;
                break;
        }

        c = user_handler(ev, c);
        assert(c != NULL);
    }

    __am_switch(c);
//    printf("cr3=%x\n",get_cr3());
    return c;
}

bool cte_init(Context *(*handler)(Event, Context *)) {
    static GateDesc32 idt[NR_IRQ];

    // initialize IDT
    for (unsigned int i = 0; i < NR_IRQ; i++) {
        idt[i] = GATE32(STS_TG, KSEL(SEG_KCODE), __am_vecnull, DPL_KERN);
    }

    // ----------------------- interrupts ----------------------------
    idt[32] = GATE32(STS_IG, KSEL(SEG_KCODE), __am_irq0, DPL_KERN);
    // ---------------------- system call ----------------------------
    idt[0x80] = GATE32(STS_TG, KSEL(SEG_KCODE), __am_vecsys, DPL_USER);
    idt[0x81] = GATE32(STS_TG, KSEL(SEG_KCODE), __am_vectrap, DPL_KERN);
    set_idt(idt, sizeof(idt));

    // register event handler
    user_handler = handler;

    return true;
}


Context *kcontext(Area kstack, void (*entry)(void *), void *arg) {
    Context save;
    save.eip = (uintptr_t) entry;
    save.cr3 = NULL;
    save.cs = 8;
//    IF = 2
    save.eflags = (1<<2);
    *(Context * )(kstack.end - sizeof(Context) - 2 * sizeof(uintptr_t)) = save;
    *(uintptr_t * )(kstack.end - sizeof(uintptr_t)) = (uintptr_t) arg;
    return kstack.end - sizeof(Context) - 2 * sizeof(uintptr_t);
}

void yield() {
    asm volatile("int $0x81");
}

bool ienabled() {
    return false;
}

void iset(bool enable) {
}
