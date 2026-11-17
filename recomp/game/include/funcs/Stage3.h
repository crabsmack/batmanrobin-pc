/* funcs/Stage3.h - prototypes of the Stage3 module: one line per function, its register
 * convention in the tag (dkit genesis recomp bridge). First written from the measured
 * contracts (dkit disasm regs); edited by the C's authors from then on. */
#pragma once
void Stage3_SpawnRevealDirector(uint32_t *d0, uint16_t *d1, uint16_t d4, uint16_t d5, uint32_t *d7, uint32_t *a0, uint32_t *a2, uint32_t *a3, uint32_t *a4, uint32_t *a5, uint32_t *a6);	/* $03CB84 regs(&d0,&d1,d4,d5,&d7,&a0,&a2,&a3,&a4,&a5,&a6) ccout(Z) */
void Stage3_RevealInit(uint32_t *d0, uint16_t *d6, uint16_t *d7, uint32_t *a1, uint32_t *a6);	/* $03CC82 regs(&d0,&d6,&d7,&a1,&a6) clobbers(a0) */
void Stage3_RevealVIntFlush(uint32_t a0, uint32_t a1);	/* $03CCD4 regs(a0,a1) clobbers(d7,a4) */
void Stage3_RevealPlaceTile(uint16_t *d0, uint32_t *d1, uint32_t *d2, uint16_t *d4, uint16_t *d5, RevealPiece *piece, dk::Flags *ccr);	/* $03CEF8 regs(&d0,&d1,&d2,&d4,&d5,a5,&ccr) ccout(NZVC) clobbers(a0) */
void Stage3_Line3CycleStep(uint16_t *d0, uint16_t *d1, uint16_t *d2, uint16_t *d3, uint16_t *d4, uint16_t *d5);	/* $03D000 regs(&d0,&d1,&d2,&d3,&d4,&d5) */
void Stage3_PlaceRoadEndSwirl(uint32_t *d4, uint16_t *d5, uint32_t *a1, Vm2Obj *swirl);	/* $03DC6A regs(&d4,&d5,&a1,a6) */
uint32_t Stage3_RoadPlaceAtHorizon(Vm2Obj *obj);	/* $03DDD2 regs(a6)->d0 */
void Stage3_RunBacklot(uint32_t *d1, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6, uint32_t *a1, uint32_t *a2, uint32_t *a3, uint32_t *a4, uint32_t *a5, uint32_t *a6);	/* $03FE94 regs(&d1,&d3,&d4,&d5,&d6,&a1,&a2,&a3,&a4,&a5,&a6) clobbers(d0,d2,d7,a0) */
void Stage3_RunCatHeadBoss(uint32_t *d1, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6, uint32_t *a1, uint32_t *a2, uint32_t *a3, uint32_t *a4, uint32_t *a5, uint32_t *a6);	/* $03FFBE regs(&d1,&d3,&d4,&d5,&d6,&a1,&a2,&a3,&a4,&a5,&a6) clobbers(d0,d2,d7,a0) */
void Stage3_RunMushroomForest(uint32_t *d1, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6, uint32_t *a1, uint32_t *a2, uint32_t *a3, uint32_t *a4, uint32_t *a5, uint32_t *a6);	/* $0400E4 regs(&d1,&d3,&d4,&d5,&d6,&a1,&a2,&a3,&a4,&a5,&a6) clobbers(d0,d2,d7,a0) */
void Stage3_RunSpottedRoom(uint32_t *d1, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6, uint32_t *a1, uint32_t *a2, uint32_t *a3, uint32_t *a4, uint32_t *a5, uint32_t *a6);	/* $0401DA regs(&d1,&d3,&d4,&d5,&d6,&a1,&a2,&a3,&a4,&a5,&a6) clobbers(d0,d2,d7,a0) */
void Stage3_SpottedRoomInitScroll(void);	/* $040258 regs() */
void Stage3_SpottedRoomScroll(uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6, uint32_t *a2, uint32_t *a3);	/* $040272 regs(&d1,&d2,&d3,&d4,&d5,&d6,&a2,&a3) clobbers(d0,d7) */
void Stage3_RunCardHall(uint32_t *d1, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6, uint32_t *a1, uint32_t *a2, uint32_t *a3, uint32_t *a4, uint32_t *a5, uint32_t *a6);	/* $0403F6 regs(&d1,&d3,&d4,&d5,&d6,&a1,&a2,&a3,&a4,&a5,&a6) clobbers(d0,d2,d7,a0) */
void Stage3_CardHallFloorScroll(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t d3, uint16_t *d7, uint32_t *a1);	/* $040460 regs(&d0,&d1,&d2,d3,&d7,&a1) clobbers(a0) */
void Stage3_RunHatterRoad(uint32_t *d1, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6, uint32_t *a1, uint32_t *a2, uint32_t *a3, uint32_t *a4, uint32_t *a5, uint32_t *a6);	/* $040574 regs(&d1,&d3,&d4,&d5,&d6,&a1,&a2,&a3,&a4,&a5,&a6) clobbers(d0,d2,d7,a0) */
void Stage3_InitHatterRoad(uint16_t d4, uint16_t d5, uint32_t *a3, uint32_t *a4, uint32_t a6);	/* $0405F2 regs(d4,d5,&a3,&a4,a6) clobbers(d0,d1,d7,a0,a1,a2,a5,a6) */
void Stage3_SpawnRoadDriver(uint16_t d4, uint16_t d5, uint32_t *script, uint32_t *a3, uint32_t *a4, uint32_t *a6);	/* $04060E regs(d4,d5,&a2,&a3,&a4,&a6) clobbers(d0,d1,d7,a0,a5) */
void Stage3_RoadCameraFollow(uint32_t *d0, uint32_t *d1, uint32_t *d2);	/* $04062A regs(&d0,&d1,&d2) */
void Stage3_DrawRoadLinesRun(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6, uint32_t *d7, uint32_t *a0, uint32_t *a1, uint32_t *a2, uint32_t *a3, uint32_t *a4, uint32_t *a5, uint32_t *a6, dk::Flags *ccr);	/* $040980 regs(&d0,&d1,&d2,&d3,&d4,&d5,&d6,&d7,&a0,&a1,&a2,&a3,&a4,&a5,&a6,&ccr) ccout(NZVC) */
void Stage3_RevealBurst(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6, uint32_t *d7, uint32_t a1, uint32_t a3, uint32_t a4, uint32_t a5, uint32_t a6);	/* $03CCEC regs(&d0,&d1,&d2,&d3,&d4,&d5,&d6,&d7,a1,a3,a4,a5,a6) clobbers(a0,a2,a3,a4,a5,a6) */
void Stage3_RevealNextCell(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6, uint32_t *d7, uint32_t a1, uint32_t a2, uint32_t a3, uint32_t *a4, uint32_t a5, uint32_t *a6);	/* $03CD9C regs(&d0,&d1,&d2,&d3,&d4,&d5,&d6,&d7,a1,a2,a3,&a4,a5,&a6) clobbers(a0) */
void Stage3_RevealDrawTile(uint16_t *d0, uint16_t *d2, uint16_t *d4, uint16_t *d5, uint16_t *d6, uint32_t *a0, uint32_t *a1, const RevealPiece *piece, dk::Flags *ccr);	/* $03CF72 regs(&d0,&d2,&d4,&d5,&d6,&a0,&a1,a6,&ccr) ccout(NZVC) */
void Stage3_InitLine3Cycle(uint16_t *d0, uint16_t *d1, uint16_t *d2, uint16_t *d3, uint16_t *d4, uint16_t *d5);	/* $03CFB4 regs(&d0,&d1,&d2,&d3,&d4,&d5) */
void Stage3_StepLine3Cycle(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint16_t *d6, uint32_t *d7);	/* $03CFD4 regs(&d0,&d1,&d2,&d3,&d4,&d5,&d6,&d7) clobbers(a0) */
void Stage3_RunRoadEndSwirl(uint16_t *d0, uint32_t *a0, uint32_t *a2, uint32_t *a3, uint32_t *a4, uint32_t *a5, uint32_t *a6);	/* $03DBA2 regs(&d0,&a0,&a2,&a3,&a4,&a5,&a6) */
void Stage3_SpawnRoadEndSwirl(uint16_t d4, uint16_t d5, uint32_t *a3, uint32_t *a4, uint32_t *a6);	/* $03DBB2 regs(d4,d5,&a3,&a4,&a6) clobbers(d0,d1,d7,a0,a2,a5) */
void Stage3_DrawRoad(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6, uint32_t *d7, uint32_t *a0, uint32_t *a1, uint32_t *a2, uint32_t *a3, uint32_t *a4, uint32_t *a5, uint32_t *a6, dk::Flags *ccr);	/* $0406CC regs(&d0,&d1,&d2,&d3,&d4,&d5,&d6,&d7,&a0,&a1,&a2,&a3,&a4,&a5,&a6,&ccr) ccout(NZVC) */
void Stage3_DrawRoadNextRow(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6, uint32_t *d7, uint32_t *a0, uint32_t *a1, uint32_t *a2, uint32_t a3, uint32_t *a4, uint32_t *a5, uint32_t *a6, dk::Flags *ccr);	/* $040830 regs(&d0,&d1,&d2,&d3,&d4,&d5,&d6,&d7,&a0,&a1,&a2,a3,&a4,&a5,&a6,&ccr) ccout(NZVC) */
void Stage3_DrawRoadRow(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6, uint32_t *d7, uint32_t *a0, uint32_t *a1, uint32_t *a2, uint32_t a3, uint32_t *a4, uint32_t *a5, uint32_t *a6, dk::Flags *ccr);	/* $0408C8 regs(&d0,&d1,&d2,&d3,&d4,&d5,&d6,&d7,&a0,&a1,&a2,a3,&a4,&a5,&a6,&ccr) ccout(NZVC) */
void Stage3_DrawRoadDone(void);	/* $040942 regs() */
void Stage3_DrawRoadLoopRows(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6, uint32_t *d7, uint32_t *a0, uint32_t *a1, uint32_t *a2, uint32_t a3, uint32_t *a4, uint32_t *a5, uint32_t *a6, dk::Flags *ccr);	/* $040C3E regs(&d0,&d1,&d2,&d3,&d4,&d5,&d6,&d7,&a0,&a1,&a2,a3,&a4,&a5,&a6,&ccr) ccout(NZVC) */
void Stage3_DrawRoadRowLines(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6, uint32_t *d7, uint32_t *a0, uint32_t *a1, uint32_t *a2, uint32_t a3, uint32_t *a4, uint32_t *a5, uint32_t *a6, dk::Flags *ccr);	/* $040C64 regs(&d0,&d1,&d2,&d3,&d4,&d5,&d6,&d7,&a0,&a1,&a2,a3,&a4,&a5,&a6,&ccr) ccout(NZVC) */
uint32_t Stage3_RoadPlaceNear(Vm2Obj *obj);	/* $03DDE6 regs(a6)->d0 */
