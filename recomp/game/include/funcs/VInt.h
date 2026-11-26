/* funcs/VInt.h - prototypes of the VInt module: one line per function, its register
 * convention in the tag (dkit genesis recomp bridge). First written from the measured
 * contracts (dkit disasm regs); edited by the C's authors from then on. */
#pragma once
void VInt_Handler(uint32_t *d0, uint32_t d1, uint32_t d2, uint32_t d3, uint32_t d4, uint32_t d5, uint32_t d6, uint32_t d7, uint32_t a0, uint32_t a1, uint32_t a2, uint32_t a3, uint32_t *a4, uint32_t a5, uint32_t a6);	/* $007736 regs(&d0,d1,d2,d3,d4,d5,d6,d7,a0,a1,a2,a3,&a4,a5,a6) ccout(NZVC) vector */
void VInt_UploadScroll(uint32_t a0, uint32_t a1, uint32_t a2);	/* $00AE74 regs(a0,a1,a2) clobbers(d0,d1,a4) */
void VInt_ScrollModeTbl(uint32_t a0);	/* $00AE7C regs(a0) */
void VInt_ScrollModeTbl28(uint32_t a0, uint32_t a2);	/* $00AEA4 regs(a0,a2) clobbers(d0,d1) */
void VInt_BusyScroll(uint32_t a0);	/* $00AEBC regs(a0) clobbers(d0) */
void VInt_BusyScrollTbl(uint32_t a0);	/* $00AEC4 regs(a0) */
void VInt_BusyScrollTbl8(uint32_t a0);	/* $00AECC regs(a0) */
void VInt_BusyScrollTbl28(uint32_t a0);	/* $00AEEC regs(a0) */
void VInt_BusyScrollTbl34(uint32_t a0);	/* $00AEF8 regs(a0) */
void VInt_ScrollDisplayOn(uint32_t a0);	/* $00AF04 regs(a0) */
void VInt_ScrollLinesARaster(uint32_t a0, uint32_t a2);	/* $00AF84 regs(a0,a2) clobbers(d0,d1) */
void VInt_BusyRearmRaster(uint32_t a0);	/* $00AFDA regs(a0) */
void VInt_ScrollCustom(void);	/* $00AF0A regs() clobbers(a4) */
void VInt_ScrollPlanes(uint32_t a0, uint32_t a1);	/* $00AF10 regs(a0,a1) */
void VInt_ScrollLines(uint32_t a0, uint32_t a1, uint32_t a2);	/* $00AF2E regs(a0,a1,a2) */
void VInt_ScrollLinePairs(uint32_t a0, uint32_t a1, uint32_t a2);	/* $00AF42 regs(a0,a1,a2) */
void VInt_ScrollLinesColumns(uint32_t a0, uint32_t a2);	/* $00AF8C regs(a0,a2) */
void VInt_ScrollLinesA(uint32_t a0, uint32_t a2);	/* $00AF9A regs(a0,a2) */
void VInt_ScrollLinesBRaster(uint32_t a0, uint32_t a2);	/* $00AFA4 regs(a0,a2) clobbers(d0,d1) */
void VInt_ScrollLinesRaster(uint32_t a0, uint32_t a1, uint32_t a2);	/* $00AFAC regs(a0,a1,a2) clobbers(d0,d1) */
void VInt_ScrollPlanesRaster(uint32_t a0, uint32_t a1);	/* $00AFBE regs(a0,a1) clobbers(d0,d1) */
/* the jump-table slots entered only by the V-int's computed jmp (no static caller, so no measured
   contract): each the convention of the routine its bra.w goes to */
void VInt_ScrollMode04(void);	/* $00AE80 regs() clobbers(a4) */
void VInt_ScrollMode08(uint32_t a0, uint32_t a1);	/* $00AE84 regs(a0,a1) */
void VInt_ScrollMode0C(uint32_t a0, uint32_t a1, uint32_t a2);	/* $00AE88 regs(a0,a1,a2) */
void VInt_ScrollMode10(uint32_t a0, uint32_t a1, uint32_t a2);	/* $00AE8C regs(a0,a1,a2) */
void VInt_ScrollMode2C(uint32_t a0, uint32_t a2);	/* $00AEA8 regs(a0,a2) */
void VInt_ScrollMode30(uint32_t a0, uint32_t a2);	/* $00AEAC regs(a0,a2) */
void VInt_ScrollMode34(uint32_t a0, uint32_t a2);	/* $00AEB0 regs(a0,a2) clobbers(d0,d1) */
void VInt_ScrollMode38(uint32_t a0, uint32_t a1, uint32_t a2);	/* $00AEB4 regs(a0,a1,a2) clobbers(d0,d1) */
void VInt_ScrollMode3C(uint32_t a0, uint32_t a1);	/* $00AEB8 regs(a0,a1) clobbers(d0,d1) */
void VInt_BusyMode0C(uint32_t a0);	/* $00AED0 regs(a0) */
void VInt_BusyMode10(uint32_t a0);	/* $00AED4 regs(a0) */
void VInt_BusyMode2C(uint32_t a0);	/* $00AEF0 regs(a0) ccout(NZVC) */
void VInt_BusyMode38(uint32_t a0);	/* $00AEFC regs(a0) ccout(NZVC) */
void VInt_BusyMode04(uint32_t a0);	/* $00AEC8 regs(a0) ccout(NZVC) */
void VInt_BusyMode14(uint32_t a0);	/* $00AED8 regs(a0) ccout(NZVC) */
void VInt_BusyMode18(uint32_t a0);	/* $00AEDC regs(a0) ccout(NZVC) */
void VInt_BusyMode1C(uint32_t a0);	/* $00AEE0 regs(a0) ccout(NZVC) */
void VInt_BusyMode24(uint32_t a0);	/* $00AEE8 regs(a0) ccout(NZVC) */
void VInt_BusyMode30(uint32_t a0);	/* $00AEF4 regs(a0) ccout(NZVC) */
void VInt_BusyMode3C(uint32_t a0);	/* $00AF00 regs(a0) ccout(NZVC) */
