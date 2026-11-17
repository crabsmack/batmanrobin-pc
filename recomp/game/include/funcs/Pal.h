/* funcs/Pal.h - prototypes of the Pal module: one line per function, its register
 * convention in the tag (dkit genesis recomp bridge). First written from the measured
 * contracts (dkit disasm regs); edited by the C's authors from then on. */
#pragma once
void Pal_FlashWhite(dk::Flags *ccr);	/* $009584 regs(&ccr) ccout(NVC) */
void Pal_VIntUpload(uint32_t a0, uint32_t a1, uint32_t a2);	/* $00A7F0 regs(a0,a1,a2) clobbers(d0,d1,d7,a3) */
void Pal_LoadLine0(uint32_t *src, uint32_t *a1, dk::Flags *ccr);	/* $00ABCA regs(&a0,&a1,&ccr) ccout(NZVC) */
void Pal_LoadLine1(uint32_t *src, uint32_t *a1, dk::Flags *ccr);	/* $00ABE6 regs(&a0,&a1,&ccr) ccout(NZVC) */
void Pal_LoadLine2(uint32_t *src, uint32_t *a1, dk::Flags *ccr);	/* $00AC02 regs(&a0,&a1,&ccr) ccout(NZVC) */
void Pal_LoadLine3(uint32_t *src, uint32_t *a1, dk::Flags *ccr);	/* $00AC1E regs(&a0,&a1,&ccr) ccout(NZVC) */
void Pal_CopyBright(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d6, uint16_t d7, uint32_t *a0, uint32_t *a1, uint32_t *a2, uint32_t *a3, uint32_t *a4);	/* $00AD4A regs(&d0,&d1,&d2,&d6,d7,&a0,&a1,&a2,&a3,&a4) clobbers(d7) */
