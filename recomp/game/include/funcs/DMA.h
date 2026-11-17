/* funcs/DMA.h - prototypes of the DMA module: one line per function, its register
 * convention in the tag (dkit genesis recomp bridge). First written from the measured
 * contracts (dkit disasm regs); edited by the C's authors from then on. */
#pragma once
void DMA_ResetQueue(void);	/* $007836 regs() */
void DMA_FlushQueue(uint32_t ctrl);	/* $00784A regs(a0) clobbers(d0,a3) */
void DMA_Reserve(uint32_t *d0, uint16_t d1, uint32_t *a0);	/* $00799C regs(&d0,d1,&a0) tst(d0.l) ccout(NZ) */
void DMA_ReserveSlots(uint32_t *d0, uint32_t *a0);	/* $0079AA regs(&d0,&a0) tst(d0.l) ccout(NZVC) */
void DMA_FlushQueue_40(uint32_t ctrl, uint32_t *a3);	/* $00785A regs(a0,&a3) ccout(NZVC) */
void DMA_FlushQueue_38(uint32_t ctrl, uint32_t *a3);	/* $00786A regs(a0,&a3) ccout(NZVC) */
void DMA_FlushQueue_37(uint32_t ctrl, uint32_t *a3);	/* $007872 regs(a0,&a3) ccout(NZVC) */
void DMA_FlushQueue_32(uint32_t ctrl, uint32_t *a3);	/* $00789A regs(a0,&a3) ccout(NZVC) */
void DMA_FlushQueue_29(uint32_t ctrl, uint32_t *a3, dk::Flags *ccr);	/* $0078B2 regs(a0,&a3,&ccr) ccout(NZVC) */
void DMA_FlushQueue_16(uint32_t ctrl, uint32_t *a3, dk::Flags *ccr);	/* $00791A regs(a0,&a3,&ccr) ccout(NZVC) */
void DMA_FlushQueue_15(uint32_t ctrl, uint32_t *a3, dk::Flags *ccr);	/* $007922 regs(a0,&a3,&ccr) ccout(NZVC) */
void DMA_FlushQueue_14(uint32_t ctrl, uint32_t *a3, dk::Flags *ccr);	/* $00792A regs(a0,&a3,&ccr) ccout(NZVC) */
void DMA_FlushQueue_13(uint32_t ctrl, uint32_t *a3, dk::Flags *ccr);	/* $007932 regs(a0,&a3,&ccr) ccout(NZVC) */
void DMA_FlushQueue_12(uint32_t ctrl, uint32_t *a3, dk::Flags *ccr);	/* $00793A regs(a0,&a3,&ccr) ccout(NZVC) */
void DMA_FlushQueue_11(uint32_t ctrl, uint32_t *a3, dk::Flags *ccr);	/* $007942 regs(a0,&a3,&ccr) ccout(NZVC) */
void DMA_FlushQueue_10(uint32_t ctrl, uint32_t *a3, dk::Flags *ccr);	/* $00794A regs(a0,&a3,&ccr) ccout(NZVC) */
void DMA_FlushQueue_9(uint32_t ctrl, uint32_t *a3, dk::Flags *ccr);	/* $007952 regs(a0,&a3,&ccr) ccout(NZVC) */
void DMA_FlushQueue_8(uint32_t ctrl, uint32_t *a3, dk::Flags *ccr);	/* $00795A regs(a0,&a3,&ccr) ccout(NZVC) */
void DMA_FlushQueue_7(uint32_t ctrl, uint32_t *a3, dk::Flags *ccr);	/* $007962 regs(a0,&a3,&ccr) ccout(NZVC) */
void DMA_FlushQueue_6(uint32_t ctrl, uint32_t *a3, dk::Flags *ccr);	/* $00796A regs(a0,&a3,&ccr) ccout(NZVC) */
void DMA_FlushQueue_5(uint32_t ctrl, uint32_t *a3, dk::Flags *ccr);	/* $007972 regs(a0,&a3,&ccr) ccout(NZVC) */
void DMA_FlushQueue_4(uint32_t ctrl, uint32_t *a3, dk::Flags *ccr);	/* $00797A regs(a0,&a3,&ccr) ccout(NZVC) */
void DMA_FlushQueue_3(uint32_t ctrl, uint32_t *a3, dk::Flags *ccr);	/* $007982 regs(a0,&a3,&ccr) ccout(NZVC) */
void DMA_FlushQueue_2(uint32_t ctrl, uint32_t *a3, dk::Flags *ccr);	/* $00798A regs(a0,&a3,&ccr) ccout(NZVC) */
void DMA_FlushQueue_1(uint32_t ctrl, uint32_t *a3, dk::Flags *ccr);	/* $007992 regs(a0,&a3,&ccr) ccout(NZVC) */
