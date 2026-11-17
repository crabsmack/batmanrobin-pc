/* funcs/Rand.h - prototypes of the Rand module: one line per function, its register convention
 * in the tag (dkit genesis recomp bridge). */
#pragma once

int32_t Rand_Next(void);	/* $00995A regs()->d0 tst(d0.l) */
void Rand_Signed(uint32_t *d0);	/* $009982 regs(&d0) */
void Rand_Below(uint32_t *d0);	/* $00998E regs(&d0) */
