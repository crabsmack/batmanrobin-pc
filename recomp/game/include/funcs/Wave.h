/* funcs/Wave.h - prototypes of the Wave module: one line per function, its register
 * convention in the tag (dkit genesis recomp bridge). First written from the measured
 * contracts (dkit disasm regs); edited by the C's authors from then on. */
#pragma once
void Wave_Start(uint32_t *d0, uint16_t d4, uint16_t d5, uint32_t *a0, uint32_t *a1, uint32_t a5, uint32_t *a6);	/* $00932E regs(&d0,d4,d5,&a0,&a1,a5,&a6) tst(d0.l) ccout(NZVC) */
uint32_t Wave_EndAll(void);	/* $00930C regs()->d0 tst(d0.l) ccout(NVC) */
void Wave_StartScriptArg(uint32_t *d0, uint16_t d4, uint16_t d5, uint32_t a1, uint32_t *a2, uint32_t a3, uint32_t a4, uint32_t a5, uint32_t a6);	/* $00931A regs(&d0,d4,d5,a1,&a2,a3,a4,a5,a6) ccout(NVC) clobbers(a0) */
void Wave_StartFromEntry(uint16_t d4, uint16_t d5, uint32_t *a0, uint32_t *a1, uint32_t *a5, uint32_t *a6);	/* $00932C regs(d4,d5,&a0,&a1,&a5,&a6) ccout(Z) clobbers(d0) */
