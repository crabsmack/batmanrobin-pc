/* funcs/PlayerSelect.h - prototypes of the PlayerSelect module: one line per function, its register
 * convention in the tag (dkit genesis recomp bridge). First written from the measured
 * contracts (dkit disasm regs); edited by the C's authors from then on. */
#pragma once
void PlayerSelect_Run(uint32_t *d1, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6, uint32_t *a2, uint32_t *a3, uint32_t *a4, uint32_t *a5, uint32_t *a6);	/* $049AA6 regs(&d1,&d3,&d4,&d5,&d6,&a2,&a3,&a4,&a5,&a6) clobbers(d0,d2,d7,a0,a1) */
void PlayerSelect_ScrollHalves(uint16_t *d0, uint32_t *d1, uint16_t *d2, uint16_t *d7, uint32_t *a1, const Vm2Obj *self);	/* $049AD4 regs(&d0,&d1,&d2,&d7,&a1,a6) clobbers(a0) */
void PlayerSelect_SpawnChooser(uint16_t *d1, uint16_t d4, uint16_t d5, uint32_t *a3, uint32_t *a4, uint32_t *a6);	/* $049B18 regs(&d1,d4,d5,&a3,&a4,&a6) clobbers(d0,d7,a0,a2,a5) */
void PlayerSelect_ToggleHero(uint32_t d0, uint32_t d1, uint32_t d2, uint32_t d3, uint32_t d4, uint32_t d5, uint32_t d6, uint32_t d7, uint32_t a1, uint32_t a3, uint32_t a4, uint32_t a5, uint32_t a6);	/* $049B72 regs(d0,d1,d2,d3,d4,d5,d6,d7,a1,a3,a4,a5,a6) */
void PlayerSelect_SlideBatmanHalf(Vm2Motion *motion, Vm2Obj *self);	/* $049BA0 regs(a3,a6) */
void PlayerSelect_SpawnCaption(uint32_t *d0, uint16_t *d1, uint16_t d4, uint16_t d5, uint32_t *d7, uint32_t *a2, uint32_t *a3, uint32_t *a4, uint32_t a6);	/* $049C3C regs(&d0,&d1,d4,d5,&d7,&a2,&a3,&a4,a6) clobbers(a0,a5,a6) */
void PlayerSelect_DrawCaption(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint16_t *d6, const Vm2Obj *self);	/* $049C88 regs(&d0,&d1,&d2,&d3,&d4,&d5,&d6,a6) clobbers(a0) */
void PlayerSelect_ReadPad(uint16_t *d0, Vm2Block *vm);	/* $049B04 regs(&d0,a4) clobbers(a0) */
void PlayerSelect_SlideRobinHalf(Vm2Motion *motion, Vm2Obj *self);	/* $049BEE regs(a3,a6) */
