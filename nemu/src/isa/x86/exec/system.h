#include <monitor/difftest.h>

uint32_t pio_read_l(ioaddr_t);
uint32_t pio_read_w(ioaddr_t);
uint32_t pio_read_b(ioaddr_t);
void pio_write_l(ioaddr_t, uint32_t);
void pio_write_w(ioaddr_t, uint32_t);
void pio_write_b(ioaddr_t, uint32_t);

void raise_intr(DecodeExecState *, uint32_t, vaddr_t);

static inline def_EHelper(lidt) {
//  TODO();
        rtl_lm(s, &cpu.idtr, s->isa.mbase, s->isa.moff + 2, id_dest->width);
        print_asm_template1(lidt);
}

static inline def_EHelper(mov_r2cr) {
        TODO();
        print_asm("movl %%%s,%%cr%d", reg_name(id_src1->reg, 4), id_dest->reg);
}

static inline def_EHelper(mov_cr2r) {
        TODO();
        print_asm("movl %%cr%d,%%%s", id_src1->reg, reg_name(id_dest->reg, 4));

#ifndef __DIFF_REF_NEMU__
        difftest_skip_ref();
#endif
}

static inline def_EHelper(int) {
    raise_intr(s, *ddest, s->seq_pc);
    print_asm("int %s", id_dest->str);

#ifndef __DIFF_REF_NEMU__
    difftest_skip_dut(1, 2);
#endif
}

static inline def_EHelper(iret) {
//        TODO();
        rtl_pop(s, &s->jmp_pc);
        rtl_pop(s, &cpu.cs);
        rtl_pop(s, &cpu.eflags);
        s->is_jmp = 1;
        print_asm("iret");

#ifndef __DIFF_REF_NEMU__
        difftest_skip_ref();
#endif
}

static inline def_EHelper(in) {
//  TODO();

//    printf("%d---- \n",id_dest -> width);
        switch (id_dest -> width){
            case 1:
                *ddest = pio_read_b(*dsrc1);
            operand_write(s, id_dest, ddest);
            break;
            case 2:
                *ddest = pio_read_w(*dsrc1);
            operand_write(s, id_dest, ddest);
            break;
            case 4:
                *ddest = pio_read_l(*dsrc1);
            operand_write(s, id_dest, ddest);
            break;
        }
        print_asm_template2(in);
}

static inline def_EHelper(out) {
//  TODO();

        switch (id_dest -> width){

            case 1:
                pio_write_b(*ddest, *dsrc1);
            break;
            case 2:
                pio_write_w(*ddest, *dsrc1);
            break;
            case 4:
                pio_write_l(*ddest, *dsrc1);
            break;
        }
        print_asm_template2(out);
}
