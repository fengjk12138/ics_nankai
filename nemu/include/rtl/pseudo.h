#ifndef __RTL_PSEUDO_H__
#define __RTL_PSEUDO_H__

#ifndef __RTL_RTL_H__
#error "Should be only included by <rtl/rtl.h>"
#endif

/* RTL pseudo instructions */

static inline def_rtl(li, rtlreg_t* dest, const rtlreg_t imm) {
  rtl_addi(s, dest, rz, imm);
}

static inline def_rtl(mv, rtlreg_t* dest, const rtlreg_t *src1) {
  if (dest != src1) rtl_add(s, dest, src1, rz);
}

static inline def_rtl(not, rtlreg_t *dest, const rtlreg_t* src1) {
  // dest <- ~src1
//  TODO();
    *dest = ~(*src1);
}

static inline def_rtl(neg, rtlreg_t *dest, const rtlreg_t* src1) {
  // dest <- -src1
//  TODO();
    *dest = -(*src1);
}

static inline def_rtl(msb, rtlreg_t* dest, const rtlreg_t* src1, int width) {
// dest <- src1[width * 8 - 1]
//  TODO();
    *dest=((*src1)>>(8*width-1))&1;
}

static inline def_rtl(sext, rtlreg_t* dest, const rtlreg_t* src1, int width) {
  // dest <- signext(src1[(width * 8 - 1) .. 0])
//  TODO();
    rtlreg_t tmp1;
    rtl_msb(s, &tmp1, src1, width);
    if(width==1)
        *dest= (0xffffff00u * tmp1)  | (*src1);
    else if(width==2)
        *dest= (0xffff0000u * tmp1)  | (*src1);
    else *dest= *src1;
}

static inline def_rtl(zext, rtlreg_t* dest, const rtlreg_t* src1, int width) {
  // dest <- zeroext(src1[(width * 8 - 1) .. 0])
//  TODO();
    *dest = (0xffffffffu >> ((4 - width) * 8)) & (*src1);
}



#endif
