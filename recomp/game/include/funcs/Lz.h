/* funcs/Lz.h - prototypes of the Lz module: one line per function, its register
 * convention in the tag (dkit genesis recomp bridge). First written from the measured
 * contracts (dkit disasm regs); edited by the C's authors from then on. */
#pragma once
void Lz_Decompress(uint32_t *d1, uint32_t *d2, uint32_t *d6, uint32_t *d7, uint32_t *a0, uint32_t *a1, uint32_t *a2, uint32_t *a3, dk::Flags *ccr);	/* $00A23E regs(&d1,&d2,&d6,&d7,&a0,&a1,&a2,&a3,&ccr) ccout(NZVC) */
