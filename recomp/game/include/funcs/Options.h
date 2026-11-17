/* funcs/Options.h - prototypes of the Options module: one line per function, its register
 * convention in the tag (dkit genesis recomp bridge). First written from the measured
 * contracts (dkit disasm regs); edited by the C's authors from then on. */
#pragma once
void Options_Init(uint32_t *d0, uint16_t *d6, uint32_t *d7, uint32_t *a0, uint32_t *a1, uint32_t *a2, uint32_t *a3, uint32_t *a4, uint32_t *a5, uint32_t *a6);	/* $0499E8 regs(&d0,&d6,&d7,&a0,&a1,&a2,&a3,&a4,&a5,&a6) ccout(NZVC) */
void Options_Step(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6, uint32_t *d7, uint32_t *a1, uint32_t *a2, uint32_t *a3, uint32_t *a4, uint32_t *a5, uint32_t *a6);	/* $0499FC regs(&d0,&d1,&d2,&d3,&d4,&d5,&d6,&d7,&a1,&a2,&a3,&a4,&a5,&a6) clobbers(a0) */
