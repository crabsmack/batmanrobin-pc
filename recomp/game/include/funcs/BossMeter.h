/* funcs/BossMeter.h - prototypes of the BossMeter module: one line per function, its register
 * convention in the tag (dkit genesis recomp bridge). First written from the measured
 * contracts (dkit disasm regs); edited by the C's authors from then on. */
#pragma once
void BossMeter_Spawn(uint32_t *d0, uint16_t *d1, uint16_t d4, uint16_t d5, uint32_t *d7, uint32_t a4, uint32_t a6);	/* $011AEE regs(&d0,&d1,d4,d5,&d7,a4,a6) tst(d0.l) ccout(NVC) clobbers(a0) */
uint16_t BossMeter_AddPart(const Vm2Obj *part, dk::Flags *ccr);	/* $011B06 regs(a6,&ccr)->d0 ccout(NVC) */
void BossMeter_SpawnObject(uint32_t *d0, uint16_t *d1, uint16_t d4, uint16_t d5, uint32_t *d7, uint32_t a4, uint32_t *a6);	/* $011B32 regs(&d0,&d1,d4,d5,&d7,a4,&a6) ccout(NZVC) clobbers(a0,a2,a3,a4,a5) */
void BossMeter_Exists(uint32_t *d0);	/* $011BBA regs(&d0) */
void BossMeter_DrawDigit(uint32_t *d0, uint32_t *d1, uint32_t *a1, uint32_t a2);	/* $011C5E regs(&d0,&d1,&a1,a2) clobbers(a0,a2) */
void BossMeter_Show(uint32_t *d0, uint16_t *d1, uint16_t d4, uint16_t d5, uint32_t *d7, uint32_t a4, uint32_t a6);	/* $011ACA regs(&d0,&d1,d4,d5,&d7,a4,a6) tst(d0.l) ccout(NVC) clobbers(a0) */
uint16_t BossMeter_Defeated(dk::Flags *ccr);	/* $011B14 regs(&ccr)->d0 ccout(NVC) clobbers(a0) */
void BossMeter_AnimRing(uint16_t *d0, uint32_t *d6, uint32_t *d7, uint32_t *a1, Vm2Block *vm);	/* $011BC4 regs(&d0,&d6,&d7,&a1,a4) clobbers(a0) */
void BossMeter_DrawPercent(uint32_t *d0, uint32_t *d1, uint32_t *a1, const Vm2Block *vm);	/* $011C20 regs(&d0,&d1,&a1,a4) clobbers(a0) */
void BossMeter_QueueTileDMA(uint32_t *d0, uint32_t *d1, uint32_t a0);	/* $011CB2 regs(&d0,&d1,a0) clobbers(a0) */
uint16_t BossMeter_DrawSprite(const Vm2Sprite *spr, const Vm2Obj *self);	/* $011CD8 regs(a5,a6)->d0 clobbers(a0) */
void BossMeter_Init(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6, uint32_t *d7, uint32_t *a1, uint32_t a3, Vm2Obj *self);	/* $011D04 regs(&d0,&d1,&d2,&d3,&d4,&d5,&d6,&d7,&a1,a3,a6) clobbers(a0) */
void BossMeter_Free(uint32_t *d0, uint16_t *d1);	/* $011D4E regs(&d0,&d1) clobbers(a0) */
