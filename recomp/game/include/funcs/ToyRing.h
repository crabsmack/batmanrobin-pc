/* funcs/ToyRing.h - prototypes of the ToyRing module: one line per function, its register
 * convention in the tag (dkit genesis recomp bridge). First written from the measured
 * contracts (dkit disasm regs); edited by the C's authors from then on. */
#pragma once
void ToyRing_DoubleKnockback(uint32_t d1, uint32_t d2, uint32_t d3, uint32_t d4, uint32_t d5, uint32_t d6, uint32_t d7, uint32_t a1, Vm2Motion *motion, uint32_t a4, uint32_t a5, uint32_t a6);	/* $0375F6 regs(d1,d2,d3,d4,d5,d6,d7,a1,a3,a4,a5,a6) */
void ToyRing_StartBreakUp(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint16_t *d7, uint32_t *a1, const Vm2Block *vm, const Vm2Obj *self);	/* $03768C regs(&d0,&d1,&d2,&d7,&a1,a4,a6) clobbers(a0) */
void ToyRing_ReleaseFrames(uint16_t *d0, uint16_t *d2, uint16_t *d7, uint32_t *a1, const Vm2Block *vm, const Vm2Obj *self);	/* $037708 regs(&d0,&d2,&d7,&a1,a4,a6) clobbers(a0) */
void ToyRing_SpawnBig(uint16_t *d1, uint16_t d4, uint16_t d5, uint16_t *d7, uint32_t *a2, uint32_t *a3, uint32_t *a4, uint32_t *a6);	/* $03904E regs(&d1,d4,d5,&d7,&a2,&a3,&a4,&a6) ccout(Z) clobbers(d0,a0,a5) */
void ToyRing_SpawnRandom(uint16_t *d1, uint16_t d4, uint16_t d5, uint16_t *d7, uint32_t *a2, uint32_t *a3, uint32_t *a4, uint32_t *a6);	/* $039176 regs(&d1,d4,d5,&d7,&a2,&a3,&a4,&a6) ccout(Z) clobbers(d0,a0,a5) */
void ToyRing_SpawnSix(uint16_t *d1, uint16_t d4, uint16_t d5, uint16_t *d7, uint32_t *a2, uint32_t *a3, uint32_t *a4, uint32_t *a6);	/* $0391E4 regs(&d1,d4,d5,&d7,&a2,&a3,&a4,&a6) ccout(Z) clobbers(d0,a0,a5) */
void ToyRing_SpawnRandomAlt(uint16_t *d1, uint16_t d4, uint16_t d5, uint16_t *d7, uint32_t *a0, uint32_t *a2, uint32_t *a3, uint32_t *a4, uint32_t *a5, uint32_t *a6);	/* $03925C regs(&d1,d4,d5,&d7,&a0,&a2,&a3,&a4,&a5,&a6) ccout(Z) clobbers(d0) */
uint32_t ToyRing_NextFigurePos(Vm2Block *vm, Vm2Obj *self);	/* $0376C0 regs(a4,a6)->d0 clobbers(a0) */
uint32_t ToyRing_SetSquashSpeedArg(const Vm2Obj *self);	/* $037724 regs(a6)->d0 clobbers(a0) */
void ToyRing_SetSquashSpeed(uint32_t *d0, const u16 **script, const Vm2Obj *self);	/* $03772A regs(&d0,&a2,a6) clobbers(a0) */
uint32_t ToyRing_TestRebound(const Vm2Obj *self);	/* $03773A regs(a6)->d0 clobbers(a0) */
void ToyRing_Init(uint32_t *d0, uint32_t *d1, uint32_t *d2, const u16 **script, Vm2Block *vm, const Vm2Obj *self);	/* $037752 regs(&d0,&d1,&d2,&a2,a4,a6) clobbers(a0) */
void ToyRing_Update(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6, uint32_t *d7, uint32_t *a1, Vm2Motion *motion, Vm2Block *vm, const Vm2Sprite *spr, Vm2Obj *self);	/* $037796 regs(&d0,&d1,&d2,&d3,&d4,&d5,&d6,&d7,&a1,a3,a4,a5,a6) clobbers(a0) */
/* The spawners no 4:3 corpus run calls (ws-fix, 2026-10-30): the contract of their siblings above, the
   one caller being the same (VM5_SpawnEntry's jsr (a0) $007016, which reads Z and the a6 left) */
void ToyRing_SpawnSixB(uint16_t *d1, uint16_t d4, uint16_t d5, uint16_t *d7, uint32_t *a2, uint32_t *a3, uint32_t *a4, uint32_t *a6);	/* $0391AA regs(&d1,d4,d5,&d7,&a2,&a3,&a4,&a6) ccout(Z) clobbers(d0,a0,a5) */
void ToyRing_SpawnSixC(uint16_t *d1, uint16_t d4, uint16_t d5, uint16_t *d7, uint32_t *a2, uint32_t *a3, uint32_t *a4, uint32_t *a6);	/* $039290 regs(&d1,d4,d5,&d7,&a2,&a3,&a4,&a6) ccout(Z) clobbers(d0,a0,a5) */
void ToyRing_SpawnBigB(uint16_t *d1, uint16_t d4, uint16_t d5, uint16_t *d7, uint32_t *a2, uint32_t *a3, uint32_t *a4, uint32_t *a6);	/* $039018 regs(&d1,d4,d5,&d7,&a2,&a3,&a4,&a6) ccout(Z) clobbers(d0,a0,a5) */
