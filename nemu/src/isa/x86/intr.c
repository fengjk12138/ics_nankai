#include <cpu/exec.h>
#include "local-include/rtl.h"

void raise_intr(DecodeExecState *s, uint32_t NO, vaddr_t ret_addr) {
    /* TODO: Trigger an interrupt/exception with ``NO''.
     * That is, use ``NO'' to index the IDT.
     */
    word_t offset2 = vaddr_read(cpu.idtr + NO * 8, 4);
    word_t offset1 = vaddr_read(cpu.idtr + NO * 8 + 4, 4);
//    printf("%x--%x--\n",cpu.idtr,NO);
    word_t addr = (offset1 & (0xffff0000)) | (offset2 & 0xffff);
    rtl_push(s, &cpu.eflags);
    rtl_set_IF(s, rz);
    rtl_push(s, &cpu.cs);
    rtl_push(s, &ret_addr);
    s->is_jmp = 1;
    s->jmp_pc=addr;
//    TODO();
}
#define IRQ_TIMER 32          // for x86
void query_intr(DecodeExecState *s) {
//    TODO();
    if ((cpu.eflags&(1<<IF)) && cpu.INTR) {
        cpu.INTR = false;
        raise_intr(s, IRQ_TIMER, cpu.pc);
        update_pc(s);
    }
}
