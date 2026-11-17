/* funcs/BgTask.h - prototypes of the BgTask module: one line per function, its register
 * convention in the tag (dkit genesis recomp bridge). First written from the measured
 * contracts (dkit disasm regs); edited by the C's authors from then on. */
#pragma once
void BgTask_Start(uint32_t *d0, uint32_t d1, uint32_t a0, uint32_t a1, uint32_t a3, uint32_t *a4, dk::Flags *ccr);	/* $00A1F0 regs(&d0,d1,a0,a1,a3,&a4,&ccr) ccout(NZVC) */
void BgTask_Resume(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d6, uint32_t *d7, uint32_t *a0, uint32_t *a1, uint32_t *a2, uint32_t *a3, uint32_t *a4, dk::Flags *flags);	/* $00A20C regs(&d0,&d1,&d2,&d6,&d7,&a0,&a1,&a2,&a3,&a4,&ccr) ccout(NZVC) */
