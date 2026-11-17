/* funcs/Op2.h - prototypes of the Op2 module: one line per function, its register
 * convention in the tag (dkit genesis recomp bridge). First written from the measured
 * contracts (dkit disasm regs); edited by the C's authors from then on. */
#pragma once
void Op2_SkipWord(uint32_t *next, const u16 **script);	/* $0007E4 regs(&a0,&a2) */
void Op2_Native(const u16 *script);	/* $0007FC regs(a2) */
void Op2_RunSpawnAction(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6, uint32_t *d7, uint32_t *a0, uint32_t *a1, const u16 **script, uint32_t *a3, uint32_t *a4, uint32_t *a5, Vm2Obj **self);	/* $000ACC regs(&d0,&d1,&d2,&d3,&d4,&d5,&d6,&d7,&a0,&a1,&a2,&a3,&a4,&a5,&a6) */
void Op2_LoadPalette1(uint32_t *a1, const u16 **script);	/* $000B5E regs(&a1,&a2) clobbers(a0) */
void Op2_SetAnim(uint32_t *handler, const u16 **script, Vm2Sprite *spr);	/* $000BD0 regs(&a0,&a2,a5) */
void Op2_SetTimer0RandYield0(uint32_t *rand, uint32_t *handler, const u16 **script, Vm2Block *vm);	/* $000C2E regs(&d0,&a0,&a2,a4) */
void Op2_SetTimer2RandYield0(uint32_t *rand, const u16 **script, Vm2Block *vm);	/* $000C5E regs(&d0,&a2,a4) clobbers(a0) */
void Op2_SetTimer0Yield1(const u16 **script, Vm2Block *vm);	/* $000C8E regs(&a2,a4) clobbers(a0) */
void Op2_SetTimer0Yield2(const u16 **script, Vm2Block *vm);	/* $000D26 regs(&a2,a4) clobbers(a0) */
void Op2_SetTimer2Yield2(const u16 **script, Vm2Block *vm);	/* $000D42 regs(&a2,a4) clobbers(a0) */
void Op2_SetTimer3Yield3(const u16 **script, Vm2Block *vm);	/* $000DE8 regs(&a2,a4) clobbers(a0) */
void Op2_SetTimer3RandYield3(uint32_t *rand, const u16 **script, Vm2Block *vm);	/* $000E3E regs(&d0,&a2,a4) clobbers(a0) */
void Op2_LoopTimer0(uint16_t *tick, uint32_t *handler, const u16 **script, Vm2Block *vm);	/* $000F2E regs(&d0,&a0,&a2,a4) */
void Op2_SetTimer1(const u16 **script, Vm2Block *vm);	/* $000F80 regs(&a2,a4) clobbers(a0) */
void Op2_TickTimer1Branch(uint16_t *tick, uint32_t *handler, const u16 **script, Vm2Block *vm);	/* $000F9A regs(&d0,&a0,&a2,a4) */
void Op2_LoopTimer1(uint16_t *tick, uint32_t *handler, const u16 **script, Vm2Block *vm);	/* $001000 regs(&d0,&a0,&a2,a4) */
void Op2_SetTimer2(const u16 **script, Vm2Block *vm);	/* $00105A regs(&a2,a4) clobbers(a0) */
void Op2_LoopTimer2(uint16_t *tick, uint32_t *handler, const u16 **script, Vm2Block *vm);	/* $0010E2 regs(&d0,&a0,&a2,a4) */
void Op2_SetTimer3(const u16 **script, Vm2Block *vm);	/* $001134 regs(&a2,a4) clobbers(a0) */
void Op2_TickTimer3Branch(uint16_t *tick, uint32_t *handler, const u16 **script, Vm2Block *vm);	/* $001156 regs(&d0,&a0,&a2,a4) */
void Op2_LoopTimer3(uint16_t *tick, uint32_t *handler, const u16 **script, Vm2Block *vm);	/* $0011B4 regs(&d0,&a0,&a2,a4) */
void Op2_AllocSubBlock(uint32_t *d0, uint16_t *field, uint32_t *a0, const u16 **script, uint32_t a4, Vm2Obj *self);	/* $00120E regs(&d0,&d1,&a0,&a2,a4,a6) ccout(Z) */
void Op2_ClearNewBlock(uint32_t *d0, u32 *block, const u16 **script);	/* $001240 regs(&d0,a0,&a2) clobbers(a0) */
void Op2_SavePos(const u16 **script, Vm2Obj *self);	/* $001282 regs(&a2,a6) clobbers(a0) */
void Op2_SetCurHeadingArg0(uint16_t *heading, const u16 **script, Vm2Motion *mot);	/* $0012E4 regs(&d0,&a2,a3) clobbers(a0) */
void Op2_AimAtArgPoint(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint16_t *d3, const u16 **script, Vm2Motion *mot, const Vm2Obj *self);	/* $001364 regs(&d0,&d1,&d2,&d3,&a2,a3,a6) clobbers(a0) */
void Op2_BranchPastTarget(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *handler, const u16 **script, Vm2Motion *mot, const Vm2Obj *self);	/* $001450 regs(&d0,&d1,&d2,&a0,&a2,a3,a6) */
void Op2_LaunchAlongHeading(uint16_t *index, uint32_t *d1, uint32_t *d2, const u16 **script, Vm2Motion *mot);	/* $0014F0 regs(&d0,&d1,&d2,&a2,a3) clobbers(a0) */
void Op2_AddTargetY(uint16_t *delta, const u16 **script, Vm2Motion *mot);	/* $001708 regs(&d0,&a2,a3) clobbers(a0) */
void Op2_TargetRandomScreen(const u16 **script, Vm2Motion *mot);	/* $001736 regs(&a2,a3) clobbers(d0,a0) */
void Op2_CruiseNow(const u16 **script, Vm2Motion *mot);	/* $0017EA regs(&a2,a3) clobbers(a0) */
void Op2_StopCruiseX(const u16 **script, Vm2Motion *mot);	/* $00185A regs(&a2,a3) clobbers(a0) */
void Op2_StopX(uint32_t *handler, const u16 **script, Vm2Motion *mot);	/* $001884 regs(&a0,&a2,a3) */
void Op2_SeekWorldX8OrBranch(uint16_t *d0, uint32_t *tolerance, uint32_t *handler, const u16 **script, const Vm2Motion *mot, const Vm2Obj *self);	/* $0019EC regs(&d0,&d1,&a0,&a2,a3,a6) ccout(NVC) */
void Op2_SeekScreenY4(uint16_t *d0, uint32_t *tolerance, uint32_t *handler, const u16 **script, const Vm2Motion *mot, const Vm2Obj *self);	/* $001A5C regs(&d0,&d1,&a0,&a2,a3,a6) ccout(NVC) */
void Op2_SeekWorldY8OrBranch(uint16_t *d0, uint32_t *tolerance, uint32_t *handler, const u16 **script, const Vm2Motion *mot, const Vm2Obj *self);	/* $001B2C regs(&d0,&d1,&a0,&a2,a3,a6) ccout(NVC) */
void Op2_ClearVelY(const u16 **script, Vm2Motion *mot);	/* $001BDA regs(&a2,a3) clobbers(a0) */
void Op2_CruiseDown(uint32_t *handler, const u16 **script, Vm2Motion *mot);	/* $001BEC regs(&a0,&a2,a3) ccout(NVC) */
void Op2_ClearTargetSpeedY(uint32_t *handler, const u16 **script, Vm2Motion *mot);	/* $001C3C regs(&a0,&a2,a3) */
void Op2_SetTerrainBoxB(const u16 **script, Vm2Motion *mot);	/* $001E66 regs(&a2,a3) clobbers(a0) */
void Op2_BranchWallMask(uint16_t *hits, uint32_t *handler, const u16 **script, const Vm2Motion *mot);	/* $001EAC regs(&d0,&a0,&a2,a3) */
void Op2_BranchFloorMask(uint16_t *hits, uint32_t *handler, const u16 **script, const Vm2Motion *mot);	/* $001EEA regs(&d0,&a0,&a2,a3) */
void Op2_BranchNoWall(uint32_t *handler, const u16 **script, const Vm2Motion *mot);	/* $001F38 regs(&a0,&a2,a3) */
void Op2_BranchNotFloorMask(uint16_t *hits, uint32_t *handler, const u16 **script, const Vm2Motion *mot);	/* $001F66 regs(&d0,&a0,&a2,a3) */
void Op2_BranchTerrainCode(uint16_t *code, uint32_t *handler, const u16 **script, const Vm2Motion *mot);	/* $001FB4 regs(&d0,&a0,&a2,a3) */
void Op2_IncVar0(const u16 **script, Vm2Block *vm);	/* $002002 regs(&a2,a4) clobbers(a0) */
void Op2_DecVar0Loop(uint32_t *handler, const u16 **script, Vm2Block *vm);	/* $002020 regs(&a0,&a2,a4) */
void Op2_SwitchVar0Bits(uint16_t *value, uint32_t *pair, uint16_t *count, const u16 **script, Vm2Block *vm);	/* $00208E regs(&d0,&d1,&d7,&a2,a4) clobbers(a0) */
void Op2_DecVar1Loop(uint32_t *handler, const u16 **script, Vm2Block *vm);	/* $0020F2 regs(&a0,&a2,a4) */
void Op2_BranchVar1Lt(uint16_t *value, uint32_t *handler, const u16 **script, Vm2Block *vm);	/* $002150 regs(&d0,&a0,&a2,a4) */
void Op2_SetVar2(const u16 **script, Vm2Block *vm);	/* $002180 regs(&a2,a4) clobbers(a0) */
void Op2_BranchVar2Any(uint16_t *value, uint32_t *handler, const u16 **script, Vm2Block *vm);	/* $0021D2 regs(&d0,&a0,&a2,a4) */
void Op2_SwitchVar2Bits(uint16_t *value, uint32_t *pair, uint16_t *count, const u16 **script, Vm2Block *vm);	/* $002232 regs(&d0,&d1,&d7,&a2,a4) clobbers(a0) */
void Op2_SetVar3(const u16 **script, Vm2Block *vm);	/* $002252 regs(&a2,a4) clobbers(a0) */
void Op2_StopCoroutine0(const u16 **script, Vm2Block *vm);	/* $00231C regs(&a2,a4) clobbers(a0) */
void Op2_StartCoroutine0Here(uint32_t *handler, const u16 **script, Vm2Block *vm);	/* $002334 regs(&a0,&a2,a4) */
void Op2_StepCoroutine0(uint32_t *handler, const u16 **script, Vm2Block *vm);	/* $00235A regs(&a0,&a2,a4) */
void Op2_StartCoroutine1Here(uint32_t *handler, const u16 **script, Vm2Block *vm);	/* $00237C regs(&a0,&a2,a4) */
void Op2_StepCoroutine1(uint32_t *handler, const u16 **script, Vm2Block *vm);	/* $0023A2 regs(&a0,&a2,a4) */
void Op2_StartCoroutine2Here(uint32_t *handler, const u16 **script, Vm2Block *vm);	/* $0023C4 regs(&a0,&a2,a4) */
void Op2_CallSlot2Native(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6, uint32_t *d7, uint32_t *a0, uint32_t *a1, const u16 **script, Vm2Block *vm, uint32_t a6);	/* $0023E0 regs(&d0,&d1,&d2,&d3,&d4,&d5,&d6,&d7,&a0,&a1,&a2,a4,a6) */
/* a native routine in coroutine slot 2, called by Op2_CallSlot2Native (`jsr (a0)` at $23E4; the corpus's one
 * target is Boss_Claw_SwingAtCraneStep $1EA24): the op's own convention, so whatever the routine leaves in
 * them goes on to the script's next op */
void Slot2NativeFn(uint32_t code, uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6, uint32_t *d7, uint32_t *a0, uint32_t *a1, const u16 **script, Vm2Block *vm, uint32_t a6);	/* type regs(&d0,&d1,&d2,&d3,&d4,&d5,&d6,&d7,&a0,&a1,&a2,a4,a6) */
void AnimOpFn(uint32_t code, uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6, uint32_t *d7, uint32_t *a0, uint32_t *a1, uint32_t *a2, uint32_t *a3, uint32_t *a4, uint32_t *a5, uint32_t *a6);	/* type regs(&d0,&d1,&d2,&d3,&d4,&d5,&d6,&d7,&a0,&a1,&a2,&a3,&a4,&a5,&a6) */
void Op2_StartCoroutine3Here(const u16 **script, Vm2Block *vm);	/* $00240C regs(&a2,a4) clobbers(a0) */
void Op2_StepCoroutine3(const u16 **script, Vm2Block *vm);	/* $002432 regs(&a2,a4) clobbers(a0) */
void Op2_SpawnChild(uint32_t *d0, uint16_t *d1, uint32_t *d4, uint32_t *d5, uint32_t *d7, uint32_t *a0, uint32_t *a1, const u16 **script, uint32_t a3, uint32_t a4, uint32_t a5, Vm2Obj *self);	/* $00244A regs(&d0,&d1,&d4,&d5,&d7,&a0,&a1,&a2,a3,a4,a5,a6) */
/* a child spawner called by Op2_SpawnChild (`jsr (a0)` at $2462; 28 targets in the corpus, e.g. ToyChain_MakeChild5
 * $398BA, ToyChain_MakeHanging6 $3A73E): the op's convention; a6 = the new object, Z set = spawned */
