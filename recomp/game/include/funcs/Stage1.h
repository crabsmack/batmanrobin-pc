/* funcs/Stage1.h - prototypes of the Stage1 module: one line per function, its register
 * convention in the tag (dkit genesis recomp bridge). First written from the measured
 * contracts (dkit disasm regs); edited by the C's authors from then on. */
#pragma once
void Stage1_VIntFacade(uint32_t vdp_ctrl, uint32_t vdp_data, uint32_t a2);	/* $015C5E regs(a0,a1,a2) clobbers(d0,d1,d2,d3,d6,d7,a3,a4,a5) */
void Stage1_DrawSignPanel(uint8_t state, uint32_t vdp_ctrl, uint32_t vdp_data, const Stage1SignSlot *slot);	/* $015CA6 regs(d0,a0,a1,a4) clobbers(d0,d1,d2,d3,a3) */
void Stage1_UpdateStreetScroll(uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d6, uint32_t a1);	/* $015D34 regs(&d1,&d2,&d3,&d4,&d6,a1) clobbers(d0,d7,a0,a1) */
void Stage1_ScrollFacadeLines(uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint16_t *d6, uint32_t a1);	/* $015D90 regs(&d1,&d2,&d3,&d4,&d6,a1) clobbers(d0,d7,a0,a1) */
uint32_t Stage1_SpawnPoliceCar(uint16_t *d1, uint16_t d4, uint16_t d5, uint32_t *a0, uint32_t *a4, uint32_t *a5, uint32_t *a6);	/* $01E58C regs(&d1,d4,d5,&a0,&a4,&a5,&a6)->d0 tst(d0.l) ccout(Z) clobbers(d7,a3) */
void Stage1_RunStreet(uint32_t *d1, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6, uint32_t *a1, uint32_t *a2, uint32_t *a3, uint32_t *a4, uint32_t *a5, uint32_t *a6);	/* $01FB3A regs(&d1,&d3,&d4,&d5,&d6,&a1,&a2,&a3,&a4,&a5,&a6) clobbers(d0,d2,d7,a0) */
void Stage1_RunGemExpo(uint32_t *d1, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6, uint32_t *a1, uint32_t *a2, uint32_t *a3, uint32_t *a4, uint32_t *a5, uint32_t *a6);	/* $01FC50 regs(&d1,&d3,&d4,&d5,&d6,&a1,&a2,&a3,&a4,&a5,&a6) clobbers(d0,d2,d7,a0) */
void Stage1_RunWarehouse(uint32_t *d1, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6, uint32_t *a1, uint32_t *a2, uint32_t *a3, uint32_t *a4, uint32_t *a5, uint32_t *a6);	/* $01FD76 regs(&d1,&d3,&d4,&d5,&d6,&a1,&a2,&a3,&a4,&a5,&a6) clobbers(d0,d2,d7,a0) */
void Stage1_RunTruck(uint32_t *d1, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6, uint32_t *a1, uint32_t *a2, uint32_t *a3, uint32_t *a4, uint32_t *a5, uint32_t *a6);	/* $01FECC regs(&d1,&d3,&d4,&d5,&d6,&a1,&a2,&a3,&a4,&a5,&a6) clobbers(d0,d2,d7,a0) */
void Stage1_SpawnBalloonLauncherA(uint16_t *d1, uint16_t d4, uint16_t d5, uint32_t *d7, uint32_t *script, uint32_t *a3, uint32_t *a4, uint32_t *a6);	/* $01FFA6 regs(&d1,d4,d5,&d7,&a2,&a3,&a4,&a6) ccout(Z) clobbers(d0,a0,a5) */
void Stage1_SpawnBalloonLauncherB(uint16_t *d1, uint16_t d4, uint16_t d5, uint32_t *d7, uint32_t *script, uint32_t *a3, uint32_t *a4, uint32_t *a6);	/* $020082 regs(&d1,d4,d5,&d7,&a2,&a3,&a4,&a6) ccout(Z) clobbers(d0,a0,a5) */
uint32_t Stage1_BalloonUseHeightsA(BalloonLauncherObj *self);	/* $020276 regs(a6)->d0 tst(d0.l) ccout(Z) */
void Stage1_LaunchBalloonA(uint32_t *d0, uint16_t *d1, uint16_t *d4, uint16_t *d5, uint32_t *d7, uint32_t *a1, uint32_t a4, BalloonLauncherObj *self);	/* $02029A regs(&d0,&d1,&d4,&d5,&d7,&a1,a4,a6) clobbers(a0) */
void Stage1_BalloonBob(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint16_t *d3, Vm2Motion *motion, BalloonBombObj *self);	/* $0203AA regs(&d0,&d1,&d2,&d3,a3,a6) */
void Stage1_SpawnSectionExit(uint16_t *d1, uint16_t d4, uint16_t d5, uint32_t *d7, uint32_t *script, uint32_t *a3, uint32_t *a4, uint32_t *a6);	/* $0204C8 regs(&d1,d4,d5,&d7,&a2,&a3,&a4,&a6) ccout(Z) clobbers(d0,a0,a5) */
void Stage1_SpawnTrashCan(uint16_t *d1, uint16_t d4, uint16_t d5, uint32_t *a0, uint32_t *a4, uint32_t *a5, uint32_t *a6, uint32_t *script);	/* $0204F8 regs(&d1,d4,d5,&a0,&a4,&a5,&a6,&a2) ccout(Z) clobbers(d0,d7,a3) */
void Stage1_TrashCanHasDrop(uint32_t *d0, const Vm2Obj *self);	/* $020550 regs(&d0,a6) */
void Stage1_SpawnUrn(uint16_t *d1, uint16_t d4, uint16_t d5, uint32_t *a0, uint32_t *a4, uint32_t *a5, uint32_t *a6, uint32_t *script);	/* $02061C regs(&d1,d4,d5,&a0,&a4,&a5,&a6,&a2) ccout(Z) clobbers(d0,d7,a3) */
void Stage1_SpawnGiftBox(uint16_t *d1, uint16_t d4, uint16_t d5, uint32_t *a0, uint32_t *a4, uint32_t *a5, uint32_t *a6, uint32_t *script);	/* $020792 regs(&d1,d4,d5,&a0,&a4,&a5,&a6,&a2) ccout(Z) clobbers(d0,d7,a3) */
void Stage1_BlastSignChained(uint32_t *d0, uint32_t *d1, uint16_t d4, uint16_t *d5, uint16_t *d6, uint32_t *d7, uint32_t *a0, uint32_t *a1, uint32_t *a2, uint32_t *a3, uint32_t *a4, uint32_t *a5, uint32_t *a6);	/* $020866 regs(&d0,&d1,d4,&d5,&d6,&d7,&a0,&a1,&a2,&a3,&a4,&a5,&a6) ccout(NZVC) */
void Stage1_SpawnSignDebris(uint32_t *d1, uint16_t d4, uint16_t d5, uint16_t d6, uint32_t *d7, uint32_t *a0, uint32_t *a1, uint32_t *a2, uint32_t *a3, uint32_t *a4, uint32_t *a5, uint32_t *a6);	/* $0208BE regs(&d1,d4,d5,d6,&d7,&a0,&a1,&a2,&a3,&a4,&a5,&a6) clobbers(d0) */
void Stage1_InitSignDebris(uint32_t *d1, uint16_t d4, uint16_t d5, uint16_t d6, uint32_t *a3, uint32_t *a5);	/* $0208F8 regs(&d1,d4,d5,d6,&a3,&a5) clobbers(d0) */
void Stage1_SignDebrisRun(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint16_t *d6, uint16_t *d7, uint32_t *a0, uint32_t *a1, uint32_t *a4, SignDebrisObj *self);	/* $020A12 regs(&d0,&d1,&d2,&d3,&d4,&d5,&d6,&d7,&a0,&a1,&a4,a6) clobbers(a5) */
void Stage1_StepSignDebris(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint16_t *d6, uint32_t *a0, uint32_t *a1, uint32_t *a5, SignDebrisObj *self);	/* $020A48 regs(&d0,&d1,&d2,&d3,&d4,&d5,&d6,&a0,&a1,&a5,a6) */
uint32_t Stage1_SpawnBankBombing(uint16_t *d1, uint16_t d4, uint16_t d5, uint32_t *a0, uint32_t *a1, uint32_t *a4, uint32_t *a5, uint32_t *a6);	/* $020CDC regs(&d1,d4,d5,&a0,&a1,&a4,&a5,&a6)->d0 tst(d0.l) ccout(Z) clobbers(d7,a2,a3) */
uint32_t Stage1_BombingStartWave(uint16_t d4, uint16_t d5, const BankBombingObj *self, uint32_t *script);	/* $020D76 regs(d4,d5,a6,&a5)->d0 clobbers(a0,a1,a6) */
uint32_t Stage1_StartFireFlicker(uint32_t a6);	/* $020DDA regs(a6)->d0 clobbers(a0,a1,a6) */
void Stage1_VIntFireStep(uint8_t state, uint32_t vdp_ctrl, uint32_t vdp_data, uint32_t dma_cmd);	/* $0159B4 regs(d0,a0,a1,a2) clobbers(d0,d1,d2,d3,d7,a3,a4) */
void Stage1_StartFireAt(uint32_t *d0, uint32_t *d1, uint16_t *d2, uint16_t d4);	/* $015E22 regs(&d0,&d1,&d2,d4) tst(d0.l) ccout(Z) */
void Stage1_SwayFireLines(uint32_t *d1, uint32_t *d4);	/* $015E7E regs(&d1,&d4) clobbers(d0,d7,a0) */
void Stage1_WaveSkyLines(uint16_t *d1, uint32_t *d6);	/* $015EEE regs(&d1,&d6) clobbers(d0,d2,d7,a0,a1) */
uint32_t unk_Stage1_01FF78(uint32_t d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6, uint32_t *d7, uint32_t *a0, uint32_t *a1, uint32_t *a2, uint32_t *a3, uint32_t *a4, uint32_t *a5, uint32_t *a6);	/* $01FF78 regs(d0,&d1,&d2,&d3,&d4,&d5,&d6,&d7,&a0,&a1,&a2,&a3,&a4,&a5,&a6)->d0 tst(d0.l) ccout(Z) */
uint32_t Stage1_BalloonUseHeightsB(BalloonLauncherObj *self);	/* $020282 regs(a6)->d0 tst(d0.l) ccout(Z) */
void Stage1_LaunchBalloonB(uint32_t *d0, uint16_t *d1, uint16_t *d4, uint16_t *d5, uint32_t *d7, uint32_t *a1, uint32_t a4, BalloonLauncherObj *self);	/* $0202D8 regs(&d0,&d1,&d4,&d5,&d7,&a1,a4,a6) clobbers(a0) */
void Stage1_BlastSignAt(uint32_t *d0, uint32_t *d1, uint16_t d4, uint16_t *d5, uint16_t *d6, uint32_t *d7, uint32_t a6);	/* $020868 regs(&d0,&d1,d4,&d5,&d6,&d7,a6) clobbers(a0,a1,a2,a3,a4,a5,a6) */
uint32_t Stage1_StartRubbleRain(uint32_t a6);	/* $020AA8 regs(a6)->d0 clobbers(a0,a1,a6) */
void Stage1_RubbleRainRun(uint32_t *d0, uint32_t *a0, uint32_t *a1, uint32_t *a6);	/* $020ACE regs(&d0,&a0,&a1,&a6) clobbers(a5) */
void Stage1_RubblePieceRun(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6, uint32_t *d7, uint32_t *a0, uint32_t *a1, RubblePieceObj *self);	/* $020B54 regs(&d0,&d1,&d2,&d3,&d4,&d5,&d6,&d7,&a0,&a1,a6) */
/* the scene's terrain routine as Stage1_RubblePieceRun calls it (jsr (a0) through g_TerrainFn: d1/d2 the move,
   d4/d5 the position, d6/d7 the masks; the routine leaves the position and the contact in d4/d5/d7) */
