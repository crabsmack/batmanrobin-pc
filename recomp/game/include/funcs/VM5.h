/* funcs/VM5.h - prototypes of the VM5 module: one line per function, its register
 * convention in the tag (dkit genesis recomp bridge). First written from the measured
 * contracts (dkit disasm regs); edited by the C's authors from then on. */
#pragma once
void VM5_Yield(const u16 *script, Vm5Wave *self);	/* $006A68 regs(a5,a6) ccout(NZVC) */
uint16_t VM5_CloseGroup(uint32_t a6);	/* $006F56 regs(a6)->d0 clobbers(a0) */
void VM5_SpawnEntry(uint32_t *d0, uint32_t *d1, uint32_t d2, uint32_t d3, uint32_t *d4, uint32_t *d5, uint32_t *d6, uint32_t *d7, uint32_t *a0, uint32_t *a1, uint32_t *a2, uint32_t *a3, uint32_t *a4, uint32_t *a5, uint32_t a6);	/* $006FEA regs(&d0,&d1,d2,d3,&d4,&d5,&d6,&d7,&a0,&a1,&a2,&a3,&a4,&a5,a6) */
void VM5_RunObject(uint32_t *d0, uint32_t *a0, uint32_t *a1, uint32_t *a5, Vm5Wave *self);	/* $006A14 regs(&d0,&a0,&a1,&a5,a6) */
void VM5_Next(uint32_t *a0, uint32_t *a5);	/* $006A56 regs(&a0,&a5) */
void VM5_EndWave(uint16_t *d0, uint32_t *a0, uint32_t *a1, Vm5Wave *self, dk::Flags *ccr);	/* $006A76 regs(&d0,&a0,&a1,a6,&ccr) */
uint16_t VM5_AfterSpawn(Vm5Wave *wave, dk::Flags *ccr);	/* $007052 regs(a6,&ccr)->d0 ccout(NZVC) */
/* a spawn entry's spawner (`jsr (a0)` in VM5_SpawnEntry: the Obj_SpawnEnemy wrappers, e.g.
 * Enemy_Clown_SpawnRunIn $1B6FA): d4 / d5 = the position, the new object back in a6 and Z set on
 * success; every register the entry hands it, in and out. */
void Vm5SpawnerFn(uint32_t code, uint32_t *d0, uint32_t *d1, uint32_t d2, uint32_t d3, uint32_t *d4, uint32_t *d5, uint32_t *d6, uint32_t *d7, uint32_t *a0, uint32_t *a1, uint32_t *a2, uint32_t *a3, uint32_t *a4, uint32_t *a5, uint32_t *a6, dk::Flags *ccr);	/* type regs(&d0,&d1,d2,d3,&d4,&d5,&d6,&d7,&a0,&a1,&a2,&a3,&a4,&a5,&a6,&ccr) */
