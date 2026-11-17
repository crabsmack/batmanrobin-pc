/* funcs/Spr.h - prototypes of the Spr module: one line per function, its register
 * convention in the tag (dkit genesis recomp bridge). First written from the measured
 * contracts (dkit disasm regs); edited by the C's authors from then on. */
#pragma once
void Spr_QueueSATDMA(uint32_t *d1, uint32_t *a3);	/* $0079C0 regs(&d1,&a3) clobbers(d0,a0) */
void Spr_ResetList(uint32_t *a0, uint32_t *a1);	/* $007A08 regs(&a0,&a1) */
void Spr_AddSorted(uint16_t *d0, uint32_t *d1, uint32_t *d2, uint16_t d4, uint16_t d5, uint16_t d6, uint16_t d7, uint32_t a0, uint32_t a1, uint32_t a2, dk::Flags *ccr);	/* $007A4A regs(&d0,&d1,&d2,d4,d5,d6,d7,a0,a1,a2,&ccr) ccout(NZVC) */
void Spr_AddBack(uint16_t *d0, uint32_t *d1, uint16_t d4, uint16_t d5, uint16_t d6, uint32_t a0, uint32_t a1, dk::Flags *ccr);	/* $007B1C regs(&d0,&d1,d4,d5,d6,a0,a1,&ccr) ccout(NZVC) */
void Spr_AddFront(uint32_t *d0, uint16_t *d1, uint16_t d4, uint16_t d5, uint16_t d6, uint32_t *a0, dk::Flags *ccr);	/* $007B50 regs(&d0,&d1,d4,d5,d6,&a0,&ccr) ccout(NZVC) */
void Spr_BuildSAT(uint32_t *d1, uint16_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6, uint16_t *d7, uint32_t *a1, uint32_t *a2, uint32_t *a4, uint32_t *a5, uint32_t *a6);	/* $007B72 regs(&d1,&d2,&d3,&d4,&d5,&d6,&d7,&a1,&a2,&a4,&a5,&a6) clobbers(d0,a0) */
void Spr_DrawDirect(uint32_t *d0, uint32_t *d1, uint16_t *d2, uint32_t *d3, uint16_t x, uint16_t y, uint16_t *attr, uint16_t *d7, uint32_t *entry, uint32_t *sat, uint32_t *a2);	/* $007D5E regs(&d0,&d1,&d2,&d3,d4,d5,&d6,&d7,&a0,&a1,&a2) */
void Spr_EmitClipped(uint32_t *d0, uint32_t *d1, uint16_t *count, uint16_t *slot, uint16_t x, uint16_t y, uint16_t *attr, uint16_t *d7, uint32_t *entry, uint32_t *sat, uint32_t *art);	/* $007D76 regs(&d0,&d1,&d2,&d3,d4,d5,&d6,&d7,&a0,&a1,&a2) */
void Spr_InitBankFrames(uint32_t *d0, uint16_t handle_base, uint16_t *count, uint32_t bank, uint32_t *a1, uint32_t *a2);	/* $007EB8 regs(&d0,d1,&d7,a0,&a1,&a2) clobbers(a0) */
void Spr_ChangeFrame(uint32_t *d0, uint32_t *frame, uint32_t *old, uint32_t *d3, uint32_t *entry, uint32_t *a1);	/* $007EF6 regs(&d0,&d1,&d2,&d3,&a0,&a1) */
void Spr_SwapFrameArt(uint32_t *d0, uint32_t *frame, uint32_t *old, uint32_t *d3, uint32_t *entry, uint32_t *a1);	/* $007F5E regs(&d0,&d1,&d2,&d3,&a0,&a1) */
void Spr_ReleaseFrame(uint16_t *d0, uint16_t *frame, uint32_t *a0);	/* $008000 regs(&d0,&d2,&a0) */
void Spr_QueueArtDMA(uint16_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d7, uint32_t *a1, uint32_t *blocks, uint32_t *art);	/* $008042 regs(&d1,&d2,&d3,&d7,&a1,&a3,&a4) clobbers(d0,d4,d5,a0,a2) */
void Spr_MaskLeftColumn(uint32_t *d0, uint32_t *d1, uint32_t *d7, uint32_t a0);	/* $00F808 regs(&d0,&d1,&d7,a0) clobbers(a0) */
