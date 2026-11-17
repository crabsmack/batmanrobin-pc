/* funcs/Tint.h - prototypes of the Tint module: one line per function, its register
 * convention in the tag (dkit genesis recomp bridge). First written from the measured
 * contracts (dkit disasm regs); edited by the C's authors from then on. */
#pragma once
void Tint_FlashC1(dk::Flags *ccr);	/* $00AC66 regs(&ccr) ccout(NVC) */
void Tint_StepLine3(uint8_t *level, uint32_t *step, dk::Flags *ccr);	/* $00AD14 regs(&d0,&d1,&ccr) ccout(NZVC) */
void Tint_Step(uint8_t *d0, uint32_t *d1);	/* $00AD26 regs(&d0,&d1) */
void Tint_FlashC3(void);	/* $00AC7E regs() */
void Tint_FlashE2(dk::Flags *ccr);	/* $00AC96 regs(&ccr) ccout(NVC) */
void Tint_FlashA2(dk::Flags *ccr);	/* $00ACAE regs(&ccr) ccout(NVC) */
void Tint_FlashE6(dk::Flags *ccr);	/* $00ACC6 regs(&ccr) ccout(NVC) */
uint32_t Tint_StepLine0(void);	/* $00ACDE regs()->d1 clobbers(d0) */
void Tint_StepLine1(uint8_t *level, uint32_t *step, dk::Flags *ccr);	/* $00ACF0 regs(&d0,&d1,&ccr) ccout(NVC) */
