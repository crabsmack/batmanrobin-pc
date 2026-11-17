/* funcs/Arc.h - prototypes of the Arc module: one line per function, its register
 * convention in the tag (dkit genesis recomp bridge). First written from the measured
 * contracts (dkit disasm regs); edited by the C's authors from then on. */
#pragma once
void Arc_PlanToArgs(uint32_t *d0, uint32_t *d1, uint32_t *d6, uint32_t *a4, uint32_t a5, uint32_t *a6, dk::Flags *ccr);	/* $009266 regs(&d0,&d1,&d6,&a4,a5,&a6,&ccr) ccout(NZVC) */
void Arc_PlanToMark(uint32_t *d0, uint32_t *d1, uint32_t *d6, uint32_t *a4, uint32_t a5, uint32_t *a6, dk::Flags *ccr);	/* $00926C regs(&d0,&d1,&d6,&a4,a5,&a6,&ccr) ccout(NZVC) */
void Arc_Step(uint32_t *d4, uint32_t *d5, uint32_t a6, dk::Flags *ccr);	/* $0092EA regs(&d4,&d5,a6,&ccr) ccout(NZVC) */
void Arc_HeadAtMark(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t a6, dk::Flags *ccr);	/* $00924A regs(&d0,&d1,&d2,a6,&ccr) ccout(NZVC) */
