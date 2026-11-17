/* funcs/Shake.h - prototypes of the Shake module: one line per function, its register
 * convention in the tag (dkit genesis recomp bridge). First written from the measured
 * contracts (dkit disasm regs); edited by the C's authors from then on. */
#pragma once
void Shake_StartIfIdle(dk::Flags *ccr);	/* $00959C regs(&ccr) ccout(NVC) */
void Shake_Start(void);	/* $0095A2 regs() ccout(NVC) */
void Shake_StartBig(void);	/* $0095EC regs() ccout(NZVC) */
void Shake_Update(uint32_t *d1, uint32_t *d2);	/* $009642 regs(&d1,&d2) clobbers(d0) */
void Shake_Arm(dk::Flags *ccr);	/* $0095B2 regs(&ccr) ccout(NZVC) */
void Shake_StartLongIfIdle(dk::Flags *ccr);	/* $0095CE regs(&ccr) ccout(NVC) */
void Shake_StartBigIfIdle(dk::Flags *ccr);	/* $0095E6 regs(&ccr) ccout(NVC) */
void Shake_StartRandomIfIdle(uint32_t *d0, dk::Flags *ccr);	/* $0095FE regs(&d0,&ccr) ccout(NVC) */
void Shake_Rumble(uint32_t *d1, uint32_t *d2);	/* $00961E regs(&d1,&d2) clobbers(d0) */
void Shake_Done(void);	/* $0095CC regs() */
void Shake_StartLong(void);	/* $0095D4 regs() ccout(NVC) */
void Shake_StartRandom(uint32_t *d0);	/* $009604 regs(&d0) ccout(NVC) */
