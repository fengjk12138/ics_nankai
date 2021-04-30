#include "cc.h"

static inline def_EHelper(test) {
//  TODO();

    rtl_set_OF(s, rz);
    rtl_set_CF(s, rz);
    rtl_and(s, ddest, ddest, dsrc1);
    rtl_update_ZFSF(s, ddest, id_dest->width);
    print_asm_template2(test);
}

static inline def_EHelper(and) {
//  TODO();

    rtl_set_OF(s, rz);

    rtl_set_CF(s, rz);
    rtl_and(s, ddest, ddest, dsrc1);

    rtl_update_ZFSF(s, ddest, id_dest->width);

    operand_write(s, id_dest, ddest);
//    printf("%x--\n",cpu.esp);
    print_asm_template2(and);
}

static inline def_EHelper(xor) {
//  TODO();
    rtl_set_OF(s, rz);
    rtl_set_CF(s, rz);
    rtl_xor(s, ddest, ddest, dsrc1);
    rtl_update_ZFSF(s, ddest, id_dest->width);
    operand_write(s, id_dest, ddest);
    print_asm_template2(xor);
}

static inline def_EHelper(or) {
//  TODO();
    rtl_set_OF(s, rz);
    rtl_set_CF(s, rz);
    rtl_or(s, ddest, ddest, dsrc1);
    rtl_update_ZFSF(s, ddest, id_dest->width);
    operand_write(s, id_dest, ddest);
    print_asm_template2(or);
}

static inline def_EHelper(not) {
//  TODO();
    rtl_not(s, ddest, ddest);
    print_asm_template1(not);
}

static inline def_EHelper(sar) {
//  TODO();
    rtl_sar(s, ddest, ddest, dsrc1);
  // unnecessary to update CF and OF in NEMU
    rtl_update_ZFSF(s, ddest, id_dest->width);
    operand_write(s, id_dest, ddest);
    print_asm_template2(sar);
}

static inline def_EHelper(shl) {
//  TODO();
    rtl_shl(s, ddest, ddest, dsrc1);
    // unnecessary to update CF and OF in NEMU
    rtl_update_ZFSF(s, ddest, id_dest->width);
    operand_write(s, id_dest, ddest);
    print_asm_template2(shl);
}

static inline def_EHelper(shr) {
//  TODO();
    rtl_shr(s, ddest, ddest, dsrc1);
    // unnecessary to update CF and OF in NEMU
    rtl_update_ZFSF(s, ddest, id_dest->width);
    operand_write(s, id_dest, ddest);
    print_asm_template2(shr);
}


static inline def_EHelper(setcc) {
  uint32_t cc = s->opcode & 0xf;
  rtl_setcc(s, ddest, cc);
  operand_write(s, id_dest, ddest);

  print_asm("set%s %s", get_cc_name(cc), id_dest->str);
}

static inline def_EHelper(rol) {
//  TODO();
//        rtl_shr(s, ddest, ddest, dsrc1);
        // unnecessary to update CF and OF in NEMU
//        rtl_update_ZFSF(s, ddest, id_dest->width);
//        operand_write(s, id_dest, ddest);
        for(int i=0;i<*dsrc1;i++){
            rtl_msb(s, s0, ddest, id_dest->width);
            rtl_set_CF(s, s0);
            *ddest=((*ddest)<<1)|(*s0!=0);
        }
        operand_write(s, id_dest, ddest);
        print_asm_template2(rol);
}

static inline def_EHelper(ror) {
//  TODO();
//        rtl_shr(s, ddest, ddest, dsrc1);
        // unnecessary to update CF and OF in NEMU
//        rtl_update_ZFSF(s, ddest, id_dest->width);
//        operand_write(s, id_dest, ddest);
        for(int i=0;i<*dsrc1;i++){
            rtlreg_t tmp=*ddest & 1;
            rtl_set_CF(s, &tmp);
            *ddest=((*ddest)>>1)|((tmp)<<(id_dest->width*8));
        }
        operand_write(s, id_dest, ddest);
        print_asm_template2(ror);
}

static inline def_EHelper(shld) {
//  TODO();
        for(int i=0;i<*dsrc1;i++){
            rtl_msb(s, s0, ddest, id_dest->width);
            rtl_msb(s, s1, dsrc2, id_src2->width);
            rtl_set_CF(s, s0);
            *ddest=((*ddest)<<1)|(*s1!=0);
            *dsrc2=*dsrc2<<1;
        }
        rtl_update_ZFSF(s, ddest, id_dest->width);
        operand_write(s, id_dest, ddest);
        print_asm_template2(shld);
}

static inline def_EHelper(shrd) {
//  TODO();
        for(int i=0;i<*dsrc1;i++){
            rtlreg_t tmp=*ddest & 1;
            rtl_set_CF(s, &tmp);
            *ddest=((*ddest)>>1)|(((*dsrc2)&1)<<(id_dest->width*8));
            *dsrc2=(*dsrc2)>>1;
        }
        rtl_update_ZFSF(s, ddest, id_dest->width);
        operand_write(s, id_dest, ddest);
        print_asm_template2(shrd);
}
