/* funcs/Op5.h - prototypes of the Op5 module: one line per function, its register
 * convention in the tag (dkit genesis recomp bridge). First written from the measured
 * contracts (dkit disasm regs); edited by the C's authors from then on. */
#pragma once
void Op5_Wait(uint32_t *d0, const u16 **script, Vm5Wave *self, dk::Flags *ccr);	/* $006AC4 regs(&d0,&a5,a6,&ccr) ccout(NZVC) */
void Op5_SetLoopA0(uint32_t *d0, const u16 **script, Vm5Wave *self);	/* $006C62 regs(&d0,&a5,a6) clobbers(a0) */
void Op5_LoopA(uint32_t *handler, const u16 **script, Vm5Wave *self);	/* $006CB0 regs(&a0,&a5,a6) */
void Op5_SetLoopB2(uint32_t *d0, uint32_t *a0, uint32_t *a5, uint32_t a6);	/* $006CC6 regs(&d0,&a0,&a5,a6) */
void Op5_SetLoopB5(uint32_t *d0, uint32_t *a0, uint32_t *a5, uint32_t a6);	/* $006CD2 regs(&d0,&a0,&a5,a6) */
void Op5_SetLoopB(uint16_t *d0, uint32_t *handler, const u16 **script, Vm5Wave *self);	/* $006CE2 regs(&d0,&a0,&a5,a6) */
void Op5_LoopB(uint32_t *handler, const u16 **script, Vm5Wave *self);	/* $006D10 regs(&a0,&a5,a6) */
void Op5_SetQuota(uint32_t *handler, const u16 **script, Vm5Wave *self);	/* $006DBC regs(&a0,&a5,a6) */
void Op5_SetOrigin(const u16 **script, Vm5Wave *self);	/* $006DF4 regs(&a5,a6) clobbers(a0) */
void Op5_AddOriginX(uint16_t *d0, const u16 **script, Vm5Wave *self);	/* $006E18 regs(&d0,&a5,a6) clobbers(a0) */
void Op5_LockCameraHere(uint32_t *d0, uint32_t *a0, uint32_t *a5);	/* $006E74 regs(&d0,&a0,&a5) */
void Op5_ShowGoSign(uint32_t *d0, uint32_t *a5);	/* $006E9A regs(&d0,&a5) clobbers(a0) */
void Op5_WaitPopulation6(uint32_t *d0, uint32_t *a0, uint32_t *a5, uint32_t a6, dk::Flags *ccr);	/* $006EBA regs(&d0,&a0,&a5,a6,&ccr) ccout(NZVC) */
void Op5_End(uint16_t *d0, uint32_t *a0, uint32_t *a1, Vm5Wave *self, dk::Flags *ccr);	/* $006A72 regs(&d0,&a0,&a1,a6,&ccr) ccout(NZVC) */
void Op5_ClearInterval(const u16 **script, Vm5Wave *self);	/* $006A8A regs(&a5,a6) clobbers(a0) */
void Op5_SetInterval(uint32_t *d0, const u16 **script, Vm5Wave *self);	/* $006A96 regs(&d0,&a5,a6) clobbers(a0) */
void Op5_WaitRandom(uint32_t *d0, uint32_t *d1, const u16 **script, Vm5Wave *self, dk::Flags *ccr);	/* $006AD0 regs(&d0,&d1,&a5,a6,&ccr) ccout(NZVC) */
void Op5_SetLoopA2(uint32_t *d0, const u16 **script, Vm5Wave *self);	/* $006C66 regs(&d0,&a5,a6) clobbers(a0) */
void Op5_SetLoopA3(uint32_t *d0, const u16 **script, Vm5Wave *self);	/* $006C6A regs(&d0,&a5,a6) clobbers(a0) */
void Op5_SetLoopA4(uint32_t *d0, const u16 **script, Vm5Wave *self);	/* $006C6E regs(&d0,&a5,a6) clobbers(a0) */
void Op5_SetLoopA5(uint32_t *d0, const u16 **script, Vm5Wave *self);	/* $006C72 regs(&d0,&a5,a6) clobbers(a0) */
void Op5_SetLoopA6(uint32_t *d0, const u16 **script, Vm5Wave *self);	/* $006C76 regs(&d0,&a5,a6) clobbers(a0) */
void Op5_SetLoopA7(uint32_t *d0, const u16 **script, Vm5Wave *self);	/* $006C7A regs(&d0,&a5,a6) clobbers(a0) */
void Op5_SetLoopA8(uint32_t *d0, const u16 **script, Vm5Wave *self);	/* $006C7E regs(&d0,&a5,a6) clobbers(a0) */
void Op5_SetLoopA(uint16_t *d0, const u16 **script, Vm5Wave *self);	/* $006C82 regs(&d0,&a5,a6) clobbers(a0) */
void Op5_SetLoopB3(uint32_t *d0, uint32_t *a0, uint32_t *a5, uint32_t a6);	/* $006CCA regs(&d0,&a0,&a5,a6) */
/* the shared store of Op5_SetLoopB* (a branch target only, no measured contract: the callers' registers) */
void Op5_SetLoopB_Store(uint16_t d0, uint32_t *handler, const u16 **script, Vm5Wave *self);	/* $006CE4 regs(d0,&a0,&a5,a6) */
/* the 68000 routine an a5 script calls (Op5_CallNative: `jsr (a0)` at $6D26; $240EE, $240F6): every register in and out */
void Op5NativeFn(uint32_t code, uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6, uint32_t *d7, uint32_t *a0, uint32_t *a1, uint32_t *a2, uint32_t *a3, uint32_t *a4, const u16 **script, uint32_t *a6);	/* type regs(&d0,&d1,&d2,&d3,&d4,&d5,&d6,&d7,&a0,&a1,&a2,&a3,&a4,&a5,&a6) */
void Op5_CallNative(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6, uint32_t *d7, uint32_t *a0, uint32_t *a1, uint32_t *a2, uint32_t *a3, uint32_t *a4, const u16 **script, uint32_t *a6);	/* $006D22 regs(&d0,&d1,&d2,&d3,&d4,&d5,&d6,&d7,&a0,&a1,&a2,&a3,&a4,&a5,&a6) */
void Op5_Gosub(uint32_t *handler, const u16 **script, Vm5Wave *self);	/* $006D2A regs(&a0,&a5,a6) */
void Op5_Return(uint32_t *handler, const u16 **script, Vm5Wave *self);	/* $006D3A regs(&a0,&a5,a6) */
void Op5_RandomSwitch(uint32_t *d0, uint32_t *handler, const u16 **script);	/* $006D4A regs(&d0,&a0,&a5) */
void Op5_Goto(uint32_t *handler, const u16 **script);	/* $006D54 regs(&a0,&a5) */
void Op5_SetPopulationCap(uint32_t *handler, const u16 **script, Vm5Wave *self);	/* $006DD4 regs(&a0,&a5,a6) */
void Op5_ClearOrigin(uint32_t *handler, const u16 **script, Vm5Wave *self);	/* $006DE0 regs(&a0,&a5,a6) */
void Op5_SetOriginY(const u16 **script, Vm5Wave *self);	/* $006DF8 regs(&a5,a6) clobbers(a0) */
void Op5_SetOriginX(uint32_t *handler, const u16 **script, Vm5Wave *self);	/* $006E00 regs(&a0,&a5,a6) */
void Op5_OriginAtScreenCentre(uint16_t *d0, uint32_t *handler, const u16 **script, Vm5Wave *self);	/* $006E22 regs(&d0,&a0,&a5,a6) */
void Op5_WaitPopulation(uint16_t *d0, uint32_t *a0, uint32_t *a5, uint32_t a6, dk::Flags *ccr);	/* $006EA4 regs(&d0,&a0,&a5,a6,&ccr) ccout(NZVC) */
void Op5_WaitPopulation5(uint32_t *d0, uint32_t *a0, uint32_t *a5, uint32_t a6, dk::Flags *ccr);	/* $006EBE regs(&d0,&a0,&a5,a6,&ccr) ccout(NZVC) */
void Op5_WaitPopulation4(uint32_t *d0, uint32_t *a0, uint32_t *a5, uint32_t a6, dk::Flags *ccr);	/* $006EC2 regs(&d0,&a0,&a5,a6,&ccr) ccout(NZVC) */
void Op5_WaitPopulation3(uint32_t *d0, uint32_t *a0, uint32_t *a5, uint32_t a6, dk::Flags *ccr);	/* $006EC6 regs(&d0,&a0,&a5,a6,&ccr) ccout(NZVC) */
void Op5_WaitPopulation2(uint32_t *d0, uint32_t *a0, uint32_t *a5, uint32_t a6, dk::Flags *ccr);	/* $006ECA regs(&d0,&a0,&a5,a6,&ccr) ccout(NZVC) */
void Op5_WaitPopulation1(uint32_t *d0, uint32_t *a0, uint32_t *a5, uint32_t a6, dk::Flags *ccr);	/* $006ECE regs(&d0,&a0,&a5,a6,&ccr) ccout(NZVC) */
void Op5_WaitPopulation0(uint32_t *d0, uint32_t *a0, uint32_t *a5, uint32_t a6, dk::Flags *ccr);	/* $006ED2 regs(&d0,&a0,&a5,a6,&ccr) ccout(NZVC) */
void Op5_EndOtherWaves(uint16_t *d0, uint32_t *a0, uint32_t *a5, Vm5Wave *wave);	/* $006EEC regs(&d0,&a0,&a5,a6) */
void Op5_EndOlderWaves(uint32_t *a0, uint32_t *a5, Vm5Wave *wave);	/* $006F00 regs(&a0,&a5,a6) */
void Op5_StartGroup(uint32_t *d0, uint32_t *a0, uint32_t *a5, Vm5Wave *wave);	/* $006F0A regs(&d0,&a0,&a5,a6) */
void Op5_Spawn(uint32_t *d0, uint32_t *d1, uint32_t d2, uint32_t d3, uint32_t *d4, uint32_t *d5, uint32_t *d6, uint32_t *d7, uint32_t *a0, uint32_t *a1, uint32_t *a2, uint32_t *a3, uint32_t *a4, uint32_t *a5, uint32_t *a6);	/* $006FB2 regs(&d0,&d1,d2,d3,&d4,&d5,&d6,&d7,&a0,&a1,&a2,&a3,&a4,&a5,&a6) ccout(NZVC) */
void Op5_SpawnMulti(uint32_t *d0, uint32_t *d1, uint32_t d2, uint32_t d3, uint32_t *d4, uint32_t *d5, uint32_t *d6, uint32_t *d7, uint32_t *a0, uint32_t *a1, uint32_t *a2, uint32_t *a3, uint32_t *a4, uint32_t *a5, uint32_t *a6);	/* $006FBC regs(&d0,&d1,d2,d3,&d4,&d5,&d6,&d7,&a0,&a1,&a2,&a3,&a4,&a5,&a6) ccout(NZVC) */
void Op5_SpawnRandom(uint32_t *d0, uint32_t *d1, uint32_t d2, uint32_t d3, uint32_t *d4, uint32_t *d5, uint32_t *d6, uint32_t *d7, uint32_t *a0, uint32_t *a1, uint32_t *a2, uint32_t *a3, uint32_t *a4, uint32_t *a5, uint32_t *a6);	/* $006FD2 regs(&d0,&d1,d2,d3,&d4,&d5,&d6,&d7,&a0,&a1,&a2,&a3,&a4,&a5,&a6) ccout(NZVC) */
void Op5_WaitYield(uint32_t a5, Vm5Wave *wave, dk::Flags *ccr);	/* $006EDE regs(a5,a6,&ccr) ccout(NZVC) */
