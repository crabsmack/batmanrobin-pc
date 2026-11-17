/* funcs/Player.h - prototypes of the Player module: one line per function, its register
 * convention in the tag (dkit genesis recomp bridge). First written from the measured
 * contracts (dkit disasm regs); edited by the C's authors from then on. */
#pragma once
void Player_GetP1Var1(uint16_t *var, uint32_t *one, uint32_t *vm, dk::Flags *ccr);	/* $009DC6 regs(&d0,&d1,&a4,&ccr) ccout(Z) clobbers(a0,a6) */
void Player_GetP2Var1(uint16_t *var, uint32_t *one, uint32_t *vm, dk::Flags *ccr);	/* $009DE4 regs(&d0,&d1,&a4,&ccr) ccout(Z) clobbers(a0,a6) */
void Player_InitRecord(uint32_t record);	/* $0101A8 regs(a0) */
void Player_JoinOrSpawn(uint32_t *d0, uint32_t *d1, uint32_t d2, uint32_t d3, uint32_t d4, uint32_t d5, uint32_t d6, uint32_t d7, uint32_t *a0, uint32_t *a1, uint32_t *a2, uint32_t *a3, uint32_t *a4, Vm2Obj *self);	/* $0101CE regs(&d0,&d1,d2,d3,d4,d5,d6,d7,&a0,&a1,&a2,&a3,&a4,a6) clobbers(a5) */
void Player_Despawn(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6, uint32_t *d7, uint32_t *a0, uint32_t *a1, uint32_t *a2, uint32_t *a3, uint32_t *a4, uint32_t *a5, Vm2Obj *self);	/* $0102D6 regs(&d0,&d1,&d2,&d3,&d4,&d5,&d6,&d7,&a0,&a1,&a2,&a3,&a4,&a5,a6) ccout(Z) */
void Player_HurtFixed(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6, uint32_t *d7, uint32_t *a0, uint32_t *a1, uint32_t *a2, uint32_t *a3, uint32_t *a4, uint32_t *a5, Vm2Obj *self, dk::Flags *ccr);	/* $0103FE regs(&d0,&d1,&d2,&d3,&d4,&d5,&d6,&d7,&a0,&a1,&a2,&a3,&a4,&a5,a6,&ccr) ccout(NZVC) */
void Player_LoseLife(uint32_t *d0, uint32_t *a1, const Vm2Obj *self, dk::Flags *ccr);	/* $01048E regs(&d0,&a1,a6,&ccr) ccout(NZVC) clobbers(a0) */
void Player_SetExitFlag(const PlayerRec *rec, dk::Flags *ccr);	/* $01054A regs(a0,&ccr) ccout(NZVC) clobbers(a0) */
void Player_SetBits300(const PlayerRec *rec);	/* $010580 regs(a0) clobbers(a0) */
void Player_ClearBits300(const PlayerRec *rec);	/* $0105AE regs(a0) clobbers(a0) */
uint32_t Player_NewGameSetup(void);	/* $011038 regs()->a1 clobbers(d0,d1,a0) */
uint32_t Player_InitRecords(uint32_t *a0);	/* $011050 regs(&a0)->a1 clobbers(d0,d1) */
uint32_t Player_GatherStats(uint32_t *a0);	/* $011214 regs(&a0)->d0 clobbers(d1) */
void Player_AddStats(uint32_t *d0, PlayerRec *rec);	/* $011252 regs(&d0,a0) */
void Player_CopyPadToVar2(uint32_t d0, uint32_t d1, uint32_t d2, uint32_t d3, uint32_t d4, uint32_t d5, uint32_t d6, uint32_t d7, uint32_t a1, uint32_t a3, Vm2Block *block, uint32_t a5, uint32_t a6);	/* $04D342 regs(d0,d1,d2,d3,d4,d5,d6,d7,a1,a3,a4,a5,a6) */
void Player_DespawnNative(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6, uint32_t *d7, uint32_t *a0, uint32_t *a1, uint32_t *a2, uint32_t *a3, uint32_t *a4, uint32_t *a5, Vm2Obj *self);	/* $0102C8 regs(&d0,&d1,&d2,&d3,&d4,&d5,&d6,&d7,&a0,&a1,&a2,&a3,&a4,&a5,a6) ccout(Z) */
void Player_Run(uint32_t *d0, uint32_t *d1, uint32_t d2, uint32_t d3, uint32_t d4, uint32_t d5, uint32_t d6, uint32_t d7, uint32_t *a0, uint32_t a1, uint32_t *a2, uint32_t *a3, uint32_t *a4, const Vm2Obj *self);	/* $010322 regs(&d0,&d1,d2,d3,d4,d5,d6,d7,&a0,a1,&a2,&a3,&a4,a6) clobbers(a5) */
void Player_TakeHit(uint32_t *d0, uint32_t *d1, uint16_t *d2, uint16_t *d7, uint32_t *a1, Vm2Motion *mot, Vm2Sprite *spr, const Vm2Obj *self);	/* $010424 regs(&d0,&d1,&d2,&d7,&a1,a3,a5,a6) clobbers(a0) */
uint16_t Player_CaseSprVar26(const Vm2Sprite *spr);	/* $0104BA regs(a5)->d0 */
uint16_t Player_CaseStepFrame(const Vm2Block *vm, Vm2Sprite *spr);	/* $0104C0 regs(a4,a5)->d0 */
void Player_CaseCycle5(uint32_t *d0, Vm2Sprite *spr);	/* $010512 regs(&d0,a5) */
void Player_SetBits300All(uint32_t *a0);	/* $01056A regs(&a0) */
void Player_ClearBits300All(uint32_t *a0);	/* $010598 regs(&a0) */
void Player_RunScript(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6, uint32_t *d7, uint32_t *a0, uint32_t *a1, uint32_t *a2, uint32_t *a3, uint32_t *a4, uint32_t *a5, Vm2Obj *self, dk::Flags *ccr);	/* $01036E regs(&d0,&d1,&d2,&d3,&d4,&d5,&d6,&d7,&a0,&a1,&a2,&a3,&a4,&a5,a6,&ccr) ccout(NZVC) */
void Player_CopyPadToVar2Robin(uint32_t d0, uint32_t d1, uint32_t d2, uint32_t d3, uint32_t d4, uint32_t d5, uint32_t d6, uint32_t d7, uint32_t a1, uint32_t a3, Vm2Block *block, uint32_t a5, uint32_t a6);	/* $04FE7E regs(d0,d1,d2,d3,d4,d5,d6,d7,a1,a3,a4,a5,a6) */
