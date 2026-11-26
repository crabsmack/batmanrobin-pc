/* funcs/Op4.h - prototypes of the Op4 module: one line per function, its register
 * convention in the tag (dkit genesis recomp bridge). First written from the measured
 * contracts (dkit disasm regs); edited by the C's authors from then on. */
#pragma once
void Op4_SetResume(const u16 **script, Vm4Obj *self);	/* $0053D4 regs(&a4,a6) clobbers(a0) */
void Op4_SetTimer(const u16 **script, Vm4Obj *self);	/* $005450 regs(&a4,a6) clobbers(a0) */
void Op4_BranchScreenYOutside(uint16_t *d0, uint16_t draw_y, uint32_t *handler, const u16 **script);	/* $005520 regs(&d0,d5,&a0,&a4) */
void Op4_KillOffScreen(uint16_t *d0, uint16_t *d2, uint16_t draw_x, uint16_t draw_y, uint32_t *a0, uint32_t *a1, const u16 **script, uint32_t a6, dk::Flags *ccr);	/* $00555A regs(&d0,&d2,d4,d5,&a0,&a1,&a4,a6,&ccr) ccout(NZVC) */
void Op4_Spawn(uint32_t *d0, uint32_t *d1, uint32_t d2, uint32_t d3, uint32_t d4, uint32_t d5, uint32_t d6, uint32_t d7, uint32_t *a0, uint32_t *a1, uint32_t a2, uint32_t *a3, uint32_t *a4, uint32_t *a5, uint32_t a6);	/* $005606 regs(&d0,&d1,d2,d3,d4,d5,d6,d7,&a0,&a1,a2,&a3,&a4,&a5,a6) ccout(NZVC) */
void Op4_SetDamage(uint32_t *handler, const u16 **script, Vm4Obj *self);	/* $0056E6 regs(&a0,&a4,a6) */
void Op4_JitterSpawnHeading(uint32_t *d0, uint32_t *d3, const u16 **script);	/* $005722 regs(&d0,&d3,&a4) clobbers(a0) */
void Op4_OffsetSpawnXY(uint32_t *d4, uint32_t *d5, const u16 **script);	/* $005750 regs(&d4,&d5,&a4) clobbers(a0) */
void Op4_OffsetSpawnHeading(uint32_t *d3, uint32_t *handler, const u16 **script, dk::Flags *ccr);	/* $00576A regs(&d3,&a0,&a4,&ccr) ccout(NZVC) */
void Op4_AddVelY(uint32_t *d0, const u16 **script, Vm4Obj *self);	/* $00578C regs(&d0,&a4,a6) clobbers(a0) */
void Op4_SaveDrawPos(uint32_t *d0, uint32_t *d1, uint32_t *d6, uint32_t *d7, const u16 **script, const Vm4Obj *child, Vm4Obj *self);	/* $0057C0 regs(&d0,&d1,&d6,&d7,&a4,a5,a6) clobbers(a0) */
void Op4_AddY(uint16_t *d0, uint32_t *handler, const u16 **script, Vm4Obj *self);	/* $00583C regs(&d0,&a0,&a4,a6) */
void Op4_AddAccelY(uint32_t *d0, const u16 **script, Vm4Obj *self);	/* $0058A0 regs(&d0,&a4,a6) clobbers(a0) */
void Op4_ChildAddX(const u16 **script, Vm4Obj *child);	/* $0058CA regs(&a4,a5) clobbers(d0,a0) */
void Op4_ChildAddXY(uint16_t *d0, const u16 **script, Vm4Obj *child);	/* $0058D4 regs(&d0,&a4,a5) clobbers(a0) */
void Op4_ChildAddY(uint16_t *d0, const u16 **script, Vm4Obj *child);	/* $0058DA regs(&d0,&a4,a5) clobbers(a0) */
void Op4_ChildAddHeading(uint16_t *d0, uint32_t *handler, const u16 **script, Vm4Obj *child, dk::Flags *ccr);	/* $0058EE regs(&d0,&a0,&a4,a5,&ccr) ccout(NZVC) */
void Op4_JitterX(const u16 **script, Vm4Obj *self);	/* $005968 regs(&a4,a6) clobbers(d0,a0) */
void Op4_JitterXY(const u16 **script, Vm4Obj *self);	/* $005990 regs(&a4,a6) clobbers(d0,a0) */
void Op4_JitterHeading(uint32_t *d0, const u16 **script, Vm4Obj *self);	/* $0059EA regs(&d0,&a4,a6) clobbers(a0) */
void Op4_JitterVelX(uint32_t *d0, const u16 **script, Vm4Obj *self);	/* $005A12 regs(&d0,&a4,a6) clobbers(a0) */
void Op4_RangeVelX(const u16 **script, Vm4Obj *self);	/* $005A54 regs(&a4,a6) clobbers(d0,d1,a0) */
void Op4_ScatterVelY(uint32_t *d0, uint32_t *d1, const u16 **script, Vm4Obj *self);	/* $005A80 regs(&d0,&d1,&a4,a6) clobbers(a0) */
void Op4_RandVelRadius(const u16 **script, Vm4Obj *self);	/* $005AFA regs(&a4,a6) clobbers(d0,a0) */
void Op4_JitterAccelX(uint32_t *d0, const u16 **script, Vm4Obj *self);	/* $005B82 regs(&d0,&a4,a6) clobbers(a0) */
void Op4_RangeAccelX(const u16 **script, Vm4Obj *self);	/* $005BC4 regs(&a4,a6) clobbers(d0,d1,a0) */
void Op4_RangeAccelY(uint32_t *d1, const u16 **script, Vm4Obj *self);	/* $005C20 regs(&d1,&a4,a6) clobbers(d0,a0) */
void Op4_RangeAccelRadius(uint32_t *d0, uint32_t *d1, const u16 **script, Vm4Obj *self);	/* $005C7C regs(&d0,&d1,&a4,a6) clobbers(a0) */
void Op4_ChildJitterX(uint32_t *d0, uint32_t *handler, const u16 **script, Vm4Obj *child, dk::Flags *ccr);	/* $005CF2 regs(&d0,&a0,&a4,a5,&ccr) ccout(NZVC) */
void Op4_ChildJitterXY(const u16 **script, Vm4Obj *child);	/* $005D1A regs(&a4,a5) clobbers(d0,a0) */
void Op4_ChildJitterHeading(uint32_t *d0, uint32_t *handler, const u16 **script, Vm4Obj *child, dk::Flags *ccr);	/* $005D74 regs(&d0,&a0,&a4,a5,&ccr) ccout(NZVC) */
void Op4_ChildRangeVelX(uint32_t *d0, uint32_t *d1, uint32_t *handler, const u16 **script, Vm4Obj *child, dk::Flags *ccr);	/* $005DDE regs(&d0,&d1,&a0,&a4,a5,&ccr) ccout(NZVC) */
void Op4_ChildRandVelY(uint32_t *d0, uint32_t *handler, const u16 **script, Vm4Obj *child, dk::Flags *ccr);	/* $005E28 regs(&d0,&a0,&a4,a5,&ccr) ccout(NZVC) */
void Op4_ChildScatterVelRadius(uint32_t *d0, uint32_t *d1, uint32_t *handler, const u16 **script, Vm4Obj *child, dk::Flags *ccr);	/* $005E66 regs(&d0,&d1,&a0,&a4,a5,&ccr) ccout(NZVC) */
void Op4_ChildJitterAccelX(uint32_t *d0, uint32_t *handler, const u16 **script, Vm4Obj *child, dk::Flags *ccr);	/* $005F0C regs(&d0,&a0,&a4,a5,&ccr) ccout(NZVC) */
void Op4_ChildRangeAccelX(uint32_t *d0, uint32_t *d1, uint32_t *handler, const u16 **script, Vm4Obj *child, dk::Flags *ccr);	/* $005F4E regs(&d0,&d1,&a0,&a4,a5,&ccr) ccout(NZVC) */
void Op4_ChildRandAccelY(const u16 **script, Vm4Obj *child);	/* $005F98 regs(&a4,a5) clobbers(d0,a0) */
void Op4_ChildRangeAccelY(uint32_t *d1, const u16 **script, Vm4Obj *child);	/* $005FAA regs(&d1,&a4,a5) clobbers(d0,a0) */
void Op4_ForkOrBranch(uint32_t *d0, uint32_t d2, uint32_t d3, uint32_t d4, uint32_t d5, uint32_t *a0, uint32_t *a1, uint32_t *a3, const u16 **script, uint32_t *a5, const Vm4Obj *self, dk::Flags *ccr);	/* $00607C regs(&d0,d2,d3,d4,d5,&a0,&a1,&a3,&a4,&a5,a6,&ccr) ccout(NZVC) */
void Op4_Fork(uint32_t *d0, uint32_t d2, uint32_t d3, uint32_t d4, uint32_t d5, uint32_t *a0, uint32_t *a1, uint32_t *a3, const u16 **script, uint32_t *a5, const Vm4Obj *self, dk::Flags *ccr);	/* $00609E regs(&d0,d2,d3,d4,d5,&a0,&a1,&a3,&a4,&a5,a6,&ccr) ccout(NZVC) */
void Op4_ForkRestAndEnd(uint32_t *d0, uint32_t d2, uint32_t d3, uint32_t d4, uint32_t d5, uint32_t *a0, uint32_t *a1, uint32_t *a3, uint32_t a4, uint32_t *a5, uint32_t a6);	/* $0060C2 regs(&d0,d2,d3,d4,d5,&a0,&a1,&a3,a4,&a5,a6) ccout(NZVC) */
void Op4_SpawnAtDrawPos(uint32_t *d0, uint32_t *d1, uint32_t *d4, uint32_t *d5, const u16 **script, const Vm4Obj *self);	/* $006238 regs(&d0,&d1,&d4,&d5,&a4,a6) clobbers(a0) */
void Op4_SpawnHeadingFromSelf(uint32_t *d2, uint32_t *d3, const u16 **script, const Vm4Obj *self);	/* $0063B6 regs(&d2,&d3,&a4,a6) clobbers(a0) */
void Op4_BounceYRandom(uint32_t *d0, uint32_t *d1, const u16 **script, Vm4Obj *self);	/* $0063E2 regs(&d0,&d1,&a4,a6) clobbers(a0) */
void Op4_BranchScriptTimerZero(uint32_t *handler, const u16 **script);	/* $00646E regs(&a0,&a4) */
void Op4_AltTerrainMoveOrFall(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6, uint32_t *d7, uint32_t *a0, uint32_t *a1, uint32_t *a2, uint32_t *a3, uint32_t *a4, uint32_t *a5, uint32_t *a6);	/* $006576 regs(&d0,&d1,&d2,&d3,&d4,&d5,&d6,&d7,&a0,&a1,&a2,&a3,&a4,&a5,&a6) */
void Op4_BranchTouchPlayer(uint32_t *d0, uint16_t *d2, uint16_t *d3, uint16_t *d4, uint16_t *d5, uint16_t *d6, uint16_t *d7, uint32_t *a0, uint32_t *a2, uint32_t *a4);	/* $0065B4 regs(&d0,&d2,&d3,&d4,&d5,&d6,&d7,&a0,&a2,&a4) */
void Op4_TakeHitPhaseA(const u16 **script, Vm4Obj *self);	/* $006690 regs(&a4,a6) clobbers(a0) */
void Op4_BodyBox(uint32_t *d0, uint16_t *d1, uint16_t *d2, uint16_t *d3, uint16_t d4, uint16_t d5, uint32_t *a0, uint32_t a1, const u16 **script, Vm4Obj *self, dk::Flags *ccr);	/* $0066E2 regs(&d0,&d1,&d2,&d3,d4,d5,&a0,a1,&a4,a6,&ccr) ccout(NZVC) */
void Op4_DrawKillSidesBelowOrClear(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6, uint32_t *d7, uint32_t *a0, uint32_t *a1, uint32_t *a2, uint32_t *a3, uint32_t *a4, uint32_t a6);	/* $006776 regs(&d0,&d1,&d2,&d3,&d4,&d5,&d6,&d7,&a0,&a1,&a2,&a3,&a4,a6) ccout(NZVC) */
void Op4_SetShotFrames(const u16 **script, Vm4Obj *self);	/* $006952 regs(&a4,a6) clobbers(a0) */
void Op4_SkipWord(const u16 **script);	/* $0053BE regs(&a4) clobbers(a0) */
void Op4_Native(uint32_t code);	/* $0053C4 regs(a4) */
void Op4_SetResumeHere(const u16 **script, Vm4Obj *self);	/* $0053E0 regs(&a4,a6) clobbers(a0) */
void Op4_YieldGotoAbs(const u16 **script, Vm4Obj *self, dk::Flags *ccr);	/* $0053E8 regs(&a4,a6,&ccr) ccout(NZVC) */
void Op4_YieldGoto(const u16 **script, Vm4Obj *self, dk::Flags *ccr);	/* $0053EE regs(&a4,a6,&ccr) ccout(NZVC) */
void Op4_Return(void);	/* $0053F4 regs() */
void Op4_Gosub(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6, uint32_t *d7, uint32_t *a0, uint32_t *a1, uint32_t *a2, uint32_t *a3, uint32_t *a4, uint32_t *a5, uint32_t *a6);	/* $0053F6 regs(&d0,&d1,&d2,&d3,&d4,&d5,&d6,&d7,&a0,&a1,&a2,&a3,&a4,&a5,&a6) ccout(NZVC) */
void Op4_Kill(uint16_t *d0, uint16_t *d2, uint32_t *a0, uint32_t *a1, Vm4Obj *self);	/* $005406 regs(&d0,&d2,&a0,&a1,a6) ccout(NZVC) */
void Op4_GotoAbs(const u16 **script);	/* $005422 regs(&a4) clobbers(a0) */
void Op4_RandomSwitch(uint32_t *d0, uint32_t *handler, const u16 **script);	/* $005428 regs(&d0,&a0,&a4) */
void Op4_Goto(uint32_t *handler, const u16 **script);	/* $005432 regs(&a0,&a4) */
void Op4_CallNative(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6, uint32_t *d7, uint32_t *a0, uint32_t *a1, uint32_t *a2, uint32_t *a3, uint32_t *a4, uint32_t *a5, uint32_t *a6);	/* $005438 regs(&d0,&d1,&d2,&d3,&d4,&d5,&d6,&d7,&a0,&a1,&a2,&a3,&a4,&a5,&a6) ccout(NZVC) */
void Op4_SetTimerRandom(uint32_t *d0, const u16 **script, Vm4Obj *self);	/* $005458 regs(&d0,&a4,a6) clobbers(a0) */
void Op4_ChildSetTimer(const u16 **script, Vm4Obj *child);	/* $005468 regs(&a4,a5) clobbers(a0) */
void Op4_ChildSetTimerRandom(uint32_t *d0, uint32_t *handler, const u16 **script, Vm4Obj *child, dk::Flags *ccr);	/* $005470 regs(&d0,&a0,&a4,a5,&ccr) ccout(NZVC) */
void Op4_TimerBranchOut(uint32_t *handler, const u16 **script, Vm4Obj *self);	/* $005480 regs(&a0,&a4,a6) */
void Op4_TimerLoop(const u16 **script, Vm4Obj *self);	/* $00548C regs(&a4,a6) clobbers(a0) */
void Op4_SetCounterTicker(uint16_t *d0, const u16 **script, Vm4Obj *self);	/* $00549A regs(&d0,&a4,a6) clobbers(a0) */
void Op4_SetTicker(const u16 **script, Vm4Obj *self);	/* $0054A8 regs(&a4,a6) clobbers(a0) */
void Op4_CounterKillOut(uint16_t *d0, uint16_t *d2, uint32_t *a0, uint32_t *a1, const u16 **script, Vm4Obj *self);	/* $0054B0 regs(&d0,&d2,&a0,&a1,&a4,a6) ccout(NZVC) */
void Op4_CounterBranchOut(uint32_t *handler, const u16 **script, Vm4Obj *self);	/* $0054BC regs(&a0,&a4,a6) */
void Op4_WaitTicker(uint8_t *d0, uint32_t *handler, const u16 **script, Vm4Obj *self, dk::Flags *ccr);	/* $0054CA regs(&d0,&a0,&a4,a6,&ccr) ccout(NZVC) */
void Op4_TickerLoop(uint8_t *d0, uint32_t *handler, const u16 **script, Vm4Obj *self);	/* $0054DE regs(&d0,&a0,&a4,a6) */
void Op4_ReplaceSelf(uint32_t *d0, uint32_t *d1, uint32_t d2, uint32_t d3, uint32_t d4, uint32_t d5, uint32_t *a0, uint32_t a1, uint32_t *a3, uint32_t *a4, uint32_t *a5, uint32_t a6);	/* $00557A regs(&d0,&d1,d2,d3,d4,d5,&a0,a1,&a3,&a4,&a5,a6) ccout(NZVC) */
void Op4_SpawnRunChild(uint32_t *d0, uint32_t *d1, uint32_t d2, uint32_t d3, uint32_t d4, uint32_t d5, uint32_t d6, uint32_t d7, uint32_t *a0, uint32_t *a1, uint32_t a2, uint32_t *a3, uint32_t *a4, uint32_t *a5, uint32_t *a6);	/* $0055CA regs(&d0,&d1,d2,d3,d4,d5,d6,d7,&a0,&a1,a2,&a3,&a4,&a5,&a6) ccout(NZVC) */
void Op4_SpawnAddVel(uint32_t *d0, uint32_t *d1, uint32_t d2, uint32_t d3, uint32_t d4, uint32_t d5, uint32_t d6, uint32_t d7, uint32_t *a0, uint32_t *a1, uint32_t a2, uint32_t *a3, uint32_t *a4, uint32_t *a5, uint32_t a6, dk::Flags *ccr);	/* $00560E regs(&d0,&d1,d2,d3,d4,d5,d6,d7,&a0,&a1,a2,&a3,&a4,&a5,a6,&ccr) ccout(NZVC) */
void Op4_AddMotionX(uint32_t *d0, const u16 **script, Vm4Obj *self);	/* $005636 regs(&d0,&a4,a6) clobbers(a0) */
void Op4_AddMotionY(uint32_t *d0, const u16 **script, Vm4Obj *self);	/* $00564C regs(&d0,&a4,a6) clobbers(a0) */
void Op4_AddMotionRadius(uint32_t *d0, const u16 **script, Vm4Obj *self);	/* $005662 regs(&d0,&a4,a6) clobbers(a0) */
void Op4_AddMotionHeading(uint32_t *d0, const u16 **script, Vm4Obj *self);	/* $005678 regs(&d0,&a4,a6) clobbers(a0) */
void Op4_ChildAddMotionX(uint32_t *d0, uint32_t *handler, const u16 **script, Vm4Obj *child, dk::Flags *ccr);	/* $00568E regs(&d0,&a0,&a4,a5,&ccr) ccout(NZVC) */
void Op4_ChildAddMotionY(uint32_t *d0, uint32_t *handler, const u16 **script, Vm4Obj *child, dk::Flags *ccr);	/* $0056A4 regs(&d0,&a0,&a4,a5,&ccr) ccout(NZVC) */
void Op4_ChildAddMotionRadius(uint32_t *d0, uint32_t *handler, const u16 **script, Vm4Obj *child, dk::Flags *ccr);	/* $0056BA regs(&d0,&a0,&a4,a5,&ccr) ccout(NZVC) */
void Op4_ChildSetDamage(uint32_t *handler, const u16 **script, Vm4Obj *child, dk::Flags *ccr);	/* $0056EE regs(&a0,&a4,a5,&ccr) ccout(NZVC) */
void Op4_JitterSpawnX(uint32_t *d4, const u16 **script);	/* $0056F6 regs(&d4,&a4) clobbers(d0,a0) */
void Op4_JitterSpawnXY(uint32_t *d4, uint32_t *d5, const u16 **script);	/* $005702 regs(&d4,&d5,&a4) clobbers(d0,a0) */
void Op4_JitterDrawXY(uint32_t *d0, uint32_t *d4, uint32_t *d5, const u16 **script);	/* $00572E regs(&d0,&d4,&d5,&a4) clobbers(a0) */
void Op4_OffsetSpawnY(uint32_t *d5, const u16 **script);	/* $005756 regs(&d5,&a4) clobbers(a0) */
void Op4_AddVelRadius(uint32_t *d0, const u16 **script, Vm4Obj *self);	/* $00579A regs(&d0,&a4,a6) clobbers(a0) */
void Op4_ChildSaveDrawPos(uint32_t *d0, uint32_t *d1, uint32_t *d6, uint32_t *d7, uint32_t *handler, const u16 **script, Vm4Obj *child, dk::Flags *ccr);	/* $0057F6 regs(&d0,&d1,&d6,&d7,&a0,&a4,a5,&ccr) ccout(NZVC) */
void Op4_AddRadius(uint16_t *d0, const u16 **script, Vm4Obj *self);	/* $005846 regs(&d0,&a4,a6) clobbers(a0) */
void Op4_AddHeading(const u16 **script, Vm4Obj *self);	/* $005850 regs(&a4,a6) clobbers(d0,a0) */
void Op4_AddVelX(uint32_t *d0, const u16 **script, Vm4Obj *self);	/* $00585A regs(&d0,&a4,a6) clobbers(a0) */
void Op4_AddVelYAlt(uint32_t *d0, const u16 **script, Vm4Obj *self);	/* $005868 regs(&d0,&a4,a6) clobbers(a0) */
void Op4_AddVelRadiusAlt(uint32_t *d0, const u16 **script, Vm4Obj *self);	/* $005876 regs(&d0,&a4,a6) clobbers(a0) */
void Op4_ChildAddVelX(uint32_t *d0, const u16 **script, Vm4Obj *child);	/* $0058F8 regs(&d0,&a4,a5) clobbers(a0) */
void Op4_ChildAddVelY(uint32_t *d0, uint32_t *handler, const u16 **script, Vm4Obj *child, dk::Flags *ccr);	/* $005906 regs(&d0,&a0,&a4,a5,&ccr) ccout(NZVC) */
void Op4_ChildAddVelRadius(uint32_t *d0, const u16 **script, Vm4Obj *child);	/* $005914 regs(&d0,&a4,a5) clobbers(a0) */
void Op4_ChildAddVelHeading(uint32_t *d0, const u16 **script, Vm4Obj *child);	/* $005922 regs(&d0,&a4,a5) clobbers(a0) */
void Op4_ChildAddAccelX(uint32_t *d0, const u16 **script, Vm4Obj *child);	/* $005930 regs(&d0,&a4,a5) clobbers(a0) */
void Op4_ChildAddAccelY(uint32_t *d0, uint32_t *handler, const u16 **script, Vm4Obj *child, dk::Flags *ccr);	/* $00593E regs(&d0,&a0,&a4,a5,&ccr) ccout(NZVC) */
void Op4_ChildAddAccelRadius(uint32_t *d0, uint32_t *handler, const u16 **script, Vm4Obj *child, dk::Flags *ccr);	/* $00594C regs(&d0,&a0,&a4,a5,&ccr) ccout(NZVC) */
void Op4_JitterY(const u16 **script, Vm4Obj *self);	/* $00599A regs(&a4,a6) clobbers(d0,a0) */
void Op4_ScatterVelX(uint32_t *d0, uint32_t *d1, const u16 **script, Vm4Obj *self);	/* $005A24 regs(&d0,&d1,&a4,a6) clobbers(a0) */
void Op4_RandVelY(const u16 **script, Vm4Obj *self);	/* $005A9E regs(&a4,a6) clobbers(d0,a0) */
void Op4_RangeVelY(uint32_t *d0, uint32_t *d1, const u16 **script, Vm4Obj *self);	/* $005AB0 regs(&d0,&d1,&a4,a6) clobbers(a0) */
void Op4_JitterVelRadius(uint32_t *d0, const u16 **script, Vm4Obj *self);	/* $005ACA regs(&d0,&a4,a6) clobbers(a0) */
void Op4_RangeVelRadius(uint32_t *d0, uint32_t *d1, const u16 **script, Vm4Obj *self);	/* $005B0C regs(&d0,&d1,&a4,a6) clobbers(a0) */
void Op4_JitterAccelY(uint32_t *d0, uint32_t *handler, const u16 **script, Vm4Obj *self);	/* $005BDE regs(&d0,&a0,&a4,a6) */
void Op4_ChildJitterY(const u16 **script, Vm4Obj *child);	/* $005D24 regs(&a4,a5) clobbers(d0,a0) */
void Op4_ChildScatterHeading(const u16 **script, Vm4Obj *child);	/* $005D82 regs(&a4,a5) clobbers(d0,d1,a0) */
void Op4_ChildJitterVelX(uint32_t *d0, uint32_t *handler, const u16 **script, Vm4Obj *child, dk::Flags *ccr);	/* $005D9C regs(&d0,&a0,&a4,a5,&ccr) ccout(NZVC) */
void Op4_ChildScatterVelX(uint32_t *d0, uint32_t *d1, uint32_t *handler, const u16 **script, Vm4Obj *child, dk::Flags *ccr);	/* $005DAE regs(&d0,&d1,&a0,&a4,a5,&ccr) ccout(NZVC) */
void Op4_ChildRangeVelY(uint32_t *d0, uint32_t *d1, uint32_t *handler, const u16 **script, Vm4Obj *child, dk::Flags *ccr);	/* $005E3A regs(&d0,&d1,&a0,&a4,a5,&ccr) ccout(NZVC) */
void Op4_ChildScatterAccelX(uint32_t *d0, uint32_t *d1, uint32_t *handler, const u16 **script, Vm4Obj *child, dk::Flags *ccr);	/* $005F1E regs(&d0,&d1,&a0,&a4,a5,&ccr) ccout(NZVC) */
void Op4_ChildJitterAccelY(uint32_t *d0, uint32_t *handler, const u16 **script, Vm4Obj *child, dk::Flags *ccr);	/* $005F68 regs(&d0,&a0,&a4,a5,&ccr) ccout(NZVC) */
void Op4_ForkAbs(uint32_t *d0, uint32_t d2, uint32_t d3, uint32_t d4, uint32_t d5, uint32_t *a0, uint32_t *a1, uint32_t *a3, const u16 **script, uint32_t *a5, const Vm4Obj *self, dk::Flags *ccr);	/* $0060AC regs(&d0,d2,d3,d4,d5,&a0,&a1,&a3,&a4,&a5,a6,&ccr) ccout(NZVC) */
void Op4_ForkAbsAndEnd(uint32_t *d0, uint32_t d2, uint32_t d3, uint32_t d4, uint32_t d5, uint32_t *a0, uint32_t *a1, uint32_t *a3, const u16 **script, uint32_t *a5, uint32_t a6);	/* $0060B8 regs(&d0,d2,d3,d4,d5,&a0,&a1,&a3,&a4,&a5,a6) ccout(NZVC) */
void Op4_ChildAddOwnVel(uint32_t *d0, const u16 **script, Vm4Obj *child, const Vm4Obj *self);	/* $006116 regs(&d0,&a4,a5,a6) clobbers(a0) */
void Op4_SetXYFromDraw(uint16_t d4, uint16_t d5, const u16 **script, Vm4Obj *self);	/* $0061FC regs(d4,d5,&a4,a6) clobbers(a0) */
void Op4_CalcDrawPos(uint32_t *d0, uint32_t *d1, uint32_t *d4, uint32_t *d5, const u16 **script, const Vm4Obj *self);	/* $006240 regs(&d0,&d1,&d4,&d5,&a4,a6) clobbers(a0) */
void Op4_BounceY(uint32_t *d0, const u16 **script, Vm4Obj *self);	/* $006276 regs(&d0,&a4,a6) clobbers(a0) */
void Op4_Move(uint32_t *d0, uint32_t *d1, uint32_t *d4, uint32_t *d5, uint32_t *d6, uint32_t *d7, uint32_t *a0, const u16 **script, Vm4Obj *self);	/* $006286 regs(&d0,&d1,&d4,&d5,&d6,&d7,&a0,&a4,a6) */
void Op4_MovePolar(uint32_t *d0, uint32_t *d4, uint32_t *d5, const u16 **script, Vm4Obj *self);	/* $0062E2 regs(&d0,&d4,&d5,&a4,a6) clobbers(a0) */
void Op4_MoveXY(uint32_t *d0, uint16_t *d4, uint16_t *d5, uint32_t *a0, const u16 **script, Vm4Obj *self);	/* $00632E regs(&d0,&d4,&d5,&a0,&a4,a6) */
void Op4_LoadXY(uint16_t *d4, uint16_t *d5, uint32_t *a0, const u16 **script, const Vm4Obj *self);	/* $00634E regs(&d4,&d5,&a0,&a4,a6) */
void Op4_PolarToXY(uint32_t *d0, uint32_t *d1, uint32_t *a0, const u16 **script, Vm4Obj *self);	/* $00635A regs(&d0,&d1,&a0,&a4,a6) */
void Op4_SpawnFromSelf(uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5, const u16 **script, const Vm4Obj *self);	/* $0063A2 regs(&d2,&d3,&d4,&d5,&a4,a6) clobbers(a0) */
void Op4_SpawnXYFromSelf(uint32_t *d4, uint32_t *d5, const u16 **script, const Vm4Obj *self);	/* $0063C0 regs(&d4,&d5,&a4,a6) clobbers(a0) */
void Op4_ClearSpawnPolar(uint32_t *d2, uint32_t *d3, const u16 **script);	/* $0063CA regs(&d2,&d3,&a4) clobbers(a0) */
void Op4_SetAnimRateRandom(uint32_t *d0, const u16 **script, Vm4Obj *self);	/* $006402 regs(&d0,&a4,a6) clobbers(a0) */
void Op4_SetAnim(const u16 **script, Vm4Obj *self);	/* $006418 regs(&a4,a6) clobbers(a0) */
void Op4_SetFrame(const u16 **script, Vm4Obj *self);	/* $006428 regs(&a4,a6) clobbers(a0) */
void Op4_ChildSetAnim(uint32_t *handler, const u16 **script, Vm4Obj *child, dk::Flags *ccr);	/* $006438 regs(&a0,&a4,a5,&ccr) ccout(NZVC) */
void Op4_ChildSetFrame(uint32_t *handler, const u16 **script, Vm4Obj *child, dk::Flags *ccr);	/* $006444 regs(&a0,&a4,a5,&ccr) ccout(NZVC) */
void Op4_SetDepth(uint32_t *handler, const u16 **script, Vm4Obj *self);	/* $006450 regs(&a0,&a4,a6) */
void Op4_ChildSetDepth(const u16 **script, Vm4Obj *child);	/* $006458 regs(&a4,a5) clobbers(a0) */
void Op4_AddScriptTimer(uint16_t *d0, uint32_t *handler, const u16 **script, dk::Flags *ccr);	/* $00647C regs(&d0,&a0,&a4,&ccr) ccout(NZVC) */
void Op4_PlaySound(uint32_t *d0, const u16 **script);	/* $006486 regs(&d0,&a4) clobbers(a0) */
void Op4_FlashRandom(uint32_t *d0, uint32_t *handler, const u16 **script, dk::Flags *ccr);	/* $006492 regs(&d0,&a0,&a4,&ccr) ccout(NZVC) */
void Op4_ShakeRandom(uint32_t *d0, uint32_t *handler, const u16 **script);	/* $0064A8 regs(&d0,&a0,&a4) ccout(NZVC) */
void Op4_Shake(uint32_t *handler, const u16 **script);	/* $0064B0 regs(&a0,&a4) */
void Op4_BranchCrossY(uint16_t *d0, uint16_t d4, uint16_t *d5, const u16 **script, Vm4Obj *self);	/* $0064BA regs(&d0,d4,&d5,&a4,a6) clobbers(a0) */
void Op4_TerrainMove(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6, uint32_t *d7, uint32_t *a0, uint32_t *a1, uint32_t *a2, uint32_t *a3, uint32_t *a4, uint32_t *a5, uint32_t *a6);	/* $0064E4 regs(&d0,&d1,&d2,&d3,&d4,&d5,&d6,&d7,&a0,&a1,&a2,&a3,&a4,&a5,&a6) */
void Op4_TerrainMoveDefault(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6, uint32_t *d7, uint32_t *a0, uint32_t *a1, uint32_t *a2, uint32_t *a3, uint32_t *a4, uint32_t *a5, uint32_t *a6);	/* $006518 regs(&d0,&d1,&d2,&d3,&d4,&d5,&d6,&d7,&a0,&a1,&a2,&a3,&a4,&a5,&a6) */
void Op4_HitPlayersOnPhase(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6, uint32_t *d7, uint32_t *a0, uint32_t *a1, uint32_t *a2, uint32_t *a3, uint32_t *a4, uint32_t a5, uint32_t *a6);	/* $0065D6 regs(&d0,&d1,&d2,&d3,&d4,&d5,&d6,&d7,&a0,&a1,&a2,&a3,&a4,a5,&a6) */
void Op4_HitPlayers(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6, uint32_t *d7, uint32_t *a0, uint32_t *a1, uint32_t *a2, uint32_t *a3, uint32_t *a4, uint32_t a5, uint32_t *a6);	/* $0065E8 regs(&d0,&d1,&d2,&d3,&d4,&d5,&d6,&d7,&a0,&a1,&a2,&a3,&a4,a5,&a6) */
void Op4_BranchTouchEnemyOnPhase(uint32_t *d0, uint16_t *d2, uint16_t *d3, uint16_t *d4, uint16_t *d5, uint16_t *d6, uint16_t *d7, uint32_t *a0, uint32_t *a2, uint32_t *a4, const Vm4Obj *self);	/* $00661C regs(&d0,&d2,&d3,&d4,&d5,&d6,&d7,&a0,&a2,&a4,a6) */
void Op4_TakeHitPhaseB(const u16 **script, Vm4Obj *self);	/* $00669E regs(&a4,a6) clobbers(a0) */
void Op4_SwitchHeading(uint32_t *d0, const u16 **script, const Vm4Obj *self);	/* $006744 regs(&d0,&a4,a6) clobbers(a0) */
void Op4_BranchScreenClear(uint32_t *handler, const u16 **script);	/* $00675E regs(&a0,&a4) */
void Op4_DrawKillSidesBelow(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6, uint32_t *d7, uint32_t *a0, uint32_t *a1, uint32_t *a2, uint32_t *a3, uint32_t *a4, uint32_t a6);	/* $00677E regs(&d0,&d1,&d2,&d3,&d4,&d5,&d6,&d7,&a0,&a1,&a2,&a3,&a4,a6) ccout(NZVC) */
void Op4_DrawKillOutsideOrClear(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6, uint32_t *d7, uint32_t *a0, uint32_t *a1, uint32_t *a2, uint32_t *a3, uint32_t *a4, uint32_t a6);	/* $00679C regs(&d0,&d1,&d2,&d3,&d4,&d5,&d6,&d7,&a0,&a1,&a2,&a3,&a4,a6) ccout(NZVC) */
void Op4_DrawKillOutside(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6, uint32_t *d7, uint32_t *a0, uint32_t *a1, uint32_t *a2, uint32_t *a3, uint32_t *a4, uint32_t a6);	/* $0067A4 regs(&d0,&d1,&d2,&d3,&d4,&d5,&d6,&d7,&a0,&a1,&a2,&a3,&a4,a6) ccout(NZVC) */
void Op4_DrawKillOffScreenOrClear(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6, uint32_t *d7, uint32_t *a0, uint32_t *a1, uint32_t *a2, uint32_t *a3, uint32_t *a4, uint32_t a6);	/* $0067CA regs(&d0,&d1,&d2,&d3,&d4,&d5,&d6,&d7,&a0,&a1,&a2,&a3,&a4,a6) ccout(NZVC) */
void Op4_DrawKillOffScreen(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6, uint32_t *d7, uint32_t *a0, uint32_t *a1, uint32_t *a2, uint32_t *a3, uint32_t *a4, uint32_t a6);	/* $0067D2 regs(&d0,&d1,&d2,&d3,&d4,&d5,&d6,&d7,&a0,&a1,&a2,&a3,&a4,a6) ccout(NZVC) */
void Op4_Draw(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6, uint32_t *d7, uint32_t *a0, uint32_t *a1, uint32_t *a2, uint32_t *a3, uint32_t *a4, uint32_t a6);	/* $0067EE regs(&d0,&d1,&d2,&d3,&d4,&d5,&d6,&d7,&a0,&a1,&a2,&a3,&a4,a6) ccout(NZVC) */
void Op4_SetAttr(uint8_t *d0, const u16 **script, Vm4Obj *self);	/* $006800 regs(&d0,&a4,a6) clobbers(a0) */
void Op4_ChildSetAttr(uint8_t *d0, uint32_t *handler, const u16 **script, Vm4Obj *child, dk::Flags *ccr);	/* $006810 regs(&d0,&a0,&a4,a5,&ccr) ccout(NZVC) */
void Op4_AnimFrameAttr(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6, uint32_t *d7, uint32_t *a0, uint32_t *a1, uint32_t *a2, uint32_t *a3, uint32_t *a4, uint32_t a6);	/* $006820 regs(&d0,&d1,&d2,&d3,&d4,&d5,&d6,&d7,&a0,&a1,&a2,&a3,&a4,a6) ccout(NZVC) */
void Op4_AnimFrame(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6, uint32_t *d7, uint32_t *a0, uint32_t *a1, uint32_t *a2, uint32_t *a3, uint32_t *a4, uint32_t a6);	/* $00682C regs(&d0,&d1,&d2,&d3,&d4,&d5,&d6,&d7,&a0,&a1,&a2,&a3,&a4,a6) ccout(NZVC) */
void Op4_DrawFrame(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6, uint32_t *d7, uint32_t *a0, uint32_t *a1, uint32_t *a2, uint32_t *a3, uint32_t *a4, uint32_t a6);	/* $006834 regs(&d0,&d1,&d2,&d3,&d4,&d5,&d6,&d7,&a0,&a1,&a2,&a3,&a4,a6) ccout(NZVC) */
void Op4_SetVelFromHeading(uint32_t *d1, const u16 **script, Vm4Obj *self);	/* $00695E regs(&d1,&a4,a6) clobbers(d0,a0) */
void Op4_ShotStep(uint32_t *d0, uint16_t *d2, uint16_t *d3, uint16_t *d4, uint16_t *d5, uint16_t *d6, uint16_t *d7, uint32_t *a0, uint32_t *a1, uint32_t *a2, const u16 **script, Vm4Obj *self, dk::Flags *ccr);	/* $006990 regs(&d0,&d2,&d3,&d4,&d5,&d6,&d7,&a0,&a1,&a2,&a4,a6,&ccr) ccout(NZVC) */
void Op4_ShotStepAt(uint32_t *d0, uint16_t *d2, uint16_t *d3, uint16_t *d4, uint16_t *d5, uint16_t *d6, uint16_t *d7, uint32_t *a0, uint32_t *a1, uint32_t *a2, const u16 **script, Vm4Obj *self, dk::Flags *ccr);	/* $0069A8 regs(&d0,&d2,&d3,&d4,&d5,&d6,&d7,&a0,&a1,&a2,&a4,a6,&ccr) ccout(NZVC) */
/* a4-VM code entered by `jsr (a0)` and run until an op returns: an op handler (Op4_Gosub $5400, the a2
 * effect-spawn ops $51BE / $522A start a spawn script) or a native routine (Op4_CallNative $543A): every
 * register, since the script or routine may leave anything for the next op */
