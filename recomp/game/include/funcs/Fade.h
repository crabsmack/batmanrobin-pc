/* funcs/Fade.h - prototypes of the Fade module: one line per function, its register
 * convention in the tag (dkit genesis recomp bridge). First written from the measured
 * contracts (dkit disasm regs); edited by the C's authors from then on. */
#pragma once
void Fade_Step(void);	/* $00A43C regs() clobbers(d0,d1,d2,d3,d4,d5,d6,d7,a0,a1,a2,a3,a4,a5,a6) */
void Fade_StartIn(uint8_t *level);	/* $00A9F6 regs(&d0) */
void Fade_ToNormal(dk::Flags *ccr);	/* $00AA1A regs(&ccr) ccout(NVC) */
void Fade_StartOutBlack(dk::Flags *ccr);	/* $00AA2A regs(&ccr) ccout(NVC) */
void Fade_StartOutWhite(void);	/* $00AA4C regs() */
uint32_t Fade_FlickerTarget(dk::Flags *ccr);	/* $00AA7A regs(&ccr)->d0 ccout(NVC) */
void Fade_FlashWhite6(dk::Flags *ccr);	/* $00AA9C regs(&ccr) ccout(NZVC) */
void Fade_InFromPale6(dk::Flags *ccr);	/* $00AB24 regs(&ccr) ccout(NVC) */
uint8_t Fade_MaxTint(void);	/* $00AB6C regs()->d0 clobbers(d1,a0) */
uint8_t Fade_MinTint(void);	/* $00AB92 regs()->d0 clobbers(d1,a0) */
void Fade_FlashWhite7(dk::Flags *ccr);	/* $00AABE regs(&ccr) ccout(NVC) */
void Fade_FlashWhite3(dk::Flags *ccr);	/* $00AAE0 regs(&ccr) ccout(NZVC) */
