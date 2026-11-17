/* funcs/Stage2.h - prototypes of the Stage2 module: one line per function, its register
 * convention in the tag (dkit genesis recomp bridge). First written from the measured
 * contracts (dkit disasm regs); edited by the C's authors from then on. */
#pragma once
uint32_t Stage2_LiftStop(void);	/* $0240EE regs()->d0 */
void Stage2_LiftArriveTop(uint32_t *d0, uint16_t *d1, uint16_t d4, uint16_t d5, uint32_t *d7, uint32_t *a2, uint32_t *a3, uint32_t *a4, uint32_t *a5, uint32_t *a6);	/* $02417E regs(&d0,&d1,d4,d5,&d7,&a2,&a3,&a4,&a5,&a6) clobbers(a0) */
void Stage2_LiftVIntScroll(uint32_t a0, uint32_t a1, uint32_t a2);	/* $0244A8 regs(a0,a1,a2) clobbers(d0,d1,d7,a3,a4,a5) */
void Stage2_ArenaVIntUpload(uint32_t *d0, uint32_t *d7, uint32_t vdp_ctrl, uint32_t a1, uint32_t a2, uint32_t a3, uint32_t *a4);	/* $024576 regs(&d0,&d7,a0,a1,a2,a3,&a4) */
void Stage2_ArenaVIntColumns(uint32_t *d0, uint32_t vdp_ctrl, uint32_t a2);	/* $0245B0 regs(&d0,a0,a2) */
void Stage2_ArenaHIntSplit(uint32_t a0, uint32_t a1);	/* $0245E0 regs(a0,a1) ccout(NZVC) */
void Stage2_ArenaDrawTilt(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d7, uint32_t *a0, uint32_t *a1);	/* $024660 regs(&d0,&d1,&d2,&d3,&d4,&d5,&d7,&a0,&a1) */
void Stage2_MoveOnScrollingMap(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6, uint32_t *d7, uint32_t a0, uint32_t *a1, uint32_t *a2, Vm2Motion **motion, uint32_t *a4, uint32_t *a5, Vm2Obj **object);	/* $024F70 regs(&d0,&d1,&d2,&d3,&d4,&d5,&d6,&d7,a0,&a1,&a2,&a3,&a4,&a5,&a6) clobbers(a0) */
void Stage2_SetSpawnVarsFromEntry(const u16 **entry, Vm2Block *vm, dk::Flags *ccr);	/* $025554 regs(&a1,a4,&ccr) ccout(Z) clobbers(d0) */
void Stage2_RunLift(uint32_t *d1, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6, uint32_t *a1, uint32_t *a2, uint32_t *a3, uint32_t *a4, uint32_t *a5, uint32_t *a6);	/* $032F14 regs(&d1,&d3,&d4,&d5,&d6,&a1,&a2,&a3,&a4,&a5,&a6) clobbers(d0,d2,d7,a0) */
void Stage2_RunFlight(uint32_t *d1, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6, uint32_t *a1, uint32_t *a2, uint32_t *a3, uint32_t *a4, uint32_t *a5, uint32_t *a6);	/* $03324A regs(&d1,&d3,&d4,&d5,&d6,&a1,&a2,&a3,&a4,&a5,&a6) clobbers(d0,d2,d7,a0) */
void Stage2_RunFlightClouds(uint32_t *d1, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6, uint32_t *a1, uint32_t *a2, uint32_t *a3, uint32_t *a4, uint32_t *a5, uint32_t *a6);	/* $03325A regs(&d1,&d3,&d4,&d5,&d6,&a1,&a2,&a3,&a4,&a5,&a6) clobbers(d0,d2,d7,a0) */
void Stage2_LiftLampInitY(uint32_t d1, uint32_t d2, uint32_t d3, uint32_t d4, uint32_t d5, uint32_t d6, uint32_t d7, uint32_t a1, uint32_t a3, uint32_t a4, uint32_t a5, Vm2Obj *self);	/* $033BD8 regs(d1,d2,d3,d4,d5,d6,d7,a1,a3,a4,a5,a6) */
uint32_t Stage2_LiftStart(void);	/* $0240F6 regs()->d0 */
void Stage2_LiftUpdate(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint16_t d4, uint16_t d5, uint32_t *d7, uint32_t *a2, uint32_t *a3, uint32_t *a4, uint32_t *a5, uint32_t *a6);	/* $02410A regs(&d0,&d1,&d2,d4,d5,&d7,&a2,&a3,&a4,&a5,&a6) clobbers(a0) */
void Stage2_ArenaRequestTilt(dk::Flags *ccr);	/* $0241D0 regs(&ccr) ccout(NZVC) */
void Stage2_LiftInstallVInt(uint32_t a6);	/* $0241EA regs(a6) clobbers(d0,a0,a1,a6) */
void Stage2_UpdateScroll(uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6, uint32_t d7, uint32_t a1, uint32_t *a2, uint32_t *a3, uint32_t *a4, uint32_t *a5, uint32_t *a6);	/* $0241FC regs(&d1,&d2,&d3,&d4,&d5,&d6,d7,a1,&a2,&a3,&a4,&a5,&a6) clobbers(d0,d7,a0,a1) */
void Stage2_LiftHIntFloor(uint32_t d0, uint32_t a0);	/* $024526 regs(d0,a0) ccout(NZVC) */
void Stage2_ArenaVIntScroll(uint32_t a0, uint32_t a2);	/* $02456A regs(a0,a2) clobbers(d0,d7,a4) */
void Stage2_LiftColumnScroll(uint32_t *d1, uint16_t *d6);	/* $02461C regs(&d1,&d6) clobbers(d0,d7,a0,a1) */
void Stage2_SpawnLeapingThug(uint16_t *d1, uint16_t d4, uint16_t d5, uint32_t *d7, uint32_t *a1, uint32_t *a2, uint32_t *a3, uint32_t *a4, uint32_t *a6);	/* $025560 regs(&d1,d4,d5,&d7,&a1,&a2,&a3,&a4,&a6) ccout(Z) clobbers(d0,a0,a5) */
void Stage2_RunFlightAirship(uint32_t *d1, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6, uint32_t *a1, uint32_t *a2, uint32_t *a3, uint32_t *a4, uint32_t *a5, uint32_t *a6);	/* $03326A regs(&d1,&d3,&d4,&d5,&d6,&a1,&a2,&a3,&a4,&a5,&a6) clobbers(d0,d2,d7,a0) */
void Stage2_RunFlightBoss(uint32_t *d1, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6, uint32_t *a1, uint32_t *a2, uint32_t *a3, uint32_t *a4, uint32_t *a5, uint32_t *a6);	/* $033270 regs(&d1,&d3,&d4,&d5,&d6,&a1,&a2,&a3,&a4,&a5,&a6) clobbers(d0,d2,d7,a0) */
void Stage2_SpawnLiftLamp(uint16_t *d1, uint16_t d4, uint16_t d5, uint32_t *a0, uint32_t *a2, uint32_t *a4, uint32_t *a5, uint32_t *a6);	/* $033BAC regs(&d1,d4,d5,&a0,&a2,&a4,&a5,&a6) ccout(Z) clobbers(d0,d7,a3) */