bool ChildSpawnerFn(uint32_t code, uint32_t *d0, uint16_t *d1, uint32_t *d4, uint32_t *d5, uint32_t *d7, uint32_t *a0, uint32_t *a1, const u16 **script, uint32_t a3, uint32_t a4, uint32_t a5, uint32_t *a6);	/* type regs(&d0,&d1,&d4,&d5,&d7,&a0,&a1,&a2,a3,a4,a5,&a6)->Z */
void Op2_SpawnChildOrWait(uint32_t *d0, uint32_t *d1, uint16_t *d4, uint16_t *d5, uint32_t *d7, uint32_t *a0, const u16 **script, uint32_t a3, uint32_t a4, uint32_t a5, Vm2Obj *self);	/* $002486 regs(&d0,&d1,&d4,&d5,&d7,&a0,&a2,a3,a4,a5,a6) ccout(Z) */
/* Op2_SpawnChildOrWait's spawner (`jsr (a0)` at $24A0; 12 targets in the corpus, e.g. $327FE, $32AAA): the op's
 * convention; a6 = the new object, Z set = spawned */
bool ChildSpawnerWaitFn(uint32_t code, uint32_t *d0, uint32_t *d1, uint16_t *d4, uint16_t *d5, uint32_t *d7, uint32_t *a0, const u16 **script, uint32_t a3, uint32_t a4, uint32_t a5, uint32_t *a6);	/* type regs(&d0,&d1,&d4,&d5,&d7,&a0,&a2,a3,a4,a5,&a6)->Z */
void Op2_StartChildCoroutine0Rel(uint32_t *d0, uint32_t *d1, uint32_t d2, uint32_t d3, uint32_t d4, uint32_t d5, uint32_t d6, uint32_t d7, uint32_t *a1, const u16 **script, uint32_t a3, uint32_t a4, uint32_t a5, const Vm2Obj *self);	/* $002AA6 regs(&d0,&d1,d2,d3,d4,d5,d6,d7,&a1,&a2,a3,a4,a5,a6) clobbers(a0) */
void Op2_StopChildCoroutine1(uint32_t *d0, uint32_t *d1, uint32_t d2, uint32_t d3, uint32_t d4, uint32_t d5, uint32_t d6, uint32_t d7, uint32_t a1, const u16 **script, uint32_t a3, uint32_t a4, uint32_t a5, const Vm2Obj *self);	/* $002AEA regs(&d0,&d1,d2,d3,d4,d5,d6,d7,a1,&a2,a3,a4,a5,a6) clobbers(a0) */
void Op2_StartChildCoroutine1Rel(uint32_t *d0, uint32_t *d1, uint32_t d2, uint32_t d3, uint32_t d4, uint32_t d5, uint32_t d6, uint32_t d7, uint32_t *a1, const u16 **script, uint32_t a3, uint32_t a4, uint32_t a5, const Vm2Obj *self);	/* $002B16 regs(&d0,&d1,d2,d3,d4,d5,d6,d7,&a1,&a2,a3,a4,a5,a6) clobbers(a0) */
void Op2_StartChildCoroutine2Rel(uint32_t *d0, uint32_t *d1, uint32_t d2, uint32_t d3, uint32_t d4, uint32_t d5, uint32_t d6, uint32_t d7, uint32_t *a1, const u16 **script, uint32_t a3, uint32_t a4, uint32_t a5, const Vm2Obj *self);	/* $002B86 regs(&d0,&d1,d2,d3,d4,d5,d6,d7,&a1,&a2,a3,a4,a5,a6) clobbers(a0) */
void Op2_StartChildCoroutine3Rel(uint32_t *d0, uint32_t *d1, uint32_t d2, uint32_t d3, uint32_t d4, uint32_t d5, uint32_t d6, uint32_t d7, uint32_t *a1, const u16 **script, uint32_t a3, uint32_t a4, uint32_t a5, const Vm2Obj *self);	/* $002BF6 regs(&d0,&d1,d2,d3,d4,d5,d6,d7,&a1,&a2,a3,a4,a5,a6) clobbers(a0) */
void Op2_SetCounter0(const u16 **script, Vm2Block *vm);	/* $002C3A regs(&a2,a4) clobbers(a0) */
void Op2_LoopCounter1(uint32_t *handler, const u16 **script, Vm2Block *vm);	/* $002C7C regs(&a0,&a2,a4) */
void Op2_LoopCounter2(uint32_t *handler, const u16 **script, Vm2Block *vm);	/* $002CA4 regs(&a0,&a2,a4) */
void Op2_GosubAbsArgs(const u16 **script);	/* $002CE6 regs(&a2) clobbers(a0) */
void Op2_Return(uint32_t *handler, const u16 **script);	/* $002D2A regs(&a0,&a2) */
void Op2_CruiseToPlayerX(uint16_t *d0, uint32_t *a0, const u16 **script, Vm2Motion *mot, const Vm2Block *vm, const Vm2Obj *self);	/* $002F7C regs(&d0,&a0,&a2,a3,a4,a6) */
void Op2_SetRandJitter(uint32_t *d0, const u16 **script, Vm2Obj *self);	/* $002FCE regs(&d0,&a2,a6) clobbers(a0) */
void Op2_HoldRangeOfPlayerX(uint32_t *d0, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint16_t *d5, uint16_t *d6, const u16 **script, Vm2Motion *mot, const Vm2Block *vm, Vm2Sprite *spr, const Vm2Obj *self);	/* $002FF6 regs(&d0,&d2,&d3,&d4,&d5,&d6,&a2,a3,a4,a5,a6) clobbers(a0) */
void Op2_FaceCruiseToPlayerXTurnBranch(uint32_t *d0, uint32_t *a0, const u16 **script, Vm2Motion *mot, const Vm2Block *vm, Vm2Sprite *spr, const Vm2Obj *self);	/* $003342 regs(&d0,&a0,&a2,a3,a4,a5,a6) */
void Op2_BranchPlayerInBox(uint32_t *d0, uint16_t *d1, uint16_t *d2, uint16_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6, uint32_t *d7, const u16 **script, const Vm2Block *vm, const Vm2Sprite *spr, const Vm2Obj *self);	/* $003434 regs(&d0,&d1,&d2,&d3,&d4,&d5,&d6,&d7,&a2,a4,a5,a6) clobbers(a0) */
void Op2_BranchTouchBox(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6, uint32_t *d7, uint32_t *a0, uint32_t *a1, const u16 **script, uint32_t a4, const Vm2Sprite *spr, const Vm2Obj *self);	/* $003714 regs(&d0,&d1,&d2,&d3,&d4,&d5,&d6,&d7,&a0,&a1,&a2,a4,a5,a6) ccout(NVC) */
void Op2_BranchTouchBox_Kind0(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6, uint32_t *d7, uint32_t *a0, uint32_t *a1, const u16 **script, uint32_t a4, uint32_t a5, uint32_t a6);	/* $00374A regs(&d0,&d1,&d2,&d3,&d4,&d5,&d6,&d7,&a0,&a1,&a2,a4,a5,a6) */
void Op2_BranchTouchBox_Kind4(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6, uint32_t *d7, uint32_t *a0, uint32_t *a1, const u16 **script, uint32_t a4, uint32_t a5, uint32_t a6);	/* $00374E regs(&d0,&d1,&d2,&d3,&d4,&d5,&d6,&d7,&a0,&a1,&a2,a4,a5,a6) */
void Op2_BranchTouchBox_Kind8(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6, uint32_t *d7, uint32_t *a0, uint32_t *a1, const u16 **script, uint32_t a4, uint32_t a5, uint32_t a6);	/* $003752 regs(&d0,&d1,&d2,&d3,&d4,&d5,&d6,&d7,&a0,&a1,&a2,a4,a5,a6) */
void Op2_BranchTouchBox_Players(uint32_t *d0, uint16_t left, uint16_t top, uint16_t right, uint16_t bottom, uint32_t *a0, const u16 **script);	/* $00375A regs(&d0,d4,d5,d6,d7,&a0,&a2) */
void Op2_BranchTouchBox_Miss(uint32_t *d0, uint16_t left, uint16_t top, uint16_t right, uint16_t bottom, uint32_t *a0, const u16 **script);	/* $003760 regs(&d0,d4,d5,d6,d7,&a0,&a2) */
void Op2_BranchTouchBox_Hit(uint32_t *d0, uint16_t left, uint16_t top, uint16_t right, uint16_t bottom, uint32_t *a0, uint32_t *a2);	/* $00376E regs(&d0,d4,d5,d6,d7,&a0,&a2) */
void Op2_BranchBoxStruck(uint32_t *d0, uint32_t *a0, uint32_t *a1, const u16 **script, Vm2Block *vm);	/* $003928 regs(&d0,&a0,&a1,&a2,a4) */
void Op2_SwitchAttacker(uint16_t *attacker, uint16_t *count, uint32_t *box_or_handler, const u16 **script);	/* $003A74 regs(&d0,&d7,&a0,&a2) */
void Op2_SetVector0HereGotoArg(const u16 **script, Vm2Obj *self);	/* $003ABE regs(&a2,a6) clobbers(a0) */
void Op2_SetVector1HereGoto(const u16 **script, Vm2Obj *self);	/* $003ADE regs(&a2,a6) clobbers(a0) */
void Op2_GotoVector2Plus(const u16 **script, Vm2Obj *self);	/* $003B50 regs(&a2,a6) clobbers(a0) */
void Op2_GosubVector2Plus(const u16 **script, Vm2Obj *self);	/* $003B76 regs(&a2,a6) clobbers(a0) */
void Op2_SwitchAimDir(const u16 **script, const Vm2Sprite *spr);	/* $003CCA regs(&a2,a5) clobbers(d0,a0) */
void Op2_BranchWeaponLvlMaxGE(uint16_t *stat, uint32_t *handler, const u16 **script);	/* $003CEE regs(&d0,&a0,&a2) */
void Op2_BranchStatGE(uint16_t stat, uint32_t *handler, const u16 **script);	/* $003D58 regs(d0,&a0,&a2) */
void Op2_RunAnim(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6, uint32_t *d7, uint32_t *a0, uint32_t *a1, uint32_t *a2, uint32_t *a3, uint32_t *a4, uint32_t *a5, uint32_t *a6);	/* $003E58 regs(&d0,&d1,&d2,&d3,&d4,&d5,&d6,&d7,&a0,&a1,&a2,&a3,&a4,&a5,&a6) ccout(NZVC) */
void Op2_SwitchHeading(uint32_t *d0, const u16 **script, const Vm2Motion *mot);	/* $003F6C regs(&d0,&a2,a3) clobbers(a0) */
void Op2_AnimFrame(uint16_t *delay, uint32_t *draw, const u16 **script, Vm2Sprite *spr, dk::Flags *ccr);	/* $003FE4 regs(&d0,&a0,&a2,a5,&ccr) ccout(NZVC) */
void Op2_SetSprVar0(const u16 **script, Vm2Sprite *spr);	/* $003FFE regs(&a2,a5) clobbers(a0) */
void Op2_SetSprVar1(const u16 **script, Vm2Sprite *spr);	/* $004098 regs(&a2,a5) clobbers(a0) */
void Op2_SetSprVar2(const u16 **script, Vm2Sprite *spr);	/* $004132 regs(&a2,a5) clobbers(a0) */
void Op2_BranchNotSection(uint16_t *section, uint32_t *handler, const u16 **script);	/* $0042FE regs(&d0,&a0,&a2) */
void Op2_SetBlinkTimer(uint16_t *frames, uint32_t *handler, const u16 **script, Vm2Sprite *spr);	/* $004326 regs(&d0,&a0,&a2,a5) */
void Op2_PlaySound(uint32_t *sound, const u16 **script);	/* $00433E regs(&d0,&a2) clobbers(a0) */
void Op2_WaitPopBelow(uint32_t *population, uint32_t *a0, const u16 **script, uint32_t a4);	/* $00437A regs(&d0,&a0,&a2,a4) ccout(Z) */
void Op2_StartA6Script(const u16 **script);	/* $0043E2 regs(&a2) clobbers(a0) */
void Op2_DrawInScreenBox(uint32_t *d0, uint32_t *a0, uint32_t *a1, const u16 **script, Vm2Sprite *spr, Vm2Obj *self);	/* $004454 regs(&d0,&a0,&a1,&a2,a5,a6) */
void Op2_DrawSprFront(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint16_t *d4, uint16_t *d5, uint16_t *d6, uint32_t *a0, uint32_t *a1, const u16 **script, const Vm2Sprite *spr, const Vm2Obj *self);	/* $0046CC regs(&d0,&d1,&d2,&d3,&d4,&d5,&d6,&a0,&a1,&a2,a5,a6) */
void Op2_DrawSprBack(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint16_t *d4, uint16_t *d5, uint16_t *d6, uint32_t *a0, uint32_t *a1, const u16 **script, const Vm2Sprite *spr, const Vm2Obj *self);	/* $00478C regs(&d0,&d1,&d2,&d3,&d4,&d5,&d6,&a0,&a1,&a2,a5,a6) */
void Op2_DrawSprAtDrawnDepth(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint16_t *d4, uint16_t *d5, uint16_t *d6, uint16_t *d7, uint32_t *a1, const u16 **script, const Vm2Sprite *spr, const Vm2Obj *self);	/* $004898 regs(&d0,&d1,&d2,&d3,&d4,&d5,&d6,&d7,&a1,&a2,a5,a6) clobbers(a0) */
void Op2_DrawSprAtDrawnBack(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint16_t *d4, uint16_t *d5, uint16_t *d6, uint32_t *a0, uint32_t *a1, const u16 **script, const Vm2Sprite *spr, const Vm2Obj *self);	/* $004AB4 regs(&d0,&d1,&d2,&d3,&d4,&d5,&d6,&a0,&a1,&a2,a5,a6) */
void Op2_StepVelocity(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6, uint32_t *a0, const u16 **script, Vm2Motion *mot);	/* $004B74 regs(&d0,&d1,&d2,&d3,&d4,&d5,&d6,&a0,&a2,a3) ccout(NVC) */
void Op2_MoveTerrain(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6, uint32_t *d7, uint32_t *a0, uint32_t *a1, uint32_t *a2, uint32_t *a3, uint32_t *a4, uint32_t *a5, uint32_t *a6);	/* $004CDA regs(&d0,&d1,&d2,&d3,&d4,&d5,&d6,&d7,&a0,&a1,&a2,&a3,&a4,&a5,&a6) */
void Op2_TestAttackBoxAtDrawn(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6, uint32_t *d7, uint32_t *a0, uint32_t *a1, uint32_t *a2, uint32_t a3, uint32_t *a4, uint32_t a5, uint32_t *a6);	/* $004EA8 regs(&d0,&d1,&d2,&d3,&d4,&d5,&d6,&d7,&a0,&a1,&a2,a3,&a4,a5,&a6) ccout(NVC) */
void Op2_PlaceBodyBoxes_Off(uint16_t *d0, uint32_t *a0, uint32_t *a1, const u16 **script, Vm2Block *vm);	/* $004F4E regs(&d0,&a0,&a1,&a2,a4) ccout(NVC) */
void Op2_ClearMotionBlock(uint32_t *d0, uint32_t *a0, const u16 **script, const Vm2Obj *self, dk::Flags *ccr);	/* $005078 regs(&d0,&a0,&a2,a6,&ccr) ccout(NVC) */
void Op2_StopOsc(uint16_t *d0, uint32_t *a0, const u16 **script, Vm2Obj *self);	/* $0050A2 regs(&d0,&a0,&a2,a6) */
void Op2_StopOscY(uint16_t *d0, uint32_t *a0, const u16 **script, Vm2Obj *self);	/* $0050CE regs(&d0,&a0,&a2,a6) */
void Op2_FxSpawnOffAbs(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6, uint32_t *d7, uint32_t *a0, uint32_t a1, const u16 **script, const Vm2Motion *mot, uint32_t a4, const Vm2Sprite *spr, Vm2Obj *self);	/* $0051F2 regs(&d0,&d1,&d2,&d3,&d4,&d5,&d6,&d7,&a0,a1,&a2,a3,a4,a5,a6) ccout(NZVC) */
void Op2_FxHeadingHere(uint32_t *d3, const u16 **script, const Vm2Motion *mot);	/* $0052F0 regs(&d3,&a2,a3) clobbers(a0) */
void Op2_FxHeading(uint32_t *d3, const u16 **script);	/* $005306 regs(&d3,&a2) clobbers(a0) */
void Op2_FxMarkPlayerTarget(const u16 **script, const Vm2Block *vm);	/* $005378 regs(&a2,a4) clobbers(d0,a0) */
void Op2_BrakeXGotoIfStopped(uint32_t *handler, const u16 **script, Vm2Motion *mot);	/* $001872 regs(&a0,&a2,a3) ccout(NVC) */
void Op2_SetOwnerId(const u16 **script, Vm2Obj *self);	/* $000804 regs(&a2,a6) clobbers(a0) */
void Op2_SetResume(const u16 **script, Vm2Block *vm);	/* $00080C regs(&a2,a4) clobbers(a0) */
void Op2_SetResumeHere(const u16 **script, Vm2Block *vm);	/* $000818 regs(&a2,a4) clobbers(a0) */
void Op2_YieldGoto(uint32_t *result, const u16 **script, Vm2Block *vm);	/* $000820 regs(&d0,&a2,a4) tst(d0.l) */
uint32_t Op2_Yield(const u16 *script, Vm2Block *vm);	/* $000822 regs(a2,a4)->d0 tst(d0.l) */
void Op2_BranchOffScrBox(uint16_t *screen, uint32_t *handler, const u16 **script, const Vm2Obj *self);	/* $00082E regs(&d0,&a0,&a2,a6) */
void Op2_BranchOffScrX(uint16_t *screen, uint32_t *handler, const u16 **script, const Vm2Obj *self);	/* $00084E regs(&d0,&a0,&a2,a6) */
void Op2_SwitchScrX(uint16_t *screen, uint32_t *handler, const u16 **script, const Vm2Obj *self);	/* $000868 regs(&d0,&a0,&a2,a6) */
void Op2_BranchOffScrY(uint16_t *screen, uint32_t *handler, const u16 **script, const Vm2Obj *self);	/* $000882 regs(&d0,&a0,&a2,a6) */
void Op2_KillOffscreen50(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6, uint32_t *d7, uint32_t *handler, uint32_t *a1, const u16 **script, uint32_t a4, uint32_t a5, const Vm2Obj *self);	/* $0008A2 regs(&d0,&d1,&d2,&d3,&d4,&d5,&d6,&d7,&a0,&a1,&a2,a4,a5,a6) tst(d0.l) */
void Op2_KillOffscreen100(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6, uint32_t *d7, uint32_t *handler, uint32_t *a1, const u16 **script, uint32_t a4, uint32_t a5, const Vm2Obj *self);	/* $0008CE regs(&d0,&d1,&d2,&d3,&d4,&d5,&d6,&d7,&a0,&a1,&a2,a4,a5,a6) tst(d0.l) */
void Op2_KillOffscreen200(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6, uint32_t *d7, uint32_t *handler, uint32_t *a1, const u16 **script, uint32_t a4, uint32_t a5, const Vm2Obj *self);	/* $0008FA regs(&d0,&d1,&d2,&d3,&d4,&d5,&d6,&d7,&a0,&a1,&a2,a4,a5,a6) tst(d0.l) */
void Op2_KillOffScrX(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6, uint32_t *d7, uint32_t *handler, uint32_t *a1, const u16 **script, uint32_t a4, uint32_t a5, const Vm2Obj *self);	/* $000926 regs(&d0,&d1,&d2,&d3,&d4,&d5,&d6,&d7,&a0,&a1,&a2,a4,a5,a6) tst(d0.l) */
void Op2_KillOffScrBox(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6, uint32_t *d7, uint32_t *handler, uint32_t *a1, const u16 **script, uint32_t a4, uint32_t a5, const Vm2Obj *self);	/* $00093A regs(&d0,&d1,&d2,&d3,&d4,&d5,&d6,&d7,&a0,&a1,&a2,a4,a5,a6) tst(d0.l) */
void Op2_Kill(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6, uint32_t *d7, uint32_t *a0, uint32_t *a1, uint32_t *a2, uint32_t a4, uint32_t a5, uint32_t a6);	/* $00095C regs(&d0,&d1,&d2,&d3,&d4,&d5,&d6,&d7,&a0,&a1,&a2,a4,a5,a6) tst(d0.l) */
void Op2_BranchRandom(uint32_t *rand, uint32_t *handler, const u16 **script);	/* $0009E2 regs(&d0,&a0,&a2) */
void Op2_SwitchRandom(uint32_t *rand, uint32_t *handler, const u16 **script);	/* $0009F4 regs(&d0,&a0,&a2) */
void Op2_ClearPopulation(uint32_t *weight, uint32_t *handler, const u16 **script, Vm2Obj *self);	/* $000A0C regs(&d0,&a0,&a2,a6) ccout(NVC) */
void Op2_IncPopulation(uint32_t *handler, const u16 **script, Vm2Obj *self);	/* $000A26 regs(&a0,&a2,a6) */
void Op2_AddPopulation(uint16_t *n, uint32_t *handler, const u16 **script, Vm2Obj *self);	/* $000A32 regs(&d0,&a0,&a2,a6) */
void Op2_SetScoreValue(const u16 **script, Vm2Block *vm);	/* $000A40 regs(&a2,a4) clobbers(a0) */
void Op2_AwardScoreToHitter(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6, uint32_t *d7, uint32_t *a0, uint32_t *a1, uint32_t *a2, uint32_t *a3, Vm2Block **vm, uint32_t *a5, uint32_t *a6);	/* $000A48 regs(&d0,&d1,&d2,&d3,&d4,&d5,&d6,&d7,&a0,&a1,&a2,&a3,&a4,&a5,&a6) */
void Op2_AwardScoreToPlayers(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6, uint32_t *d7, uint32_t *a0, uint32_t *a1, uint32_t *a2, uint32_t *a3, Vm2Block **vm, uint32_t *a5, uint32_t *a6);	/* $000A80 regs(&d0,&d1,&d2,&d3,&d4,&d5,&d6,&d7,&a0,&a1,&a2,&a3,&a4,&a5,&a6) */
void Op2_SetSpawnAction(uint16_t *type, uint32_t *handler, const u16 **script, Vm2Obj *self);	/* $000AA6 regs(&d0,&a0,&a2,a6) */
void Op2_SetSpeedRand(uint32_t *speed, uint32_t *handler, const u16 **script, Vm2Block *vm);	/* $000AE6 regs(&d0,&a0,&a2,a4) */
void Op2_SetSpeed(uint32_t *speed, uint32_t *handler, const u16 **script, Vm2Block *vm);	/* $000AF2 regs(&d0,&a0,&a2,a4) */
void Op2_SetSprAttr(const u16 **script, Vm2Sprite *spr);	/* $000B0E regs(&a2,a5) clobbers(a0) */
void Op2_ModSprAttr(uint8_t *mask, uint32_t *handler, const u16 **script, Vm2Sprite *spr);	/* $000B16 regs(&d0,&a0,&a2,a5) */
void Op2_BranchSprAttrEq(uint8_t *bits, uint32_t *handler, const u16 **script, const Vm2Sprite *spr);	/* $000B26 regs(&d0,&a0,&a2,a5) */
void Op2_BranchSprAttrNe(uint8_t *bits, uint32_t *handler, const u16 **script, const Vm2Sprite *spr);	/* $000B38 regs(&d0,&a0,&a2,a5) */
void Op2_SetSprDepth(uint32_t *handler, const u16 **script, Vm2Sprite *spr);	/* $000B4A regs(&a0,&a2,a5) */
void Op2_LoadPalette2(uint32_t *src, uint32_t *a1, const u16 **script);	/* $000B6A regs(&a0,&a1,&a2) ccout(NVC) */
void Op2_LoadPalette3(uint32_t *a1, const u16 **script);	/* $000B76 regs(&a1,&a2) clobbers(a0) */
void Op2_SetDepthFromY(uint16_t *depth, const u16 **script, Vm2Sprite *spr, const Vm2Obj *self);	/* $000B82 regs(&d0,&a2,a5,a6) clobbers(a0) */
void Op2_BranchAnimFlag(uint32_t *handler, const u16 **script, const Vm2Sprite *spr);	/* $000BE6 regs(&a0,&a2,a5) */
void Op2_SetTimer0Yield0(uint32_t *handler, const u16 **script, Vm2Block *vm);	/* $000BF6 regs(&a0,&a2,a4) */
void Op2_SetTimer1Yield0(const u16 **script, Vm2Block *vm);	/* $000C04 regs(&a2,a4) clobbers(a0) */
void Op2_SetTimer2Yield0(const u16 **script, Vm2Block *vm);	/* $000C12 regs(&a2,a4) clobbers(a0) */
void Op2_SetTimer1Yield1(const u16 **script, Vm2Block *vm);	/* $000C9C regs(&a2,a4) clobbers(a0) */
void Op2_SetTimer2Yield1(const u16 **script, Vm2Block *vm);	/* $000CAA regs(&a2,a4) clobbers(a0) */
void Op2_SetTimer3Yield1(const u16 **script, Vm2Block *vm);	/* $000CB8 regs(&a2,a4) clobbers(a0) */
void Op2_SetTimer0RandYield1(uint32_t *rand, const u16 **script, Vm2Block *vm);	/* $000CC6 regs(&d0,&a2,a4) clobbers(a0) */
void Op2_SetTimer1RandYield1(uint32_t *rand, const u16 **script, Vm2Block *vm);	/* $000CDE regs(&d0,&a2,a4) clobbers(a0) */
void Op2_SetTimer3Yield2(const u16 **script, Vm2Block *vm);	/* $000D50 regs(&a2,a4) clobbers(a0) */
void Op2_WaitTimer0(uint16_t *tick, uint32_t *handler, const u16 **script, Vm2Block *vm);	/* $000E56 regs(&d0,&a0,&a2,a4) */
void Op2_WaitTimer1(uint16_t *tick, uint32_t *handler, const u16 **script, Vm2Block *vm);	/* $000E6A regs(&d0,&a0,&a2,a4) */
void Op2_WaitTimer2(uint16_t *tick, uint32_t *handler, const u16 **script, Vm2Block *vm);	/* $000E7E regs(&d0,&a0,&a2,a4) */
void Op2_WaitTimer3(uint16_t *tick, uint32_t *handler, const u16 **script, Vm2Block *vm);	/* $000E92 regs(&d0,&a0,&a2,a4) */
void Op2_SetTimer0(uint32_t *handler, const u16 **script, Vm2Block *vm);	/* $000EA6 regs(&a0,&a2,a4) */
void Op2_SetTimer0Rand(uint32_t *rand, const u16 **script, Vm2Block *vm);	/* $000EAE regs(&d0,&a2,a4) clobbers(a0) */
void Op2_TickTimer0Branch(uint16_t *tick, uint32_t *handler, const u16 **script, Vm2Block *vm);	/* $000EC0 regs(&d0,&a0,&a2,a4) */
void Op2_TickTimer0GotoV0(uint16_t *tick, uint32_t *handler, const u16 **script, Vm2Block *vm, const Vm2Obj *self);	/* $000EDE regs(&d0,&a0,&a2,a4,a6) */
void Op2_TickTimer0GotoV1(uint16_t *tick, uint32_t *handler, const u16 **script, Vm2Block *vm, const Vm2Obj *self);	/* $000EF6 regs(&d0,&a0,&a2,a4,a6) */
void Op2_SetTimer2Rand(uint32_t *rand, const u16 **script, Vm2Block *vm);	/* $001062 regs(&d0,&a2,a4) clobbers(a0) */
void Op2_TickTimer2Branch(uint16_t *tick, uint32_t *handler, const u16 **script, Vm2Block *vm);	/* $001074 regs(&d0,&a0,&a2,a4) */
void Op2_SetTimer3Rand(uint32_t *rand, const u16 **script, Vm2Block *vm);	/* $00113C regs(&d0,&a2,a4) clobbers(a0) */
void Op2_LoadA3Handle(uint16_t *field, const u16 **script, uint32_t *a3, const Vm2Obj *self);	/* $00122C regs(&d0,&a2,&a3,a6) clobbers(a0) */
void Op2_ResetA3(const u16 **script, uint32_t *a3, const Vm2Obj *self);	/* $001266 regs(&a2,&a3,a6) clobbers(a0) */
void Op2_SwapSavedPos(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *handler, const u16 **script, Vm2Obj *self);	/* $001292 regs(&d0,&d1,&d2,&d3,&a0,&a2,a6) */
void Op2_StepSavedPos(uint32_t *d0, const u16 **script, const Vm2Motion *mot, Vm2Obj *self);	/* $0012A6 regs(&d0,&a2,a3,a6) clobbers(a0) */
void Op2_SetTurnRate(uint32_t *rate, const u16 **script, Vm2Motion *mot, const Vm2Block *vm);	/* $0012BA regs(&d0,&a2,a3,a4) clobbers(a0) */
void Op2_SetHeading(uint16_t *heading, const u16 **script, Vm2Motion *mot);	/* $0012C8 regs(&d0,&a2,a3) clobbers(a0) */
void Op2_AddHeading(const u16 **script, Vm2Motion *mot);	/* $0012F4 regs(&a2,a3) clobbers(d0,a0) */
void Op2_SetHeadingNow(const u16 **script, Vm2Motion *mot);	/* $001306 regs(&a2,a3) clobbers(d0,a0) */
void Op2_HeadingFromVel(uint32_t *d0, uint32_t *d1, uint32_t *d2, const u16 **script, Vm2Motion *mot);	/* $00131C regs(&d0,&d1,&d2,&a2,a3) clobbers(a0) */
void Op2_AimAtTargetOffset(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint16_t *d3, const u16 **script, Vm2Motion *mot, const Vm2Obj *self);	/* $001382 regs(&d0,&d1,&d2,&d3,&a2,a3,a6) clobbers(a0) */
void Op2_AimAtTargetArgOffset(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint16_t *d3, const u16 **script, Vm2Motion *mot, const Vm2Obj *self);	/* $0013A4 regs(&d0,&d1,&d2,&d3,&a2,a3,a6) clobbers(a0) */
void Op2_AimAtTracked(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *handler, const u16 **script, Vm2Motion *mot, const Vm2Block *vm, const Vm2Obj *self);	/* $001528 regs(&d0,&d1,&d2,&a0,&a2,a3,a4,a6) */
void Op2_SnapHeadingSteer(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6, uint32_t *a0, uint32_t *a2, Vm2Motion *mot);	/* $001554 regs(&d0,&d1,&d2,&d3,&d4,&d5,&d6,&a0,&a2,a3) ccout(NVC) */
void Op2_TurnSteer(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6, uint32_t *a0, uint32_t *a2, Vm2Motion *mot);	/* $00155C regs(&d0,&d1,&d2,&d3,&d4,&d5,&d6,&a0,&a2,a3) ccout(NVC) */
void Op2_TurnHeading(uint32_t *d0, uint32_t *d1, uint32_t *d2, const u16 **script, Vm2Motion *mot);	/* $001562 regs(&d0,&d1,&d2,&a2,a3) clobbers(a0) */
void Op2_SetPos(uint32_t *handler, const u16 **script, Vm2Obj *self);	/* $00161A regs(&a0,&a2,a6) */
void Op2_SetX(const u16 **script, Vm2Obj *self);	/* $00162E regs(&a2,a6) clobbers(a0) */
void Op2_MovePos(uint16_t *delta, const u16 **script, Vm2Obj *self);	/* $00163A regs(&d0,&a2,a6) clobbers(a0) */
void Op2_MoveX(uint16_t *delta, const u16 **script, Vm2Obj *self);	/* $00164A regs(&d0,&a2,a6) clobbers(a0) */
void Op2_MoveXFacing(uint32_t *handler, const u16 **script, const Vm2Sprite *spr, Vm2Obj *self);	/* $001654 regs(&a0,&a2,a5,a6) clobbers(d0) */
void Op2_PlaceOnScreen(uint32_t *x, uint32_t *y, uint32_t *handler, const u16 **script, Vm2Motion *mot, Vm2Obj *self);	/* $001668 regs(&d0,&d1,&a0,&a2,a3,a6) */
void Op2_PlaceXOnScreen(uint32_t *x, const u16 **script, Vm2Motion *mot, Vm2Obj *self);	/* $001696 regs(&d0,&a2,a3,a6) clobbers(a0) */
void Op2_PlaceYOnScreen(uint32_t *y, const u16 **script, Vm2Motion *mot, Vm2Obj *self);	/* $0016B0 regs(&d0,&a2,a3,a6) clobbers(a0) */
void Op2_SetTargetArgs(const u16 **script, Vm2Motion *mot);	/* $0016CA regs(&a2,a3) clobbers(a0) */
void Op2_SetTarget(const u16 **script, Vm2Motion *mot);	/* $0016DA regs(&a2,a3) clobbers(a0) */
void Op2_SetTargetX(const u16 **script, Vm2Motion *mot);	/* $0016E6 regs(&a2,a3) clobbers(a0) */
void Op2_AddTargetX(const u16 **script, Vm2Motion *mot);	/* $001712 regs(&a2,a3) clobbers(d0,a0) */
void Op2_TargetSelf(uint32_t *handler, const u16 **script, Vm2Motion *mot, const Vm2Obj *self);	/* $00171C regs(&a0,&a2,a3,a6) */
void Op2_TargetRandomScreenArea(uint32_t *r, const u16 **script, Vm2Motion *mot);	/* $00175E regs(&d0,&a2,a3) clobbers(a0) */
void Op2_SetAccelForDist(uint16_t *speed, uint32_t *accel, uint32_t *series, uint16_t *time, const u16 **script, Vm2Motion *mot, const Vm2Block *vm);	/* $001782 regs(&d0,&d1,&d2,&d3,&a2,a3,a4) clobbers(a0) */
void Op2_SetCruise(uint32_t *cruise, uint32_t *handler, const u16 **script, Vm2Motion *mot, const Vm2Block *vm);	/* $001792 regs(&d0,&a0,&a2,a3,a4) */
void Op2_ClearImpulseX(const u16 **script, Vm2Motion *mot);	/* $0017A4 regs(&a2,a3) clobbers(a0) */
void Op2_SetAccelXForDist(uint16_t *speed, uint32_t *accel, uint32_t *series, uint16_t *time, const u16 **script, Vm2Motion *mot, const Vm2Block *vm);	/* $0017AC regs(&d0,&d1,&d2,&d3,&a2,a3,a4) clobbers(a0) */
void Op2_SetCruiseX(uint32_t *cruise, uint32_t *handler, const u16 **script, Vm2Motion *mot, const Vm2Block *vm);	/* $0017B8 regs(&d0,&a0,&a2,a3,a4) */
void Op2_SetVelXForward(uint32_t *vel, uint32_t *handler, const u16 **script, Vm2Motion *mot, const Vm2Block *vm, const Vm2Sprite *spr);	/* $0017C6 regs(&d0,&a0,&a2,a3,a4,a5) */
void Op2_CruiseForward(uint32_t *handler, const u16 **script, Vm2Motion *mot, const Vm2Sprite *spr);	/* $001806 regs(&a0,&a2,a3,a5) ccout(NVC) */
void Op2_CruiseRight(uint32_t *handler, const u16 **script, Vm2Motion *mot);	/* $00180E regs(&a0,&a2,a3) ccout(NVC) */
void Op2_CruiseLeft(uint32_t *handler, const u16 **script, Vm2Motion *mot);	/* $001820 regs(&a0,&a2,a3) */
void Op2_CruiseRightNow(uint32_t *speed, const u16 **script, Vm2Motion *mot);	/* $001832 regs(&d0,&a2,a3) clobbers(a0) */
void Op2_ClearTargetSpeedX(const u16 **script, Vm2Motion *mot);	/* $00185E regs(&a2,a3) clobbers(a0) */
void Op2_ClearTargetSpeeds(const u16 **script, Vm2Motion *mot);	/* $001866 regs(&a2,a3) clobbers(a0) */
void Op2_ArcX(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint16_t *d3, const u16 **script, Vm2Motion *mot, const Vm2Block *vm);	/* $001890 regs(&d0,&d1,&d2,&d3,&a2,a3,a4) clobbers(a0) */
void Op2_BurstX(uint32_t *accel, uint32_t *vel, const u16 **script, Vm2Motion *mot, const Vm2Sprite *spr);	/* $0018B4 regs(&d0,&d1,&a2,a3,a5) clobbers(a0) */
void Op2_BurstY(uint32_t *accel, uint32_t *vel, const u16 **script, Vm2Motion *mot);	/* $0018DC regs(&d0,&d1,&a2,a3) clobbers(a0) */
void Op2_FaceTargetSpeedX(uint32_t *handler, const u16 **script, const Vm2Motion *mot, Vm2Sprite *spr);	/* $0018FA regs(&a0,&a2,a3,a5) */
void Op2_SeekScreenX(uint16_t *d0, uint16_t *tolerance, uint32_t *handler, const u16 **script, const Vm2Motion *mot, const Vm2Obj *self);	/* $001918 regs(&d0,&d1,&a0,&a2,a3,a6) ccout(NVC) */
void Op2_SeekScreenX4(uint16_t *d0, uint32_t *tolerance, uint32_t *handler, const u16 **script, const Vm2Motion *mot, const Vm2Obj *self);	/* $00191C regs(&d0,&d1,&a0,&a2,a3,a6) ccout(NVC) */
void Op2_SeekScreenXOrBranch(uint16_t *d0, uint16_t *tolerance, uint32_t *handler, const u16 **script, const Vm2Motion *mot, const Vm2Obj *self);	/* $001944 regs(&d0,&d1,&a0,&a2,a3,a6) ccout(NVC) */
void Op2_SeekScreenX8OrBranch(uint16_t *d0, uint32_t *tolerance, uint32_t *handler, const u16 **script, const Vm2Motion *mot, const Vm2Obj *self);	/* $001948 regs(&d0,&d1,&a0,&a2,a3,a6) ccout(NVC) */
void Op2_SeekScreenXRange(uint16_t *x, uint32_t *handler, const u16 **script, Vm2Motion *mot, const Vm2Obj *self);	/* $00197A regs(&d0,&a0,&a2,a3,a6) ccout(NVC) */
void Op2_SeekScreenXRangeOrBranch(uint16_t *x, uint32_t *handler, const u16 **script, Vm2Motion *mot, const Vm2Obj *self);	/* $00199A regs(&d0,&a0,&a2,a3,a6) ccout(NVC) */
void Op2_SeekWorldXRange(uint16_t *x, uint32_t *handler, const u16 **script, Vm2Motion *mot, const Vm2Obj *self);	/* $001A1A regs(&d0,&a0,&a2,a3,a6) ccout(NVC) */
void Op2_SeekScreenYOrBranch(uint16_t *d0, uint16_t *tolerance, uint32_t *handler, const u16 **script, const Vm2Motion *mot, const Vm2Obj *self);	/* $001A84 regs(&d0,&d1,&a0,&a2,a3,a6) ccout(NVC) */
void Op2_SeekScreenY8OrBranch(uint16_t *d0, uint32_t *tolerance, uint32_t *handler, const u16 **script, const Vm2Motion *mot, const Vm2Obj *self);	/* $001A88 regs(&d0,&d1,&a0,&a2,a3,a6) ccout(NVC) */
void Op2_SeekScreenYRange(uint16_t *y, uint32_t *handler, const u16 **script, Vm2Motion *mot, const Vm2Obj *self);	/* $001ABA regs(&d0,&a0,&a2,a3,a6) ccout(NVC) */
void Op2_SeekScreenYRangeOrBranch(uint16_t *y, uint32_t *handler, const u16 **script, Vm2Motion *mot, const Vm2Obj *self);	/* $001ADA regs(&d0,&a0,&a2,a3,a6) ccout(NVC) */
void Op2_SeekWorldYRange(uint16_t *y, uint32_t *handler, const u16 **script, Vm2Motion *mot, const Vm2Obj *self);	/* $001B5A regs(&d0,&a0,&a2,a3,a6) ccout(NVC) */
void Op2_SetAccelYForDist(uint16_t *speed, uint32_t *accel, uint32_t *series, uint16_t *time, uint32_t *handler, const u16 **script, Vm2Motion *mot, const Vm2Block *vm);	/* $001BB4 regs(&d0,&d1,&d2,&d3,&a0,&a2,a3,a4) */
void Op2_SetCruiseY(uint32_t *cruise, const u16 **script, Vm2Motion *mot, const Vm2Block *vm);	/* $001BBE regs(&d0,&a2,a3,a4) clobbers(a0) */
void Op2_CruiseUp(uint32_t *handler, const u16 **script, Vm2Motion *mot);	/* $001BFE regs(&a0,&a2,a3) */
void Op2_StopY(const u16 **script, Vm2Motion *mot);	/* $001C44 regs(&a2,a3) clobbers(a0) */
void Op2_SetJumpGravity(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint16_t *d3, const u16 **script, Vm2Motion *mot, const Vm2Block *vm);	/* $001C50 regs(&d0,&d1,&d2,&d3,&a2,a3,a4) clobbers(a0) */
void Op2_BranchIfNotRising(uint32_t *handler, const u16 **script, const Vm2Motion *mot);	/* $001C5C regs(&a0,&a2,a3) */
void Op2_BranchIfRising(uint32_t *handler, const u16 **script, const Vm2Motion *mot);	/* $001C6A regs(&a0,&a2,a3) */
void Op2_JumpAnimRandom(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint16_t *d3, const u16 **script, Vm2Motion *mot, const Vm2Block *vm, Vm2Sprite *spr);	/* $001C78 regs(&d0,&d1,&d2,&d3,&a2,a3,a4,a5) clobbers(a0) */
void Op2_JumpAnim(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint16_t *d3, const u16 **script, Vm2Motion *mot, const Vm2Block *vm, Vm2Sprite *spr);	/* $001C8C regs(&d0,&d1,&d2,&d3,&a2,a3,a4,a5) clobbers(a0) */
void Op2_Jump(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint16_t *d3, const u16 **script, Vm2Motion *mot, const Vm2Block *vm);	/* $001CBE regs(&d0,&d1,&d2,&d3,&a2,a3,a4) clobbers(a0) */
void Op2_FallAnim(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint16_t *d3, const u16 **script, Vm2Motion *mot, const Vm2Block *vm, Vm2Sprite *spr, Vm2Obj *self);	/* $001CD8 regs(&d0,&d1,&d2,&d3,&a2,a3,a4,a5,a6) clobbers(a0) */
void Op2_AnimByCruiseY(uint32_t *d0, const u16 **script, const Vm2Motion *mot, Vm2Sprite *spr);	/* $001D04 regs(&d0,&a2,a3,a5) clobbers(a0) */
void Op2_AnimByCruiseX(uint32_t *d0, const u16 **script, const Vm2Motion *mot, Vm2Sprite *spr);	/* $001D1C regs(&d0,&a2,a3,a5) clobbers(a0) */
void Op2_BounceY(uint32_t *d0, uint32_t *handler, const u16 **script, Vm2Motion *mot);	/* $001D34 regs(&d0,&a0,&a2,a3) ccout(NVC) */
void Op2_BounceYOrBranch(uint32_t *d0, uint32_t *handler, const u16 **script, Vm2Motion *mot);	/* $001D56 regs(&d0,&a0,&a2,a3) */
void Op2_SetTerrainBoxATouching(uint16_t *mask, const u16 **script, Vm2Motion *mot);	/* $001DD0 regs(&d0,&a2,a3) clobbers(a0) */
void Op2_SetTerrainBoxA(const u16 **script, Vm2Motion *mot);	/* $001DF8 regs(&a2,a3) clobbers(a0) */
void Op2_DisableTerrainBoxA(const u16 **script, Vm2Motion *mot);	/* $001E1C regs(&a2,a3) clobbers(a0) */
void Op2_SetTerrainBoxAMasks(const u16 **script, Vm2Motion *mot);	/* $001E24 regs(&a2,a3) clobbers(a0) */
void Op2_DisableTerrainBoxB(const u16 **script, Vm2Motion *mot);	/* $001E8A regs(&a2,a3) clobbers(a0) */
void Op2_SetTerrainBoxBMasks(const u16 **script, Vm2Motion *mot);	/* $001E92 regs(&a2,a3) clobbers(a0) */
void Op2_BranchOnFloor(uint32_t *handler, const u16 **script, const Vm2Motion *mot);	/* $001EFA regs(&a0,&a2,a3) */
void Op2_BranchOffFloor(uint32_t *handler, const u16 **script, const Vm2Motion *mot);	/* $001F76 regs(&a0,&a2,a3) */
void Op2_UseMainTerrain(const u16 **script, Vm2Motion *mot);	/* $001FC4 regs(&a2,a3) clobbers(a0) */
void Op2_UseAltTerrain(const u16 **script, Vm2Motion *mot);	/* $001FCC regs(&a2,a3) clobbers(a0) */
void Op2_ClearVar0(const u16 **script, Vm2Block *vm);	/* $001FD4 regs(&a2,a4) clobbers(a0) */
void Op2_SetVar0(const u16 **script, Vm2Block *vm);	/* $001FDC regs(&a2,a4) clobbers(a0) */
void Op2_OrVar0(uint16_t *bits, const u16 **script, Vm2Block *vm);	/* $001FE4 regs(&d0,&a2,a4) clobbers(a0) */
void Op2_AndVar0(uint16_t *mask, const u16 **script, Vm2Block *vm);	/* $001FEE regs(&d0,&a2,a4) clobbers(a0) */
void Op2_DecVar0(const u16 **script, Vm2Block *vm);	/* $00200A regs(&a2,a4) clobbers(a0) */
void Op2_BranchVar0Any(uint16_t *value, uint32_t *handler, const u16 **script, Vm2Block *vm);	/* $00202E regs(&d0,&a0,&a2,a4) */
void Op2_BranchVar0None(uint16_t *value, uint32_t *handler, const u16 **script, Vm2Block *vm);	/* $00203E regs(&d0,&a0,&a2,a4) */
void Op2_BranchVar0Eq(uint16_t *value, uint32_t *handler, const u16 **script, Vm2Block *vm);	/* $00204E regs(&d0,&a0,&a2,a4) */
void Op2_ClearVar1(const u16 **script, Vm2Block *vm);	/* $0020A6 regs(&a2,a4) clobbers(a0) */
void Op2_SetVar1(const u16 **script, Vm2Block *vm);	/* $0020AE regs(&a2,a4) clobbers(a0) */
void Op2_OrVar1(uint16_t *bits, const u16 **script, Vm2Block *vm);	/* $0020B6 regs(&d0,&a2,a4) clobbers(a0) */
void Op2_AndVar1(uint16_t *mask, uint32_t *handler, const u16 **script, Vm2Block *vm);	/* $0020C0 regs(&d0,&a0,&a2,a4) */
void Op2_XorVar1(uint16_t *bits, const u16 **script, Vm2Block *vm);	/* $0020CA regs(&d0,&a2,a4) clobbers(a0) */
void Op2_IncVar1(const u16 **script, Vm2Block *vm);	/* $0020D4 regs(&a2,a4) clobbers(a0) */
void Op2_BranchVar1Any(uint16_t *value, uint32_t *handler, const u16 **script, Vm2Block *vm);	/* $002100 regs(&d0,&a0,&a2,a4) */
void Op2_BranchVar1None(uint16_t *value, uint32_t *handler, const u16 **script, Vm2Block *vm);	/* $002110 regs(&d0,&a0,&a2,a4) */
void Op2_BranchVar1Eq(uint16_t *value, uint32_t *handler, const u16 **script, Vm2Block *vm);	/* $002120 regs(&d0,&a0,&a2,a4) */
void Op2_OrVar2(uint16_t *bits, const u16 **script, Vm2Block *vm);	/* $002188 regs(&d0,&a2,a4) clobbers(a0) */
void Op2_OrVar3(uint16_t *bits, const u16 **script, Vm2Block *vm);	/* $00225A regs(&d0,&a2,a4) clobbers(a0) */
void Op2_AndVar3(uint16_t *mask, const u16 **script, Vm2Block *vm);	/* $002264 regs(&d0,&a2,a4) clobbers(a0) */
void Op2_StartCoroutine0Rel(uint32_t *handler, const u16 **script, Vm2Block *vm);	/* $00233C regs(&a0,&a2,a4) */
void Op2_StartCoroutine0Abs(const u16 **script, Vm2Block *vm);	/* $002348 regs(&a2,a4) clobbers(a0) */
void Op2_StopCoroutine1(uint32_t *handler, const u16 **script, Vm2Block *vm);	/* $002364 regs(&a0,&a2,a4) */
void Op2_StartCoroutine1Rel(uint32_t *handler, const u16 **script, Vm2Block *vm);	/* $002384 regs(&a0,&a2,a4) */
void Op2_StopCoroutine2(const u16 **script, Vm2Block *vm);	/* $0023AC regs(&a2,a4) clobbers(a0) */
void Op2_StartCoroutine2Rel(uint32_t *handler, const u16 **script, Vm2Block *vm);	/* $0023CC regs(&a0,&a2,a4) */
void Op2_StepCoroutine2(uint32_t *handler, const u16 **script, Vm2Block *vm);	/* $0023EA regs(&a0,&a2,a4) */
void Op2_EnterCoroutine2(uint32_t *handler, const u16 **script, Vm2Block *vm);	/* $0023EC regs(&a0,&a2,a4) */
void Op2_StopCoroutine3(const u16 **script, Vm2Block *vm);	/* $0023F4 regs(&a2,a4) clobbers(a0) */
void Op2_StartCoroutine3Rel(const u16 **script, Vm2Block *vm);	/* $002414 regs(&a2,a4) clobbers(a0) */
void Op2_CallNativeOnChild(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6, uint32_t *d7, uint32_t *a0, uint32_t *a1, const u16 **script, uint32_t *a3, uint32_t *a4, uint32_t *a5, uint32_t *a6);	/* $0024BA regs(&d0,&d1,&d2,&d3,&d4,&d5,&d6,&d7,&a0,&a1,&a2,&a3,&a4,&a5,&a6) */
void Op2_KillChild(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6, uint32_t *d7, uint32_t *a0, const u16 **script, Vm2Obj *self);	/* $0024CE regs(&d0,&d1,&d2,&d3,&d4,&d5,&d6,&d7,&a0,&a2,a6) */
void Op2_BranchHasChild(uint16_t *field, uint32_t *a0, const u16 **script, const Vm2Obj *self);	/* $0024E6 regs(&d0,&a0,&a2,a6) */
void Op2_BranchNoChild(uint16_t *field, uint32_t *a0, const u16 **script, const Vm2Obj *self);	/* $0024F6 regs(&d0,&a0,&a2,a6) */
void Op2_SyncAndRunChild(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6, uint32_t *d7, uint32_t *a0, uint32_t a1, const u16 **script, uint32_t a3, uint32_t a4, uint32_t a5, Vm2Obj *self);	/* $002506 regs(&d0,&d1,&d2,&d3,&d4,&d5,&d6,&d7,&a0,a1,&a2,a3,a4,a5,a6) */
void Op2_RunChild(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6, uint32_t *d7, uint32_t *a0, uint32_t a1, const u16 **script, uint32_t a3, uint32_t a4, uint32_t a5, Vm2Obj *self);	/* $002512 regs(&d0,&d1,&d2,&d3,&d4,&d5,&d6,&d7,&a0,a1,&a2,a3,a4,a5,a6) */
/* a child object's handler run inline by Op2_RunChild / Op2_SyncAndRunChild (`jsr (a0)` at $2528; the corpus's one
 * target is VM2_RunObject $7D2, 413k calls): its convention; Z clear = the child ended (Obj_Destroy's d0 = -1) */
