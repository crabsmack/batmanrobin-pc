/* funcs/Shot.h - prototypes of the Shot module: one line per function, its register
 * convention in the tag (dkit genesis recomp bridge). First written from the measured
 * contracts (dkit disasm regs); edited by the C's authors from then on. */
#pragma once
void Shot_DamageTarget(uint16_t *d0, HitBox *box, const s16 **script, Vm4Obj *self, dk::Flags *ccr);	/* $010C78 regs(&d0,a2,&a4,a6,&ccr) ccout(NZVC) */
