/* funcs/VDP.h - prototypes of the VDP module: one line per function, its register
 * convention in the tag (dkit genesis recomp bridge). First written from the measured
 * contracts (dkit disasm regs); edited by the C's authors from then on. */
#pragma once
void VDP_ClearVRAM(void);	/* $00AD84 regs() clobbers(d0,d7,a0) */
void VDP_ClearCRAM(void);	/* $00ADA6 regs() clobbers(d0,d7,a0) */
void VDP_WhiteCRAM(void);	/* $00ADBA regs() clobbers(d0,d7,a0) */
void VDP_ClearVSRAM(void);	/* $00ADD4 regs() clobbers(d0,d7,a0) */
void VDP_ClearSAT(void);	/* $00ADE8 regs() clobbers(d0,d7,a0) */
void VDP_ClearHScroll(void);	/* $00ADFC regs() clobbers(d0,d7,a0) */
uint32_t VDP_ClearPlanes(void);	/* $00AE10 regs()->d0 clobbers(d7,a0) */
void VDP_LoadRegs(uint16_t *d0, uint32_t *d7, uint32_t a0);	/* $00AE2C regs(&d0,&d7,a0) clobbers(a0) */
void VDP_ClearLoop(uint16_t d7, uint32_t a0);	/* $00AD96 regs(d7,a0) clobbers(d0,d7) */
void VDP_FillLoop(uint32_t d0, uint16_t d7, uint32_t a0);	/* $00AD98 regs(d0,d7,a0) clobbers(d7) */
