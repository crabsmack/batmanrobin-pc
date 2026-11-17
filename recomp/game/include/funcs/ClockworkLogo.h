/* funcs/ClockworkLogo.h - prototypes of the ClockworkLogo module: one line per function, its register
 * convention in the tag (dkit genesis recomp bridge). First written from the measured
 * contracts (dkit disasm regs); edited by the C's authors from then on. */
#pragma once
void ClockworkLogo_Spawn(uint32_t *d0, uint16_t *d1, uint16_t d4, uint16_t d5, uint32_t *d7, uint32_t *a2, uint32_t *a3, uint32_t *a4, uint32_t a6);	/* $049E2E regs(&d0,&d1,d4,d5,&d7,&a2,&a3,&a4,a6) clobbers(a0,a5,a6) */
uint16_t ClockworkLogo_ScrollPicture(const Vm2Obj *self);	/* $049E98 regs(a6)->d0 */
void ClockworkLogo_Run(uint32_t *d1, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6, uint32_t *a1, uint32_t *a2, uint32_t *a3, uint32_t *a4, uint32_t *a5, uint32_t *a6);	/* $049E06 regs(&d1,&d3,&d4,&d5,&d6,&a1,&a2,&a3,&a4,&a5,&a6) clobbers(d0,d2,d7,a0) */
void ClockworkLogo_CycleColours(uint32_t *d0, uint32_t *d7, uint32_t *a1, Vm2Block *vm);	/* $049EA4 regs(&d0,&d7,&a1,a4) clobbers(a0) */
