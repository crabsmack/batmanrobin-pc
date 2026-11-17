/* funcs/Obj.h - prototypes of the Obj module: one line per function, its register
 * convention in the tag (dkit genesis recomp bridge). First written from the measured
 * contracts (dkit disasm regs); edited by the C's authors from then on. */
#pragma once
void Obj_Destroy(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6, uint32_t *d7, uint32_t *a0, uint32_t *a1, uint32_t *a2, uint32_t a4, uint32_t a5, uint32_t a6);	/* $000980 regs(&d0,&d1,&d2,&d3,&d4,&d5,&d6,&d7,&a0,&a1,&a2,a4,a5,a6) ccout(NZVC) */
void Obj_ReleaseFrames(uint16_t *d0, uint16_t *d2, uint16_t *count, u16 **slot, Vm2Sprite *spr);	/* $0009C4 regs(&d0,&d2,&d7,&a1,a5) clobbers(a0) */
void Obj_SyncChild(uint16_t *field, uint16_t *child, uint16_t *flags, uint16_t *pair, const u16 **script, const Vm2Sprite *spr, const Vm2Obj *self);	/* $002550 regs(&d0,&d1,&d2,&d3,&a2,a5,a6) clobbers(a0) */
void Obj_SetAttackBoxes(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6, uint16_t *count, uint32_t *a1, uint32_t *a2, uint32_t a4, uint32_t a6, dk::Flags *ccr);	/* $003798 regs(&d0,&d1,&d2,&d3,&d4,&d5,&d6,&d7,&a1,&a2,a4,a6,&ccr) ccout(NVC) clobbers(a0) */
void Obj_SetBodyBoxes(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6, uint16_t *count, uint32_t *a1, uint32_t *a2, uint32_t a4, uint32_t a6);	/* $00385A regs(&d0,&d1,&d2,&d3,&d4,&d5,&d6,&d7,&a1,&a2,a4,a6) clobbers(a0) */
void Obj_TestStruck(uint32_t *d0, uint16_t *d7, uint32_t *a1, Vm2Block *vm);	/* $003960 regs(&d0,&d7,&a1,a4) tst(d0.l) ccout(Z) clobbers(a0) */
void Obj_TestBoxStruck(uint32_t *d0, uint32_t *a1, const u16 **script, Vm2Block *vm);	/* $00398A regs(&d0,&a1,&a2,a4) tst(d0.l) ccout(Z) clobbers(a0) */
void Obj_RunList(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6, uint32_t *d7, uint32_t *a0, uint32_t *a1, uint32_t *a2, uint32_t *a3, uint32_t *a4, uint32_t *a5, uint32_t *a6);	/* $0087B6 regs(&d0,&d1,&d2,&d3,&d4,&d5,&d6,&d7,&a0,&a1,&a2,&a3,&a4,&a5,&a6) */
void Obj_ListEnd(void);	/* $0088BE regs() */
void Obj_InitList(void);	/* $0088C2 regs() clobbers(a0,a1) */
uint32_t Obj_LinkAtEnd(uint32_t obj, uint32_t a2);	/* $0088DA regs(a0,a2)->a1 */
uint32_t Obj_NewMultiFail(uint8_t blocks);	/* $00890A regs(d7)->d0 tst(d0.l) ccout(NZVC) */
void Obj_NewMulti(uint32_t *d0, uint16_t *d1, uint16_t *blocks, uint32_t *handler_last, uint32_t *a1, uint32_t *a2, uint32_t *obj);	/* $008912 regs(&d0,&d1,&d7,&a0,&a1,&a2,&a6) tst(d0.l) ccout(NZVC) */
void Obj_NewMulti_HandleRun(uint32_t *d0, uint16_t d1, uint32_t *a1, uint32_t *a2);	/* $00894C regs(&d0,d1,&a1,&a2) tst(d0.l) ccout(NZVC) */
void Obj_NewMulti_ClearRun(uint32_t *d0, uint32_t *a1, uint32_t a2);	/* $00896C regs(&d0,&a1,a2) ccout(NZVC) */
void Obj_NewMulti_Clear4(uint32_t *d0, uint32_t *a1, uint32_t a2);	/* $008972 regs(&d0,&a1,a2) tst(d0.l) ccout(NZVC) */
void Obj_NewClear(uint32_t *result, uint32_t handler, uint32_t *obj);	/* $0089CA regs(&d0,a0,&a6) tst(d0.l) ccout(Z) clobbers(a0,a1) */
void Obj_New(uint32_t *result, uint32_t *handler_last, uint32_t *tail, uint32_t *obj);	/* $008A08 regs(&d0,&a0,&a1,&a6) tst(d0.l) ccout(NZVC) */
void Obj_FreeWithBlocks(uint32_t *d0, uint32_t *count, uint32_t *handles, uint32_t *last, uint32_t obj, dk::Flags *ccr);	/* $008A48 regs(&d0,&d1,&a0,&a1,a6,&ccr) ccout(NZVC) */
void Obj_UnlinkFree(uint32_t *prev, uint32_t *next, uint32_t obj, dk::Flags *ccr);	/* $008ABE regs(&a0,&a1,a6,&ccr) ccout(NZVC) */
void Obj_SpawnEnemy(uint32_t *d0, uint16_t *d1, uint16_t d4, uint16_t d5, uint32_t *d7, uint32_t *a0, uint32_t a2, uint32_t *a3, uint32_t *a4, uint32_t *a5, uint32_t *a6);	/* $008DB6 regs(&d0,&d1,d4,d5,&d7,&a0,a2,&a3,&a4,&a5,&a6) ccout(NZVC) */
void Obj_SpawnEnemyOrQuit(uint32_t *d0, uint16_t *d1, uint16_t d4, uint16_t d5, uint32_t *d7, uint32_t *a0, uint32_t a2, uint32_t *a3, uint32_t *a4, uint32_t *a5, uint32_t *a6);	/* $008DD0 regs(&d0,&d1,d4,d5,&d7,&a0,a2,&a3,&a4,&a5,&a6) ccout(NZVC) */
bool Obj_SpawnEnemyNOrQuit(uint32_t *d0, uint16_t *d1, uint16_t d4, uint16_t d5, uint16_t *d7, uint32_t *a0, uint32_t a2, uint32_t *a3, uint32_t *a4, uint32_t *a5, uint32_t *a6);	/* $008DD2 regs(&d0,&d1,d4,d5,&d7,&a0,a2,&a3,&a4,&a5,&a6)->Z ccout(NZVC) */
void Obj_SpawnBoss(uint32_t *d0, uint16_t *d1, uint16_t d4, uint16_t d5, uint32_t *d7, uint32_t *a0, uint32_t a2, uint32_t *a3, uint32_t *a4, uint32_t *a5, uint32_t *a6);	/* $008E58 regs(&d0,&d1,d4,d5,&d7,&a0,a2,&a3,&a4,&a5,&a6) ccout(NZVC) */
bool Obj_SpawnBossOrQuit(uint32_t *d0, uint16_t *d1, uint16_t d4, uint16_t d5, uint32_t *d7, uint32_t *a0, uint32_t a2, uint32_t *a3, uint32_t *a4, uint32_t *a5, uint32_t *a6);	/* $008E72 regs(&d0,&d1,d4,d5,&d7,&a0,a2,&a3,&a4,&a5,&a6)->Z ccout(NZVC) */
void Obj_SpawnScriptedOrQuit(uint32_t *d0, uint16_t *d1, uint16_t d4, uint16_t d5, uint32_t *d7, uint32_t *a0, uint32_t *a3, uint32_t *a4, uint32_t *a5, uint32_t *a6);	/* $008EFA regs(&d0,&d1,d4,d5,&d7,&a0,&a3,&a4,&a5,&a6) ccout(NZVC) */
void Obj_SpawnScripted(uint32_t *d0, uint16_t *d1, uint16_t d4, uint16_t d5, uint32_t *d7, uint32_t *a0, uint32_t *a3, uint32_t *a4, uint32_t *a5, uint32_t *a6);	/* $008F08 regs(&d0,&d1,d4,d5,&d7,&a0,&a3,&a4,&a5,&a6) ccout(NZVC) */
void Obj_Spawn(uint32_t *d0, uint16_t *d1, uint16_t d4, uint16_t d5, uint16_t *d7, uint32_t *a0, uint32_t *a3, uint32_t *a4, uint32_t *a5, uint32_t *a6);	/* $008F0E regs(&d0,&d1,d4,d5,&d7,&a0,&a3,&a4,&a5,&a6) ccout(NZVC) */
void Obj_SpawnScriptedUnlinked(uint32_t *d0, uint16_t *d1, uint16_t d4, uint16_t d5, uint32_t *d7, uint32_t *a0, uint32_t *a3, uint32_t *a4, uint32_t *a5, uint32_t *a6);	/* $008F1E regs(&d0,&d1,d4,d5,&d7,&a0,&a3,&a4,&a5,&a6) ccout(Z) */
void Obj_SpawnUnlinked(uint32_t *d0, uint16_t *d1, uint16_t d4, uint16_t d5, uint16_t *d7, uint32_t *a0, uint32_t *a3, uint32_t *a4, uint32_t *a5, uint32_t *a6);	/* $008F24 regs(&d0,&d1,d4,d5,&d7,&a0,&a3,&a4,&a5,&a6) ccout(NZVC) */
void Obj_InitSpawned(uint32_t *d0, uint16_t d4, uint16_t d5, uint32_t *a0, uint32_t *a3, uint32_t *a4, uint32_t *a5, uint32_t a6, dk::Flags *ccr);	/* $008F4E regs(&d0,d4,d5,&a0,&a3,&a4,&a5,a6,&ccr) ccout(NZVC) */
void Obj_DestroyOther(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6, uint32_t *d7, uint32_t a0, uint32_t a2);	/* $000966 regs(&d0,&d1,&d2,&d3,&d4,&d5,&d6,&d7,a0,a2) clobbers(a0) */
void Obj_NewMultiUnlinked(uint32_t *d0, uint16_t *d1, uint16_t *blocks, uint32_t *handler, uint32_t *obj);	/* $008984 regs(&d0,&d1,&d7,&a0,&a6) tst(d0.l) ccout(NZVC) clobbers(a1,a2) */
void Obj_UnlinkFreeWithBlocks(uint32_t *d0, uint32_t *d1, uint32_t *prev, uint32_t *next, uint32_t obj);	/* $008A3A regs(&d0,&d1,&a0,&a1,a6) ccout(NZVC) */
void Obj_SpawnEnemyN(uint32_t *d0, uint16_t *d1, uint16_t d4, uint16_t d5, uint16_t *d7, uint32_t *a0, uint32_t a2, uint32_t *a3, uint32_t *a4, uint32_t *a5, uint32_t *a6);	/* $008DB8 regs(&d0,&d1,d4,d5,&d7,&a0,a2,&a3,&a4,&a5,&a6) ccout(Z) */
void Obj_SpawnEnemyUnlinked(uint16_t d4, uint16_t d5, uint32_t *d7, uint32_t a2, uint32_t a4, uint32_t *a6);	/* $008DEC regs(d4,d5,&d7,a2,a4,&a6) ccout(Z) clobbers(d0,d1,a0,a3,a4,a5) */
bool Obj_SpawnEnemyUnlinkedOrQuit(uint32_t *d0, uint16_t *d1, uint16_t d4, uint16_t d5, uint32_t *d7, uint32_t *a0, uint32_t a2, uint32_t *a3, uint32_t *a4, uint32_t *a5, uint32_t *a6);	/* $008E06 regs(&d0,&d1,d4,d5,&d7,&a0,a2,&a3,&a4,&a5,&a6)->Z ccout(NZVC) */
void Obj_SpawnBossN(uint32_t *d0, uint16_t *d1, uint16_t d4, uint16_t d5, uint16_t *d7, uint32_t *a0, uint32_t a2, uint32_t *a3, uint32_t *a4, uint32_t *a5, uint32_t *a6);	/* $008E5A regs(&d0,&d1,d4,d5,&d7,&a0,a2,&a3,&a4,&a5,&a6) ccout(Z) */
void Obj_SpawnBossUnlinked(uint32_t *d0, uint16_t *d1, uint16_t d4, uint16_t d5, uint32_t *d7, uint32_t a2, uint32_t *a4, uint32_t *a6);	/* $008E8E regs(&d0,&d1,d4,d5,&d7,a2,&a4,&a6) ccout(Z) clobbers(a0,a3,a5) */
void Obj_DestroyLoaded(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6, uint32_t *d7, uint32_t *a0, uint32_t *a1, uint32_t *a2, uint32_t *a3, uint32_t *a4, uint32_t *a5, uint32_t *a6);	/* $000978 regs(&d0,&d1,&d2,&d3,&d4,&d5,&d6,&d7,&a0,&a1,&a2,&a3,&a4,&a5,&a6) ccout(NZVC) */
uint32_t Obj_SetStruckBox(HitBox *box);	/* $003982 regs(a0)->d0 tst(d0.l) ccout(NZVC) */
void Obj_NewMulti_FillHandles(uint32_t *d0, uint16_t *d1, uint16_t *blocks, uint32_t *a0, uint32_t *a1, uint32_t *a2, uint32_t *a6);	/* $00893C regs(&d0,&d1,&d7,&a0,&a1,&a2,&a6) tst(d0.l) ccout(NZVC) */
void Obj_NewMulti_Clear5(uint32_t *d0, uint32_t *a1, uint32_t a2);	/* $008974 regs(&d0,&a1,a2) tst(d0.l) ccout(NZVC) */
void Obj_NewMulti_Clear6(uint32_t *d0, uint32_t *a1, uint32_t a2);	/* $008976 regs(&d0,&a1,a2) tst(d0.l) ccout(NZVC) */
void Obj_NewMulti_Clear7(uint32_t *d0, uint32_t *a1, uint32_t a2);	/* $008978 regs(&d0,&a1,a2) tst(d0.l) ccout(NZVC) */
void Obj_NewMulti_Clear8(uint32_t *d0, uint32_t *a1, uint32_t a2);	/* $00897A regs(&d0,&a1,a2) tst(d0.l) ccout(NZVC) */
void ObjHandlerFn(uint32_t code, uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6, uint32_t *d7, uint32_t *a0, uint32_t *a1, uint32_t *a2, uint32_t *a3, uint32_t *a4, uint32_t *a5, uint32_t *a6);	/* type regs(&d0,&d1,&d2,&d3,&d4,&d5,&d6,&d7,&a0,&a1,&a2,&a3,&a4,&a5,&a6) */
void Obj_NewMultiAfter(uint32_t *d0, uint16_t *d1, uint16_t *blocks, uint32_t *handler_last, uint32_t *a1, uint32_t *a2, uint32_t *obj);	/* $00899A regs(&d0,&d1,&d7,&a0,&a1,&a2,&a6) tst(d0.l) ccout(NZVC) */
void Obj_SpawnEnemyAfter(uint32_t *d0, uint16_t *d1, uint16_t d4, uint16_t d5, uint32_t *d7, uint32_t *a0, uint32_t a2, uint32_t *a3, uint32_t *a4, uint32_t *a5, uint32_t *a6);	/* $008E22 regs(&d0,&d1,d4,d5,&d7,&a0,a2,&a3,&a4,&a5,&a6) ccout(NZVC) */
void Obj_SpawnAfter(uint32_t *d0, uint16_t *d1, uint16_t d4, uint16_t d5, uint16_t *d7, uint32_t *a0, uint32_t *a3, uint32_t *a4, uint32_t *a5, uint32_t *a6);	/* $008F3C regs(&d0,&d1,d4,d5,&d7,&a0,&a3,&a4,&a5,&a6) ccout(NZVC) */