bool ChildHandlerFn(uint32_t code, uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6, uint32_t *d7, uint32_t *a0, uint32_t *a1, uint32_t *a2, uint32_t *a3, uint32_t *a4, uint32_t *a5, uint32_t a6);	/* type regs(&d0,&d1,&d2,&d3,&d4,&d5,&d6,&d7,&a0,&a1,&a2,&a3,&a4,&a5,a6)->Z */
void Op2_SyncChild(uint16_t *field, uint16_t *child, uint16_t *flags, uint16_t *pair, const u16 **script, const Vm2Sprite *spr, const Vm2Obj *self);	/* $002548 regs(&d0,&d1,&d2,&d3,&a2,a5,a6) clobbers(a0) */
void Op2_SetCounter0Rand(uint16_t *base, const u16 **script, Vm2Block *vm);	/* $002C42 regs(&d1,&a2,a4) clobbers(d0,a0) */
void Op2_LoopCounter0(uint32_t *handler, const u16 **script, Vm2Block *vm);	/* $002C54 regs(&a0,&a2,a4) */
void Op2_SetCounter1(const u16 **script, Vm2Block *vm);	/* $002C62 regs(&a2,a4) clobbers(a0) */
void Op2_SetCounter2(const u16 **script, Vm2Block *vm);	/* $002C8A regs(&a2,a4) clobbers(a0) */
void Op2_CallNativeRel(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6, uint32_t *d7, uint32_t *a0, uint32_t *a1, const u16 **script, uint32_t *a3, uint32_t *a4, uint32_t *a5, uint32_t *a6);	/* $002CB2 regs(&d0,&d1,&d2,&d3,&d4,&d5,&d6,&d7,&a0,&a1,&a2,&a3,&a4,&a5,&a6) */
void Op2_CallNative(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6, uint32_t *d7, uint32_t *a0, uint32_t *a1, const u16 **script, uint32_t *a3, uint32_t *a4, uint32_t *a5, uint32_t *a6);	/* $002CBC regs(&d0,&d1,&d2,&d3,&d4,&d5,&d6,&d7,&a0,&a1,&a2,&a3,&a4,&a5,&a6) ccout(NVC) */
/* native 68000 code a script calls (Op2_CallNative / CallNativeRel / CallNativeRelSaved: `jsr (a0)` at $2CB8,
 * $2CC0, $2CCC; hundreds of targets: Pal_FlashWhite, Fx_SpawnAt*, routines among the scripts): every register in
 * and out, the condition codes left for the next op */
