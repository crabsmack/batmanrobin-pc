/* funcs/SegaLogo.h - prototypes of the SegaLogo module: one line per function, its register
 * convention in the tag (dkit genesis recomp bridge). First written from the measured
 * contracts (dkit disasm regs); edited by the C's authors from then on. */
#pragma once
void SegaLogo_Run(uint32_t *d1, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6, uint32_t *a1, uint32_t *a2, uint32_t *a3, uint32_t *a4, uint32_t *a5, uint32_t *a6);	/* $049DA6 regs(&d1,&d3,&d4,&d5,&d6,&a1,&a2,&a3,&a4,&a5,&a6) clobbers(d0,d2,d7,a0) */
void SegaLogo_SpawnCycler(uint32_t *d0, uint16_t *d1, uint16_t d4, uint16_t d5, uint32_t *d7, uint32_t *a2, uint32_t *a3, uint32_t *a4, uint32_t a6);	/* $049DC2 regs(&d0,&d1,d4,d5,&d7,&a2,&a3,&a4,a6) clobbers(a0,a5,a6) */
void SegaLogo_RotatePalette(uint16_t *d0, uint32_t *d7);	/* $049DEC regs(&d0,&d7) clobbers(a0) */
