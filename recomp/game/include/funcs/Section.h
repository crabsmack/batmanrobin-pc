/* funcs/Section.h - prototypes of the Section module: one line per function, its register
 * convention in the tag (dkit genesis recomp bridge). First written from the measured
 * contracts (dkit disasm regs); edited by the C's authors from then on. */
#pragma once
void Section_RunNext(uint32_t *d1, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6, const u32 *order, uint32_t *a1, uint32_t *a2, uint32_t *a3, uint32_t *a4, uint32_t *a5, uint32_t *a6, dk::Flags *ccr);	/* $009FBC regs(&d1,&d3,&d4,&d5,&d6,a0,&a1,&a2,&a3,&a4,&a5,&a6,&ccr) ccout(Z) clobbers(d0,d2,d7,a0) */
void Section_SetIdInline(const u16 **script);	/* $00772E regs(&a6) */
void Section_StartExit(uint32_t *a0, dk::Flags *ccr);	/* $010528 regs(&a0,&ccr) ccout(NZVC) */
void Section_StartExitN(uint32_t *a0, const u16 **script, dk::Flags *ccr);	/* $010530 regs(&a0,&a2,&ccr) ccout(NVC) */
