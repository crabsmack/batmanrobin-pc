/* funcs/Op6.h - prototypes of the Op6 module: one line per function, its register
 * convention in the tag (dkit genesis recomp bridge). First written from the measured
 * contracts (dkit disasm regs); edited by the C's authors from then on. */
#pragma once
void Op6_CallNative(uint32_t *d0, uint32_t *d1, uint32_t *d2, const u16 **script);	/* $0070E0 regs(&d0,&d1,&d2,&a6) clobbers(a0) */
void Op6_ClearPlane(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *a0, uint32_t *a1, uint32_t *a6);	/* $00723A regs(&d0,&d1,&d2,&d3,&a0,&a1,&a6) ccout(NZVC) */
void Op6_MoveLayerAToCamera(uint16_t *d0, uint16_t *d1, uint16_t *d2, uint32_t *a6);	/* $00737C regs(&d0,&d1,&d2,&a6) clobbers(a0) */
uint32_t Op6_RedrawLayerA(uint32_t a6);	/* $0073EE regs(a6)->a0 ccout(NZVC) */
void Op6_End(void);	/* $0070B8 regs() */
void Op6_CallNativeRetry(uint32_t *d0, uint32_t *d1, uint32_t *d4, uint32_t *d5, uint32_t *d6, uint32_t *d7, uint32_t *a0, uint32_t *a1, uint32_t *a2, uint32_t *a3, uint32_t *a4, uint32_t *a5, uint32_t *a6, dk::Flags *ccr);	/* $0070BE regs(&d0,&d1,&d4,&d5,&d6,&d7,&a0,&a1,&a2,&a3,&a4,&a5,&a6,&ccr) ccout(NZVC) */
void Op6_InitBankFrames(uint32_t *d0, uint16_t *d1, uint16_t *d7, uint32_t *a1, uint32_t *a2, const u16 **script);	/* $0070E6 regs(&d0,&d1,&d7,&a1,&a2,&a6) clobbers(a0) */
void Op6_Decompress(uint32_t *d1, uint32_t *d2, uint32_t *d6, uint32_t *d7, uint32_t *a1, uint32_t *a2, uint32_t *a3, const u16 **script);	/* $007108 regs(&d1,&d2,&d6,&d7,&a1,&a2,&a3,&a6) clobbers(a0) */
void Op6_DecompressBg(uint32_t *d0, uint32_t d1, uint32_t *a0, uint32_t *a1, uint32_t a3, uint32_t *a4, const u16 **script);	/* $007120 regs(&d0,d1,&a0,&a1,a3,&a4,&a6) ccout(NZVC) */
void Op6_StoreLzEnd(const u16 **script);	/* $007148 regs(&a6) clobbers(a0) */
void Op6_SetBufPtrToEntry(uint32_t *a1, const u16 **script);	/* $007158 regs(&a1,&a6) clobbers(a0) */
void Op6_LoadPalettesYield(uint8_t *d0, uint32_t *d7, uint32_t *a0, uint32_t *a1, const u16 **script, dk::Flags *ccr);	/* $007170 regs(&d0,&d7,&a0,&a1,&a6,&ccr) ccout(NZVC) */
void Op6_LoadPalettes(uint8_t *d0, uint32_t *d7, uint32_t *a0, uint32_t *a1, uint32_t *a6);	/* $007182 regs(&d0,&d7,&a0,&a1,&a6) ccout(NZVC) */
void Op6_InitLayerAOnPlaneA(uint16_t *d0, uint16_t *d1, uint32_t *d2, uint16_t *d6, uint32_t *a1, uint32_t *a2, uint32_t *a3, uint32_t *a4, uint32_t *a6);	/* $0071CC regs(&d0,&d1,&d2,&d6,&a1,&a2,&a3,&a4,&a6) clobbers(a0) */
void Op6_InitLayerAOnPlaneB(uint16_t *d0, uint16_t *d1, uint32_t *d2, uint16_t *d6, uint32_t *a1, uint32_t *a2, uint32_t *a3, uint32_t *a4, uint32_t *a6);	/* $0071DA regs(&d0,&d1,&d2,&d6,&a1,&a2,&a3,&a4,&a6) clobbers(a0) */
void Op6_ClearPlaneRect(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *a0, uint32_t *a1, uint32_t *a6);	/* $00724A regs(&d0,&d1,&d2,&d3,&a0,&a1,&a6) ccout(NZVC) */
void Op6_DrawMap(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6, uint16_t *d7, uint32_t *a1, uint32_t *a3, uint32_t *a6);	/* $007468 regs(&d0,&d1,&d2,&d3,&d4,&d5,&d6,&d7,&a1,&a3,&a6) clobbers(a0) */
void Op6_DrawMapRect(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6, uint16_t *d7, uint32_t *a1, uint32_t *a3, uint32_t *a6);	/* $00747C regs(&d0,&d1,&d2,&d3,&d4,&d5,&d6,&d7,&a1,&a3,&a6) clobbers(a0) */
void Op6_DrawMapVInt(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6, uint16_t *d7, uint32_t *a0, uint32_t *a1, uint32_t *a6);	/* $007488 regs(&d0,&d1,&d2,&d3,&d4,&d5,&d6,&d7,&a0,&a1,&a6) ccout(NZVC) */
void Op6_DrawMapRectVInt(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6, uint16_t *d7, uint32_t *a0, uint32_t *a1, uint32_t *a6);	/* $00749C regs(&d0,&d1,&d2,&d3,&d4,&d5,&d6,&d7,&a0,&a1,&a6) ccout(NZVC) */
void Op6_DMAEntry(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint16_t *d3, uint16_t *d7, uint32_t *a6);	/* $007604 regs(&d0,&d1,&d2,&d3,&d7,&a6) clobbers(a0) */
void Op6_QueueDMAEntry(uint16_t *d0, uint32_t *a0, uint32_t *a6, dk::Flags *ccr);	/* $007682 regs(&d0,&a0,&a6,&ccr) ccout(NZVC) */
/* a native routine an a6 script calls (`jsr (a0)` in Op6_CallNative): a6 at its inline arguments
 * (ArtCache_AddBlocksInline $8580, Section_SetIdInline $772E), past them on the return. */
void Op6NativeFn(uint32_t code, uint32_t *d0, uint32_t *d1, uint32_t *d2, const u16 **script);	/* type regs(&d0,&d1,&d2,&a6) */
/* the routine Op6_CallNativeRetry calls (`jsr (a0)`, a0 = the routine): its Z flag set = done, clear =
 * call again next frame (the stage 2 section natives $33356, $3337C). */
void Op6RetryNativeFn(uint32_t code, uint32_t *d0, uint32_t *d1, uint32_t *d4, uint32_t *d5, uint32_t *d6, uint32_t *d7, uint32_t *a0, uint32_t *a1, uint32_t *a2, uint32_t *a3, uint32_t *a4, uint32_t *a5, uint32_t *a6, dk::Flags *ccr);	/* type regs(&d0,&d1,&d4,&d5,&d6,&d7,&a0,&a1,&a2,&a3,&a4,&a5,&a6,&ccr) */