void Op4CodeFn(uint32_t code, uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6, uint32_t *d7, uint32_t *a0, uint32_t *a1, uint32_t *a2, uint32_t *a3, uint32_t *a4, uint32_t *a5, uint32_t *a6);	/* type regs(&d0,&d1,&d2,&d3,&d4,&d5,&d6,&d7,&a0,&a1,&a2,&a3,&a4,&a5,&a6) */
/* the scene's terrain routine as the a4 terrain ops call it (`jsr (a0)` through g_TerrainFn / g_TerrainFnAlt at $6506,
 * $652A, $658E: d1 / d2 the move, d4 / d5 the position, d6 / d7 the masks; it leaves the position and the contact in
 * d4-d7): every register, as Coll_MoveScene hands them to the same routines */
void Op4TerrainFn(uint32_t code, uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6, uint32_t *d7, uint32_t *a0, uint32_t *a1, uint32_t *a2, uint32_t *a3, uint32_t *a4, uint32_t *a5, uint32_t *a6);	/* type regs(&d0,&d1,&d2,&d3,&d4,&d5,&d6,&d7,&a0,&a1,&a2,&a3,&a4,&a5,&a6) */
/* the native hit routine of Op4_HitPlayers (`jsr (a3)` at $660A, the long operand: e.g. $106A8), called once per player
 * box overlapping the test rectangle with a2 = the box and d4-d7 = the rectangle: every register */
void Op4HitFn(uint32_t code, uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6, uint32_t *d7, uint32_t *a0, uint32_t *a1, uint32_t *a2, uint32_t *a3, uint32_t *a4, uint32_t *a5, uint32_t *a6);	/* type regs(&d0,&d1,&d2,&d3,&d4,&d5,&d6,&d7,&a0,&a1,&a2,&a3,&a4,&a5,&a6) */
void Op4_ForkAndEnd(uint32_t *d0, uint32_t d2, uint32_t d3, uint32_t d4, uint32_t d5, uint32_t *a0, uint32_t *a1, uint32_t *a3, const u16 **script, uint32_t *a5, uint32_t a6);	/* $0060BC regs(&d0,d2,d3,d4,d5,&a0,&a1,&a3,&a4,&a5,a6) ccout(NZVC) */
