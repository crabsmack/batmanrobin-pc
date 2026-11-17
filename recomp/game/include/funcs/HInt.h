/* funcs/HInt.h - prototypes of the HInt module: one line per function, its register
 * convention in the tag (dkit genesis recomp bridge). First written from the measured
 * contracts (dkit disasm regs); edited by the C's authors from then on. */
#pragma once
void HInt_InstallVScrollB(uint32_t *end, uint32_t *src, uint32_t *dst);	/* $00B1A4 regs(&d7,&a0,&a1) */
void HInt_InstallVScrollA(uint32_t *end, uint32_t *src, uint32_t *dst);	/* $00B1AE regs(&d7,&a0,&a1) */
void HInt_Install(uint32_t *end, uint32_t *src, uint32_t *dst);	/* $00B1B6 regs(&d7,&a0,&a1) */
void HInt_Rearm(uint32_t ctrl);	/* $00B1D8 regs(a0) */
void HInt_SwapRearm(uint32_t ctrl);	/* $00B1E0 regs(a0) clobbers(d0,d1) */
void HInt_Arm(uint32_t ctrl);	/* $00B1F4 regs(a0) */
