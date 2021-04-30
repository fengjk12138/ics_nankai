static inline def_EHelper(mov) {
        operand_write(s, id_dest, dsrc1);
        print_asm_template2(mov);
}

static inline def_EHelper(push) {
//  TODO();
        rtl_push(s, ddest);
        print_asm_template1(push);
}

static inline def_EHelper(pop) {
//        TODO();
        rtl_pop(s, ddest);
        print_asm_template1(pop);
}

static inline def_EHelper(pusha) {
        TODO();
        print_asm("pusha");
}

static inline def_EHelper(popa) {
        TODO();
        print_asm("popa");
}

static inline def_EHelper(leave) {
//        TODO();
        cpu.esp = cpu.ebp;
        rtl_pop(s, &cpu.ebp);
        print_asm("leave");
}

static inline def_EHelper(cltd) {
        if (s->isa.is_operand_size_16) {
            rtl_msb(s, s0, &cpu.eax, 2);
            if(*s0)
                rtl_ori(s, &cpu.edx, &cpu.edx, 0x0000ffffu);
            else
                rtl_andi(s, &cpu.edx, &cpu.edx, 0xffff0000u);
        }
        else {
//            TODO();
            rtl_msb(s, s0, &cpu.eax, 4);
            if(*s0)
                rtl_li(s, &cpu.edx, 0xffffffffu);
            else
                rtl_li(s, &cpu.edx, 0);
        }
        print_asm(s->isa.is_operand_size_16 ? "cwtl" : "cltd");
}

static inline def_EHelper(cwtl) {
        if (s->isa.is_operand_size_16) {
            rtl_msb(s, s0, &cpu.eax, 1);
            if(*s0)
                rtl_ori(s, &cpu.eax, &cpu.eax, 0x0000ff00u);
            else
                rtl_andi(s, &cpu.eax, &cpu.eax, 0xffff00ffu);
        }
        else {
            rtl_msb(s, s0, &cpu.eax, 2);
            if(*s0)
                rtl_ori(s, &cpu.eax, &cpu.eax, 0xffff0000u);
            else
                rtl_andi(s, &cpu.eax, &cpu.eax, 0x0000ffffu);
        }
        print_asm(s->isa.is_operand_size_16 ? "cbtw" : "cwtl");
}

static inline def_EHelper(movsx) {
        id_dest->width = s->isa.is_operand_size_16 ? 2 : 4;
        rtl_sext(s, ddest, dsrc1, id_src1->width);
        operand_write(s, id_dest, ddest);
        print_asm_template2(movsx);
}

static inline def_EHelper(movzx) {
        id_dest->width = s->isa.is_operand_size_16 ? 2 : 4;
        operand_write(s, id_dest, dsrc1);
        print_asm_template2(movzx);
}

static inline def_EHelper(lea) {
        rtl_addi(s, ddest, s->isa.mbase, s->isa.moff);
        operand_write(s, id_dest, ddest);
        print_asm_template2(lea);
}