void Stage1RubbleTerrainFn(uint32_t code, uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6, uint32_t *d7, uint32_t *a0, uint32_t *a1, RubblePieceObj *a6);	/* type regs(&d0,&d1,&d2,&d3,&d4,&d5,&d6,&d7,&a0,&a1,a6) */
void Stage1_BombingExplosionRow(uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *a0, uint32_t *script, uint32_t *a6);	/* $020D80 regs(&d2,&d3,&d4,&d5,&a0,&a4,&a6) ccout(NZVC) */
void Stage1_BombingBigBlast(uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *a0, uint32_t *script, uint32_t *a6);	/* $020D8C regs(&d2,&d3,&d4,&d5,&a0,&a4,&a6) ccout(NZVC) */
void Stage1_BombingStartFire(uint32_t *d0, uint32_t *d1, uint16_t *d2, uint16_t *d4);	/* $020D98 regs(&d0,&d1,&d2,&d4) */
void Stage1_BombingBlastLeftSign(uint32_t *d0, uint32_t *d1, uint16_t *d4, uint16_t *d5, uint16_t *d6, uint32_t *d7, uint32_t a6);	/* $020DA8 regs(&d0,&d1,&d4,&d5,&d6,&d7,a6) clobbers(a0,a1,a2,a3,a4,a5,a6) */
void Stage1_BombingBlastRightSign(uint32_t *d0, uint32_t *d1, uint16_t *d4, uint16_t *d5, uint16_t *d6, uint32_t *d7, uint32_t a6);	/* $020DB6 regs(&d0,&d1,&d4,&d5,&d6,&d7,a6) clobbers(a0,a1,a2,a3,a4,a5,a6) */
void Stage1_FireFlickerRun(uint32_t *d0, uint32_t *a0, uint32_t *a1, FireFlickerObj *self);	/* $020DF0 regs(&d0,&a0,&a1,a6) */
void Stage1_LaunchBalloonC(uint32_t *d0, uint16_t *d1, uint16_t *d4, uint16_t *d5, uint32_t *d7, uint32_t *a1, uint32_t a4, BalloonLauncherObj *self);	/* $020316 regs(&d0,&d1,&d4,&d5,&d7,&a1,a4,a6) clobbers(a0) */
