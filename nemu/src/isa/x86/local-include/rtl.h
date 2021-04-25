#ifndef __X86_RTL_H__
#define __X86_RTL_H__

#include <rtl/rtl.h>
#include "reg.h"

/* RTL pseudo instructions */

static inline def_rtl(lr, rtlreg_t* dest, int r, int width) {
  switch (width) {
    case 4: rtl_mv(s, dest, &reg_l(r)); return;
    case 1: rtl_host_lm(s, dest, &reg_b(r), 1); return;
    case 2: rtl_host_lm(s, dest, &reg_w(r), 2); return;
    default: assert(0);
  }
}

static inline def_rtl(sr, int r, const rtlreg_t* src1, int width) {
  switch (width) {
    case 4: rtl_mv(s, &reg_l(r), src1); return;
    case 1: rtl_host_sm(s, &reg_b(r), src1, 1); return;
    case 2: rtl_host_sm(s, &reg_w(r), src1, 2); return;
    default: assert(0);
  }
}

static inline def_rtl(push, const rtlreg_t* src1) {
  // esp <- esp - 4
  // M[esp] <- src1
//  TODO();
    cpu.esp -= 4;
    rtl_sm(s, &cpu.esp, 0, src1, 4);
}

static inline def_rtl(pop, rtlreg_t* dest) {
  // dest <- M[esp]
  // esp <- esp + 4
//  TODO();
    rtl_lm(s, dest, &cpu.esp, 0, 4);
    cpu.esp += 4;
}

static inline def_rtl(is_sub_overflow, rtlreg_t* dest,
    const rtlreg_t* res, const rtlreg_t* src1, const rtlreg_t* src2, int width) {
  // dest <- is_overflow(src1 - src2)
    rtlreg_t tmp1, tmp2, tmp3;
    rtl_msb(s, &tmp1, src1, width);
    rtl_msb(s, &tmp2, src2, width);
    rtl_msb(s, &tmp3, res, width);
    *dest=0;
    if(tmp1>0 && tmp2==0 && tmp3==0) *dest=1;
    else
    if(tmp1==0 && tmp2>0 && tmp3>0) *dest=1;
}

static inline def_rtl(is_sub_carry, rtlreg_t* dest,
    const rtlreg_t* src1, const rtlreg_t* src2) {
  // dest <- is_carry(src1 - src2)
//  TODO();
    *dest=*src1<*src2;
}

static inline def_rtl(is_add_overflow, rtlreg_t* dest,
    const rtlreg_t* res, const rtlreg_t* src1, const rtlreg_t* src2, int width) {
  // dest <- is_overflow(src1 + src2)
//  TODO();
    rtlreg_t tmp1, tmp2, tmp3;
    rtl_msb(s, &tmp1, src1, width);
    rtl_msb(s, &tmp2, src2, width);
    rtl_msb(s, &tmp3, res, width);
    *dest=0;
    if(tmp1>0 && tmp2>0 && tmp3==0) *dest=1;
    else
    if(tmp1==0 && tmp2==0 && tmp3>0) *dest=1;
}

static inline def_rtl(is_add_carry, rtlreg_t* dest,
    const rtlreg_t* res, const rtlreg_t* src1) {
  // dest <- is_carry(src1 + src2)
    *dest=*res<*src1;
}

#define def_rtl_setget_eflags(f) \
  static inline def_rtl(concat(set_, f), const rtlreg_t* src) { \
    if(*src) cpu.eflags|=(1 << f);\
    else cpu.eflags&=~(1 << f);\
  } \
  static inline def_rtl(concat(get_, f), rtlreg_t* dest) { \
    *dest = (cpu.eflags >> f) & 1;\
  }

def_rtl_setget_eflags(CF)
def_rtl_setget_eflags(OF)
def_rtl_setget_eflags(ZF)
def_rtl_setget_eflags(SF)

static inline def_rtl(update_ZF, const rtlreg_t* result, int width) {
  // eflags.ZF <- is_zero(result[width * 8 - 1 .. 0])
//  TODO();
    rtlreg_t tmp=((0xffffffffu >> ((4-width)*8)) & (*result)) == 0;
    rtl_set_ZF(s, &tmp);
}

static inline def_rtl(update_SF, const rtlreg_t* result, int width) {
  // eflags.SF <- is_sign(result[width * 8 - 1 .. 0])
//    TODO();
    rtlreg_t tmp1;
    rtl_msb(s, &tmp1, result, width);
    rtl_set_SF(s, &tmp1);
}

static inline def_rtl(update_ZFSF, const rtlreg_t* result, int width) {
  rtl_update_ZF(s, result, width);
  rtl_update_SF(s, result, width);
}

#endif
