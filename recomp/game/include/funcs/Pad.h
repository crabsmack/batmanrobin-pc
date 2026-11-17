/* funcs/Pad.h - prototypes of the Pad module: one line per function, its register
 * convention in the tag (dkit genesis recomp bridge). First written from the measured
 * contracts (dkit disasm regs); edited by the C's authors from then on. */
#pragma once
uint32_t Pad_CheckStart(void);	/* $00A196 regs()->d0 */
void Pad_ReadAll(void);	/* $00A3AE regs() clobbers(d0,d1,a4,a5) */
void Pad_ReadLive(uint32_t *port, uint32_t *rec);	/* $00A3D4 regs(&a4,&a5) clobbers(d0,d1) */
void Pad_Update(uint32_t port, PadRec **rec);	/* $00A3E8 regs(a4,&a5) clobbers(d0,d1) */
uint8_t Pad_ReadPort(uint32_t port);	/* $00A400 regs(a4)->d0 clobbers(d1) */
