/* funcs/Pause.h - prototypes of the Pause module: one line per function, its register
 * convention in the tag (dkit genesis recomp bridge). First written from the measured
 * contracts (dkit disasm regs); edited by the C's authors from then on. */
#pragma once
void Pause_Update(uint32_t *pad, uint32_t *code);	/* $009C50 regs(&a0,&a1) clobbers(d0) */