void NativeFn(uint32_t code, uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6, uint32_t *d7, uint32_t *a0, uint32_t *a1, const u16 **script, uint32_t *a3, uint32_t *a4, uint32_t *a5, uint32_t *a6);	/* type regs(&d0,&d1,&d2,&d3,&d4,&d5,&d6,&d7,&a0,&a1,&a2,&a3,&a4,&a5,&a6) */
void Op2_CallNativeRelSaved(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6, uint32_t *d7, uint32_t *a0, uint32_t a1, const u16 **script, uint32_t a3, uint32_t a4, uint32_t a5, uint32_t a6);	/* $002CC4 regs(&d0,&d1,&d2,&d3,&d4,&d5,&d6,&d7,&a0,a1,&a2,a3,a4,a5,a6) ccout(NZVC) */
void Op2_GosubArgs(const u16 **script);	/* $002D00 regs(&a2) clobbers(a0) */
void Op2_GosubArgs3(const u16 **script);	/* $002D04 regs(&a2) clobbers(a0) */
void Op2_GosubArgs2(const u16 **script);	/* $002D08 regs(&a2) clobbers(a0) */
void Op2_Gosub(const u16 **script);	/* $002D10 regs(&a2) clobbers(a0) */
void Op2_GotoAbs(const u16 **script);	/* $002D1A regs(&a2) clobbers(a0) */
void Op2_Goto(uint32_t *handler, const u16 **script);	/* $002D22 regs(&a0,&a2) */
void Op2_CallNativeRelSwitch(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6, uint32_t *d7, uint32_t *a0, uint32_t *a1, const u16 **script, uint32_t *a3, uint32_t *a4, uint32_t *a5, uint32_t *a6);	/* $002D30 regs(&d0,&d1,&d2,&d3,&d4,&d5,&d6,&d7,&a0,&a1,&a2,&a3,&a4,&a5,&a6) */
void Op2_CallNativeSwitch(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6, uint32_t *d7, uint32_t *a0, uint32_t *a1, const u16 **script, uint32_t *a3, uint32_t *a4, uint32_t *a5, uint32_t *a6);	/* $002D40 regs(&d0,&d1,&d2,&d3,&d4,&d5,&d6,&d7,&a0,&a1,&a2,&a3,&a4,&a5,&a6) */
void Op2_ClearPlayerTarget(const u16 **script, Vm2Block *vm);	/* $002D4E regs(&a2,a4) clobbers(a0) */
void Op2_BalancePlayerTarget(uint16_t *d0, uint32_t *a1, const u16 **script, Vm2Block *vm);	/* $002D56 regs(&d0,&a1,&a2,a4) clobbers(a0) */
void Op2_SwapPlayerTarget(uint16_t *d0, uint32_t *handler, uint32_t *a1, const u16 **script, Vm2Block *vm);	/* $002DEA regs(&d0,&a0,&a1,&a2,a4) ccout(NVC) */
void Op2_KeepPlayerTarget(uint16_t *d0, uint32_t *handler, const u16 **script, Vm2Block *vm);	/* $002E2E regs(&d0,&a0,&a2,a4) */
void Op2_BranchNoPlayerTarget(uint16_t *d0, uint32_t *handler, const u16 **script, Vm2Block *vm);	/* $002E4E regs(&d0,&a0,&a2,a4) */
void Op2_KeepOrPickPlayerTarget(uint16_t *d0, const u16 **script, Vm2Block *vm);	/* $002E72 regs(&d0,&a2,a4) clobbers(a0) */
void Op2_BranchPlayerRight(uint16_t *d0, uint32_t *a0, const u16 **script, const Vm2Block *vm, const Vm2Obj *self);	/* $002E8E regs(&d0,&a0,&a2,a4,a6) */
void Op2_BranchPlayerLeft(uint16_t *d0, uint32_t *a0, const u16 **script, const Vm2Block *vm, const Vm2Obj *self);	/* $002EAA regs(&d0,&a0,&a2,a4,a6) */
void Op2_BranchPlayerBelow(uint16_t *d0, uint32_t *a0, const u16 **script, const Vm2Block *vm, const Vm2Obj *self);	/* $002EC6 regs(&d0,&a0,&a2,a4,a6) */
void Op2_BranchPlayerAbove(uint16_t *d0, uint32_t *a0, const u16 **script, const Vm2Block *vm, const Vm2Obj *self);	/* $002EE2 regs(&d0,&a0,&a2,a4,a6) */
void Op2_FaceCruiseToPlayerX(uint32_t *d0, const u16 **script, Vm2Motion *mot, const Vm2Block *vm, Vm2Sprite *spr, const Vm2Obj *self);	/* $002EFE regs(&d0,&a2,a3,a4,a5,a6) clobbers(a0) */
void Op2_HoldRangeOfPlayerXTurnBranch(uint32_t *d0, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint16_t *d5, uint16_t *d6, uint32_t *handler, const u16 **script, Vm2Motion *mot, const Vm2Block *vm, const Vm2Sprite *spr, const Vm2Obj *self);	/* $00300C regs(&d0,&d2,&d3,&d4,&d5,&d6,&a0,&a2,a3,a4,a5,a6) */
void Op2_FacePlayer(uint16_t *d0, uint32_t *a0, const u16 **script, const Vm2Block *vm, Vm2Sprite *spr, const Vm2Obj *self);	/* $003386 regs(&d0,&a0,&a2,a4,a5,a6) */
void Op2_FacePlayerTurnBranch(uint16_t *d0, uint32_t *a0, const u16 **script, const Vm2Block *vm, Vm2Sprite *spr, const Vm2Obj *self);	/* $0033AE regs(&d0,&a0,&a2,a4,a5,a6) */
void Op2_CruiseToPlayerY(uint16_t *d0, uint32_t *a0, const u16 **script, Vm2Motion *mot, const Vm2Block *vm, const Vm2Obj *self);	/* $0033E2 regs(&d0,&a0,&a2,a3,a4,a6) */
void Op2_SetPlayerWindowFromArgs(uint16_t *d0, uint16_t *d1, uint16_t *d2, uint16_t *d3, const u16 **script, Vm2Motion *mot);	/* $003490 regs(&d0,&d1,&d2,&d3,&a2,a3) clobbers(a0) */
void Op2_HoldPlayerWindow(uint16_t *d0, uint16_t *d1, uint16_t *d2, uint16_t *d3, uint16_t *d4, const u16 **script, Vm2Motion *mot, const Vm2Block *vm, const Vm2Obj *self);	/* $0034BE regs(&d0,&d1,&d2,&d3,&d4,&a2,a3,a4,a6) clobbers(a0) */
void Op2_BranchTouchBox_Enemies(uint32_t *d0, uint16_t left, uint16_t top, uint16_t right, uint16_t bottom, uint32_t *a0, const u16 **script);	/* $003768 regs(&d0,d4,d5,d6,d7,&a0,&a2) */
void Op2_SetAttackBoxes(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6, uint16_t *d7, uint32_t *a1, const u16 **script, Vm2Block *vm, Vm2Obj *self);	/* $00377A regs(&d0,&d1,&d2,&d3,&d4,&d5,&d6,&d7,&a1,&a2,a4,a6) clobbers(a0) */
void Op2_SetAttackBox(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6, uint32_t *d7, uint32_t *a1, const u16 **script, Vm2Block *vm, Vm2Obj *self);	/* $003784 regs(&d0,&d1,&d2,&d3,&d4,&d5,&d6,&d7,&a1,&a2,a4,a6) clobbers(a0) */
void Op2_ClearAttackBoxes(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6, uint32_t *d7, uint32_t *a0, uint32_t *a1, const u16 **script, Vm2Block *vm, Vm2Obj *self);	/* $00378E regs(&d0,&d1,&d2,&d3,&d4,&d5,&d6,&d7,&a0,&a1,&a2,a4,a6) ccout(NVC) */
void Op2_SetBodyBoxes(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6, uint16_t *d7, uint32_t *a1, const u16 **script, Vm2Block *vm, Vm2Obj *self);	/* $00381E regs(&d0,&d1,&d2,&d3,&d4,&d5,&d6,&d7,&a1,&a2,a4,a6) clobbers(a0) */
void Op2_SetBodyBox(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6, uint32_t *d7, uint32_t *a0, uint32_t *a1, const u16 **script, Vm2Block *vm, Vm2Obj *self);	/* $003828 regs(&d0,&d1,&d2,&d3,&d4,&d5,&d6,&d7,&a0,&a1,&a2,a4,a6) */
void Op2_ClearBodyBoxes(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6, uint32_t *d7, uint32_t *a0, uint32_t *a1, const u16 **script, Vm2Block *vm, Vm2Obj *self);	/* $003832 regs(&d0,&d1,&d2,&d3,&d4,&d5,&d6,&d7,&a0,&a1,&a2,a4,a6) */
void Op2_SetBodyBoxId(uint16_t *id, uint16_t *d7, uint32_t *a0, uint32_t *a1, const u16 **script, Vm2Block *vm);	/* $00383C regs(&d0,&d7,&a0,&a1,&a2,a4) */
void Op2_BranchStruck(uint32_t *d0, uint16_t *d7, uint32_t *a0, uint32_t *a1, const u16 **script, Vm2Block *vm);	/* $0038F0 regs(&d0,&d7,&a0,&a1,&a2,a4) */
void Op2_BranchNotStruck(uint32_t *d0, uint16_t *d7, uint32_t *a0, uint32_t *a1, const u16 **script, Vm2Block *vm);	/* $0038FE regs(&d0,&d7,&a0,&a1,&a2,a4) */
void Op2_BranchBoxNotStruck(uint32_t *d0, uint32_t *a0, uint32_t *a1, const u16 **script, Vm2Block *vm);	/* $003936 regs(&d0,&a0,&a1,&a2,a4) */
void Op2_SetEnergy1P2P(uint32_t *d0, uint16_t *energy, const u16 **script, Vm2Obj *self);	/* $0039AC regs(&d0,&d1,&a2,a6) clobbers(a0) */
void Op2_SetEnergy(uint32_t *d0, uint16_t *energy, const u16 **script, Vm2Obj *self);	/* $0039BA regs(&d0,&d1,&a2,a6) clobbers(a0) */
void Op2_BranchEnergyAtPct(uint32_t *d0, uint32_t *a0, const u16 **script, const Vm2Obj *self);	/* $0039E6 regs(&d0,&a0,&a2,a6) */
void Op2_TakeDamage(uint16_t *damage, uint32_t *a0, const u16 **script, Vm2Obj *self);	/* $0039FA regs(&d0,&a0,&a2,a6) */
void Op2_TakeKnockback(const u16 **script, Vm2Motion *mot);	/* $003A18 regs(&a2,a3) clobbers(a0) */
void Op2_TakeHit(uint16_t *damage, uint32_t *a0, const u16 **script, Vm2Motion *mot, Vm2Obj *self);	/* $003A26 regs(&d0,&a0,&a2,a3,a6) */
void Op2_SetBossEnergy(uint32_t *a0, const u16 **script, const Vm2Obj *self);	/* $003A4A regs(&a0,&a2,a6) */
void Op2_AddBossEnergy(uint32_t *a0, const u16 **script, const Vm2Obj *self);	/* $003A54 regs(&a0,&a2,a6) clobbers(d0) */
void Op2_SetVector0(const u16 **script, Vm2Obj *self);	/* $003A8E regs(&a2,a6) clobbers(a0) */
void Op2_SetVector1(const u16 **script, Vm2Obj *self);	/* $003A9A regs(&a2,a6) clobbers(a0) */
void Op2_SetVector0HereGoto(const u16 **script, Vm2Obj *self);	/* $003AC2 regs(&a2,a6) clobbers(a0) */
void Op2_SetVector2HereGotoArgs4(uint32_t *handler, const u16 **script, Vm2Obj *self);	/* $003AEA regs(&a0,&a2,a6) */
void Op2_SetVector2HereGotoArgs2(uint32_t *handler, const u16 **script, Vm2Obj *self);	/* $003AF2 regs(&a0,&a2,a6) */
void Op2_SetVector2HereGotoArg(uint32_t *handler, const u16 **script, Vm2Obj *self);	/* $003AF6 regs(&a0,&a2,a6) */
void Op2_SetVector2HereGoto(uint32_t *handler, const u16 **script, Vm2Obj *self);	/* $003AFA regs(&a0,&a2,a6) */
uint32_t Op2_GotoVector0(const Vm2Obj *self);	/* $003B06 regs(a6)->a2 clobbers(a0) */
uint32_t Op2_GotoVector1(const Vm2Obj *self);	/* $003B0E regs(a6)->a2 clobbers(a0) */
uint32_t Op2_GotoVector2(const Vm2Obj *self);	/* $003B16 regs(a6)->a2 clobbers(a0) */
void Op2_GosubVector0(const u16 **script, Vm2Obj *self);	/* $003B1E regs(&a2,a6) clobbers(a0) */
void Op2_GosubVector1(const u16 **script, Vm2Obj *self);	/* $003B28 regs(&a2,a6) clobbers(a0) */
void Op2_GosubVector2(const u16 **script, Vm2Obj *self);	/* $003B32 regs(&a2,a6) clobbers(a0) */
void Op2_AllocSprBlock(uint32_t *d0, uint32_t *a0, const u16 **script, uint32_t a4, uint32_t *a5, Vm2Obj *self);	/* $003B84 regs(&d0,&a0,&a2,a4,&a5,a6) ccout(Z) */
void Op2_LoadA5Handle(uint16_t *field, const u16 **script, uint32_t *a5, Vm2Obj *self);	/* $003BC2 regs(&d0,&a2,&a5,a6) clobbers(a0) */
void Op2_ResetSprBlock(uint32_t *handler, const u16 **script, Vm2Obj *self);	/* $003BCC regs(&a0,&a2,a6) clobbers(d0) */
void Op2_ResetA5(const u16 **script, uint32_t *a5, const Vm2Obj *self);	/* $003BF6 regs(&a2,&a5,a6) clobbers(a0) */
void Op2_ReleaseFrames(uint16_t *d0, uint16_t *d2, uint16_t *count, u16 **slot, const u16 **script, Vm2Sprite *spr);	/* $003BFE regs(&d0,&d2,&d7,&a1,&a2,a5) clobbers(a0) */
void Op2_ReleaseBlockFrames(uint16_t *d0, uint16_t *d2, uint16_t *count, uint32_t *handler, u16 **slot, const u16 **script, uint32_t a5, Vm2Obj *self);	/* $003C06 regs(&d0,&d2,&d7,&a0,&a1,&a2,a5,a6) */
void Op2_SetBlockAnim(uint32_t *anim, uint16_t *field, const u16 **script, Vm2Obj *self);	/* $003C18 regs(&d0,&d1,&a2,a6) clobbers(a0) */
void Op2_JitterPos(uint32_t *rand, const u16 **script, Vm2Obj *self);	/* $003C36 regs(&d0,&a2,a6) clobbers(a0) */
void Op2_PushPos(const u16 **script, const Vm2Obj *self);	/* $003C4E regs(&a2,a6) clobbers(a0) */
void Op2_PopPos(uint32_t *handler, const u16 **script, Vm2Obj *self);	/* $003C5A regs(&a0,&a2,a6) */
void Op2_AimDirAtPlayer(uint32_t *d0, uint32_t *d1, uint16_t *d2, uint16_t *d3, uint16_t *d4, uint32_t *a0, const u16 **script, const Vm2Block *vm, Vm2Sprite *spr, const Vm2Obj *self);	/* $003C66 regs(&d0,&d1,&d2,&d3,&d4,&a0,&a2,a4,a5,a6) */
void Op2_AnimGoto(const u16 **script);	/* $003E78 regs(&a2) clobbers(a0) */
void Op2_AnimStop(const u16 **script, Vm2Sprite *spr, dk::Flags *ccr);	/* $003E7E regs(&a2,a5,&ccr) ccout(Z) */
void Op2_SetAnimLoop(const u16 **script, Vm2Sprite *spr);	/* $003E88 regs(&a2,a5) clobbers(a0) */
void Op2_SetAnimLoopRand(uint32_t *rand, uint16_t *base, const u16 **script, Vm2Sprite *spr);	/* $003E90 regs(&d0,&d1,&a2,a5) clobbers(a0) */
void Op2_AnimLoop(uint32_t *handler, const u16 **script, Vm2Sprite *spr);	/* $003EA2 regs(&a0,&a2,a5) */
void Op2_AnimGotoYield(const u16 **script, Vm2Sprite *spr, dk::Flags *ccr);	/* $003EB0 regs(&a2,a5,&ccr) ccout(NZVC) */
void Op2_AnimYield(Vm2Sprite *spr, dk::Flags *ccr);	/* $003EB2 regs(a5,&ccr) ccout(NZVC) */
void Op2_SetAnimDelay(uint32_t *handler, const u16 **script, Vm2Sprite *spr);	/* $003EB8 regs(&a0,&a2,a5) */
void Op2_SetAnimDelayRand(uint32_t *rand, const u16 **script, Vm2Sprite *spr);	/* $003EC4 regs(&d0,&a2,a5) clobbers(a0) */
void Op2_AddAnimDelay(const u16 **script, Vm2Sprite *spr);	/* $003ED8 regs(&a2,a5) clobbers(d0,a0) */
void Op2_AnimHoldByVelY(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t d3, uint32_t d4, uint32_t d5, uint32_t d6, uint32_t *d7, uint32_t *a0, uint32_t *a1, uint32_t *a2, uint32_t a3, uint32_t a4, uint32_t a5, uint32_t a6, dk::Flags *ccr);	/* $003EE2 regs(&d0,&d1,&d2,d3,d4,d5,d6,&d7,&a0,&a1,&a2,a3,a4,a5,a6,&ccr) ccout(NZVC) */
void Op2_SwitchVelXBand(uint32_t *d0, uint16_t *d1, const u16 **script, const Vm2Motion *mot, const Vm2Sprite *spr);	/* $003F1C regs(&d0,&d1,&a2,a3,a5) clobbers(a0) */
void Op2_SwitchVelYBand(uint32_t *d0, uint16_t *d1, const u16 **script, const Vm2Motion *mot, const Vm2Sprite *spr);	/* $003F22 regs(&d0,&d1,&a2,a3,a5) clobbers(a0) */
void Op2_SwitchHeadingClamp(uint32_t *d0, const u16 **script, const Vm2Motion *mot);	/* $003F86 regs(&d0,&a2,a3) clobbers(a0) */
void Op2_SetShakeTimer(uint16_t *frames, const u16 **script, Vm2Sprite *spr);	/* $00430E regs(&d0,&a2,a5) clobbers(a0) */
void Op2_PlayMusic(uint32_t *song, uint32_t *d1, uint32_t *d7, uint32_t *handler, uint32_t a1, const u16 **script, uint32_t a3, uint32_t a4, uint32_t a5, uint32_t a6);	/* $00434A regs(&d0,&d1,&d7,&a0,a1,&a2,a3,a4,a5,a6) */
void Op2_BranchPopulationNz(uint32_t *handler, const u16 **script);	/* $00435E regs(&a0,&a2) */
void Op2_WaitPopBelowYield0(uint16_t *population, uint32_t *handler, const u16 **script, Vm2Block *vm);	/* $00438A regs(&d0,&a0,&a2,a4) */
void Op2_WaitPopBelowYield1(uint16_t *population, uint32_t *handler, const u16 **script, Vm2Block *vm);	/* $0043A0 regs(&d0,&a0,&a2,a4) */
void Op2_WaitA6Script(uint32_t *d0, uint32_t *a0, const u16 **script, uint32_t a4);	/* $0043F6 regs(&d0,&a0,&a2,a4) ccout(Z) */
void Op2_WaitA6ScriptYield0(uint32_t *handler, const u16 **script, Vm2Block *vm);	/* $004404 regs(&a0,&a2,a4) */
void Op2_WaitA6ScriptYield1(uint32_t *handler, const u16 **script, Vm2Block *vm);	/* $004418 regs(&a0,&a2,a4) */
void Op2_Draw(uint32_t *d0, uint32_t *a0, uint32_t *a1, const u16 **script, Vm2Sprite *spr, Vm2Obj *self);	/* $00448A regs(&d0,&a0,&a1,&a2,a5,a6) */
void Op2_Draw_RestorePos(uint32_t *handler, const u16 **script, Vm2Obj *self);	/* $0044E6 regs(&a0,&a2,a6) */
void Op2_DrawEnd(uint16_t *d0, uint16_t *d2, uint16_t *d7, uint32_t *a0, uint32_t *a1, uint32_t *a2, Vm2Sprite *spr, dk::Flags *ccr);	/* $0044F2 regs(&d0,&d2,&d7,&a0,&a1,&a2,a5,&ccr) ccout(NZVC) */
void Op2_DrawSpr(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint16_t *d4, uint16_t *d5, uint16_t *d6, uint16_t *d7, uint32_t *a1, const u16 **script, const Vm2Sprite *spr, const Vm2Obj *self);	/* $004524 regs(&d0,&d1,&d2,&d3,&d4,&d5,&d6,&d7,&a1,&a2,a5,a6) clobbers(a0) */
void Op2_DrawSprAttr(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint16_t *d4, uint16_t *d5, uint16_t *d6, uint16_t *d7, uint32_t *a1, const u16 **script, const Vm2Sprite *spr, const Vm2Obj *self);	/* $004546 regs(&d0,&d1,&d2,&d3,&d4,&d5,&d6,&d7,&a1,&a2,a5,a6) clobbers(a0) */
void Op2_DrawSprDepth(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint16_t *d4, uint16_t *d5, uint16_t *d6, uint16_t *d7, uint32_t *a0, uint32_t *a1, const u16 **script, const Vm2Sprite *spr, const Vm2Obj *self);	/* $004570 regs(&d0,&d1,&d2,&d3,&d4,&d5,&d6,&d7,&a0,&a1,&a2,a5,a6) ccout(NVC) */
void Op2_DrawSprAttrDepth(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint16_t *d4, uint16_t *d5, uint16_t *d6, uint16_t *d7, uint32_t *a0, uint32_t *a1, const u16 **script, const Vm2Sprite *spr, const Vm2Obj *self);	/* $004594 regs(&d0,&d1,&d2,&d3,&d4,&d5,&d6,&d7,&a0,&a1,&a2,a5,a6) ccout(NVC) */
void Op2_DrawSprOff(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint16_t *d4, uint16_t *d5, uint16_t *d6, uint16_t *d7, uint32_t *a1, const u16 **script, const Vm2Sprite *spr, const Vm2Obj *self);	/* $0045C0 regs(&d0,&d1,&d2,&d3,&d4,&d5,&d6,&d7,&a1,&a2,a5,a6) clobbers(a0) */
void Op2_DrawSprAttrOff(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint16_t *d4, uint16_t *d5, uint16_t *d6, uint16_t *d7, uint32_t *a1, const u16 **script, const Vm2Sprite *spr, const Vm2Obj *self);	/* $0045FE regs(&d0,&d1,&d2,&d3,&d4,&d5,&d6,&d7,&a1,&a2,a5,a6) clobbers(a0) */
void Op2_DrawSprFrontAttr(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint16_t *d4, uint16_t *d5, uint16_t *d6, uint32_t *a0, uint32_t *a1, const u16 **script, const Vm2Sprite *spr, const Vm2Obj *self);	/* $0046EA regs(&d0,&d1,&d2,&d3,&d4,&d5,&d6,&a0,&a1,&a2,a5,a6) */
void Op2_DrawSprFrontOff(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint16_t *d4, uint16_t *d5, uint16_t *d6, uint32_t *a0, uint32_t *a1, const u16 **script, const Vm2Sprite *spr, const Vm2Obj *self);	/* $004710 regs(&d0,&d1,&d2,&d3,&d4,&d5,&d6,&a0,&a1,&a2,a5,a6) */
void Op2_DrawSprBackAttr(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint16_t *d4, uint16_t *d5, uint16_t *d6, uint32_t *a0, uint32_t *a1, const u16 **script, const Vm2Sprite *spr, const Vm2Obj *self);	/* $0047AA regs(&d0,&d1,&d2,&d3,&d4,&d5,&d6,&a0,&a1,&a2,a5,a6) */
void Op2_DrawSprBackOff(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint16_t *d4, uint16_t *d5, uint16_t *d6, uint32_t *a0, uint32_t *a1, const u16 **script, const Vm2Sprite *spr, const Vm2Obj *self);	/* $0047D0 regs(&d0,&d1,&d2,&d3,&d4,&d5,&d6,&a0,&a1,&a2,a5,a6) */
void Op2_DrawSprBackAttrOff(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint16_t *d4, uint16_t *d5, uint16_t *d6, uint32_t *a0, uint32_t *a1, const u16 **script, const Vm2Sprite *spr, const Vm2Obj *self);	/* $00480A regs(&d0,&d1,&d2,&d3,&d4,&d5,&d6,&a0,&a1,&a2,a5,a6) */
void Op2_DrawSprAtDrawn(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint16_t *d4, uint16_t *d5, uint16_t *d6, uint16_t *d7, uint32_t *a0, uint32_t *a1, const u16 **script, const Vm2Sprite *spr, const Vm2Obj *self);	/* $00484C regs(&d0,&d1,&d2,&d3,&d4,&d5,&d6,&d7,&a0,&a1,&a2,a5,a6) */
void Op2_DrawSprAtDrawnAttrDepth(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint16_t *d4, uint16_t *d5, uint16_t *d6, uint16_t *d7, uint32_t *a0, uint32_t *a1, const u16 **script, const Vm2Sprite *spr, const Vm2Obj *self);	/* $0048BC regs(&d0,&d1,&d2,&d3,&d4,&d5,&d6,&d7,&a0,&a1,&a2,a5,a6) */
void Op2_DrawSprAtDrawnOff(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint16_t *d4, uint16_t *d5, uint16_t *d6, uint32_t *a0, uint32_t *a1, const u16 **script, const Vm2Sprite *spr, const Vm2Obj *self);	/* $0048E8 regs(&d0,&d1,&d2,&d3,&d4,&d5,&d6,&a0,&a1,&a2,a5,a6) clobbers(d7) */
void Op2_StepVelocity_Steer(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t d3, uint32_t d4, uint32_t d5, uint32_t d6, uint32_t *a0, const u16 **script, Vm2Motion *mot);	/* $004B8A regs(&d0,&d1,&d2,d3,d4,d5,d6,&a0,&a2,a3) ccout(NVC) */
void Op2_StepVelocity_Impulse(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *a0, const u16 **script, Vm2Motion *mot);	/* $004BBE regs(&d0,&d1,&d2,&a0,&a2,a3) ccout(NVC) */
void Op2_ClampMoveToArea(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *a0, const u16 **script, Vm2Obj *self);	/* $004C08 regs(&d0,&d1,&d2,&a0,&a2,a6) */
void Op2_ClampMoveToAreaX(uint32_t *d0, uint32_t *d1, uint32_t *a0, const u16 **script, Vm2Obj *self);	/* $004C1E regs(&d0,&d1,&a0,&a2,a6) */
void Op2_ClampMoveToAreaY(uint32_t *d0, uint32_t *d2, uint32_t *a0, const u16 **script, Vm2Obj *self);	/* $004C38 regs(&d0,&d2,&a0,&a2,a6) ccout(NVC) */
void Op2_ApplyMove(uint32_t d1, uint32_t d2, uint32_t *a0, const u16 **script, Vm2Obj *self);	/* $004C52 regs(d1,d2,&a0,&a2,a6) */
void Op2_SubLiftVelY(uint32_t *d2, const u16 **script);	/* $004D9C regs(&d2,&a2) clobbers(a0) */
void Op2_TestAttackBoxes(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6, uint32_t *d7, uint32_t *a0, uint32_t *a1, uint32_t *a2, uint32_t a3, uint32_t *a4, uint32_t a5, uint32_t *a6);	/* $004DA4 regs(&d0,&d1,&d2,&d3,&d4,&d5,&d6,&d7,&a0,&a1,&a2,a3,&a4,a5,&a6) */
void Op2_PlaceBodyBoxes(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6, uint32_t *d7, uint32_t *a0, uint32_t *a1, uint32_t *a2, uint32_t *a3, uint32_t *a4, uint32_t *a5, uint32_t *a6);	/* $004EDA regs(&d0,&d1,&d2,&d3,&d4,&d5,&d6,&d7,&a0,&a1,&a2,&a3,&a4,&a5,&a6) ccout(NVC) */
void Op2_PlaceBodyBox(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6, uint32_t *d7, uint32_t *a0, uint32_t *a1, const u16 **script, uint32_t *a3, uint32_t *a4, uint32_t *a5, uint32_t *a6);	/* $004F68 regs(&d0,&d1,&d2,&d3,&d4,&d5,&d6,&d7,&a0,&a1,&a2,&a3,&a4,&a5,&a6) ccout(NVC) */
void Op2_PlaceBodyBoxAtDrawn(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6, uint32_t *d7, uint32_t *a0, uint32_t *a1, const u16 **script, uint32_t *a3, uint32_t *a4, uint32_t *a5, uint32_t *a6);	/* $004F9E regs(&d0,&d1,&d2,&d3,&d4,&d5,&d6,&d7,&a0,&a1,&a2,&a3,&a4,&a5,&a6) ccout(NVC) */
void Op2_SetOscX(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint16_t *d3, uint32_t *a0, const u16 **script, const Vm2Block *vm, const Vm2Obj *self);	/* $004FD4 regs(&d0,&d1,&d2,&d3,&a0,&a2,a4,a6) ccout(NVC) */
void Op2_SetOscY(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint16_t *d3, uint32_t *a0, const u16 **script, const Vm2Block *vm, const Vm2Obj *self);	/* $00500C regs(&d0,&d1,&d2,&d3,&a0,&a2,a4,a6) ccout(NVC) */
void Op2_AllocOsc(uint32_t *d0, uint16_t *d1, uint32_t *a0, const u16 **script, Vm2Obj *self);	/* $005046 regs(&d0,&d1,&a0,&a2,a6) */
void Op2_StepOsc(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d4, uint32_t *d5, uint32_t *d6, uint32_t *d7, uint32_t *a0, const u16 **script, Vm2Obj *self);	/* $0050E2 regs(&d0,&d1,&d2,&d4,&d5,&d6,&d7,&a0,&a2,a6) */
void Op2_AddOsc(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d4, uint32_t *d5, uint32_t *d6, uint32_t *d7, uint32_t *a0, const u16 **script, Vm2Obj *self);	/* $0050E6 regs(&d0,&d1,&d2,&d4,&d5,&d6,&d7,&a0,&a2,a6) */
void Op2_FxSpawnRegsImpulse34(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6, uint32_t *d7, uint32_t *a0, uint32_t a1, const u16 **script, const Vm2Motion *mot, uint32_t a4, const Vm2Sprite *spr, Vm2Obj *self);	/* $00515E regs(&d0,&d1,&d2,&d3,&d4,&d5,&d6,&d7,&a0,a1,&a2,a3,a4,a5,a6) ccout(NZVC) */
void Op2_FxSpawnRegsImpulse(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6, uint32_t *d7, uint32_t *a0, uint32_t a1, const u16 **script, const Vm2Motion *mot, uint32_t a4, const Vm2Sprite *spr, Vm2Obj *self);	/* $005172 regs(&d0,&d1,&d2,&d3,&d4,&d5,&d6,&d7,&a0,a1,&a2,a3,a4,a5,a6) ccout(NZVC) */
void Op2_FxSpawnOff(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6, uint32_t *d7, uint32_t *a0, uint32_t a1, const u16 **script, const Vm2Motion *mot, uint32_t a4, const Vm2Sprite *spr, Vm2Obj *self);	/* $00517A regs(&d0,&d1,&d2,&d3,&d4,&d5,&d6,&d7,&a0,a1,&a2,a3,a4,a5,a6) ccout(NZVC) */
void Op2_FxSpawnHere(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6, uint32_t *d7, uint32_t *a0, uint32_t a1, const u16 **script, const Vm2Motion *mot, uint32_t a4, const Vm2Sprite *spr, Vm2Obj *self);	/* $00518C regs(&d0,&d1,&d2,&d3,&d4,&d5,&d6,&d7,&a0,a1,&a2,a3,a4,a5,a6) ccout(NZVC) */
void Op2_FxSpawnRegs(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6, uint32_t *d7, uint32_t *a0, uint32_t a1, const u16 **script, const Vm2Motion *mot, uint32_t a4, const Vm2Sprite *spr, Vm2Obj *self);	/* $005198 regs(&d0,&d1,&d2,&d3,&d4,&d5,&d6,&d7,&a0,a1,&a2,a3,a4,a5,a6) ccout(NZVC) */
void Op2_FxSpawnHereAbs(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6, uint32_t *d7, uint32_t *a0, uint32_t a1, const u16 **script, const Vm2Motion *mot, uint32_t a4, const Vm2Sprite *spr, Vm2Obj *self);	/* $005204 regs(&d0,&d1,&d2,&d3,&d4,&d5,&d6,&d7,&a0,a1,&a2,a3,a4,a5,a6) ccout(NZVC) */
void Op2_FxSpawnRegsAbs(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6, uint32_t *d7, uint32_t *a0, uint32_t a1, const u16 **script, const Vm2Motion *mot, uint32_t a4, const Vm2Sprite *spr, Vm2Obj *self);	/* $005210 regs(&d0,&d1,&d2,&d3,&d4,&d5,&d6,&d7,&a0,a1,&a2,a3,a4,a5,a6) ccout(NZVC) */
void Op2_FxPosScreenCentre(uint32_t *d2, uint32_t *d3, uint16_t *d4, uint16_t *d5, const u16 **script);	/* $00524C regs(&d2,&d3,&d4,&d5,&a2) clobbers(a0) */
void Op2_FxPosHere(uint32_t *d2, uint32_t *d3, uint16_t *d4, uint16_t *d5, const u16 **script, const Vm2Obj *self);	/* $005264 regs(&d2,&d3,&d4,&d5,&a2,a6) clobbers(a0) */
void Op2_FxPosAnchor(uint32_t *d2, uint32_t *d3, uint16_t *d4, uint16_t *d5, const u16 **script, const Vm2Obj *self);	/* $005274 regs(&d2,&d3,&d4,&d5,&a2,a6) clobbers(a0) */
void Op2_FxPosDrawn(uint32_t *d2, uint32_t *d3, uint16_t *d4, uint16_t *d5, const u16 **script, const Vm2Obj *self);	/* $005284 regs(&d2,&d3,&d4,&d5,&a2,a6) clobbers(a0) */
void Op2_FxPosAddB(uint16_t *d1, uint16_t *d4, uint16_t *d5, const u16 **script);	/* $005294 regs(&d1,&d4,&d5,&a2) clobbers(d0,a0) */
void Op2_FxPosAddW(uint16_t *d4, uint16_t *d5, const u16 **script);	/* $0052A4 regs(&d4,&d5,&a2) clobbers(a0) */
void Op2_FxPosAddFlip(uint16_t *d1, uint16_t *d4, uint16_t *d5, const u16 **script, const Vm2Sprite *spr);	/* $0052AC regs(&d1,&d4,&d5,&a2,a5) clobbers(d0,a0) */
void Op2_FxHeadingByFacing(uint32_t *d3, const u16 **script, const Vm2Sprite *spr);	/* $00530E regs(&d3,&a2,a5) clobbers(a0) */
void Op2_FxHeadingSnap(uint32_t *d0, uint32_t *d3, const u16 **script, const Vm2Motion *mot);	/* $005320 regs(&d0,&d3,&a2,a3) clobbers(a0) */
void Op2_FxHeadingSnapClamp(uint32_t *d0, uint32_t *d3, const u16 **script, const Vm2Motion *mot);	/* $005340 regs(&d0,&d3,&a2,a3) clobbers(a0) */
void Op2_SkipGoto(uint32_t *handler, const u16 **script);	/* $002D20 regs(&a0,&a2) */
uint32_t Op2_YieldAgain(void);	/* $000826 regs()->d0 tst(d0.l) */
void Op2_KillOffScrY(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6, uint32_t *d7, uint32_t *handler, uint32_t *a1, const u16 **script, uint32_t a4, uint32_t a5, const Vm2Obj *self);	/* $00094A regs(&d0,&d1,&d2,&d3,&d4,&d5,&d6,&d7,&a0,&a1,&a2,a4,a5,a6) tst(d0.l) */
void Op2_TickTimer0BranchNoZeroTest(uint16_t *tick, uint32_t *handler, const u16 **script, Vm2Block *vm);	/* $000EC8 regs(&d0,&a0,&a2,a4) */
void Op2_TickTimer1BranchNoZeroTest(uint16_t *tick, uint32_t *handler, const u16 **script, Vm2Block *vm);	/* $000FA2 regs(&d0,&a0,&a2,a4) */
void Op2_LoopTimer1NoZeroTest(uint16_t *tick, uint32_t *handler, const u16 **script, Vm2Block *vm);	/* $001008 regs(&d0,&a0,&a2,a4) */
void Op2_TickTimer2BranchNoZeroTest(uint16_t *tick, uint32_t *handler, const u16 **script, Vm2Block *vm);	/* $00107C regs(&d0,&a0,&a2,a4) */
void Op2_LoopTimer3NoZeroTest(uint16_t *tick, uint32_t *handler, const u16 **script, Vm2Block *vm);	/* $0011BC regs(&d0,&a0,&a2,a4) */
void Op2_SaveX(const u16 **script, Vm2Obj *self);	/* $001288 regs(&a2,a6) clobbers(a0) */
void Op2_SetY(const u16 **script, Vm2Obj *self);	/* $001622 regs(&a2,a6) clobbers(a0) */
void Op2_MoveY(uint16_t *delta, const u16 **script, Vm2Obj *self);	/* $001640 regs(&d0,&a2,a6) clobbers(a0) */
void Op2_PlaceOnScreenKeepVel(uint32_t *x, uint32_t *y, uint32_t *handler, const u16 **script, Vm2Obj *self);	/* $001678 regs(&d0,&d1,&a0,&a2,a6) */
void Op2_PlaceYOnScreenKeepVel(uint32_t *y, const u16 **script, Vm2Obj *self);	/* $0016B8 regs(&d0,&a2,a6) clobbers(a0) */
void Op2_SetTargetY(const u16 **script, Vm2Motion *mot);	/* $0016DE regs(&a2,a3) clobbers(a0) */
void Op2_EnterCoroutine1(uint32_t *handler, const u16 **script, Vm2Block *vm);	/* $0023A4 regs(&a0,&a2,a4) */
void Op2_CruiseTargets(const u16 **script, Vm2Motion *mot);	/* $0017F6 regs(&a2,a3) clobbers(a0) */
void Op2_CruiseTargetX(const u16 **script, Vm2Motion *mot);	/* $0017FC regs(&a2,a3) clobbers(a0) */
void Op2_EnableTerrainBoxA(const u16 **script, Vm2Motion *mot);	/* $001E14 regs(&a2,a3) clobbers(a0) */
void Op2_SetTerrainBoxBKeepResults(const u16 **script, Vm2Motion *mot);	/* $001E72 regs(&a2,a3) clobbers(a0) */
void Op2_EnableTerrainBoxB(const u16 **script, Vm2Motion *mot);	/* $001E82 regs(&a2,a3) clobbers(a0) */
void Op2_FallAnimInPlace(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint16_t *d3, const u16 **script, Vm2Motion *mot, const Vm2Block *vm, Vm2Sprite *spr, Vm2Obj *self);	/* $001CDC regs(&d0,&d1,&d2,&d3,&a2,a3,a4,a5,a6) clobbers(a0) */
void Op2_PlaceBodyBoxes_Place(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6, uint32_t *d7, uint32_t *a0, uint32_t *a1, uint32_t *a2, uint32_t *a3, uint32_t *a4, uint32_t *a5, uint32_t *a6);	/* $004EF6 regs(&d0,&d1,&d2,&d3,&d4,&d5,&d6,&d7,&a0,&a1,&a2,&a3,&a4,&a5,&a6) ccout(NZVC) */
void Op2_PlaceBodyBoxes_Next(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6, uint32_t *d7, uint32_t *a0, uint32_t *a1, uint32_t *a2, uint32_t *a3, uint32_t *a4, uint32_t *a5, uint32_t *a6);	/* $004EF4 regs(&d0,&d1,&d2,&d3,&d4,&d5,&d6,&d7,&a0,&a1,&a2,&a3,&a4,&a5,&a6) ccout(NZVC) */
