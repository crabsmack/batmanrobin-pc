/* funcs/Spawn.h - prototypes of the Spawn module: one line per function, its register
 * convention in the tag (dkit genesis recomp bridge). First written from the measured
 * contracts (dkit disasm regs); edited by the C's authors from then on. */
#pragma once
void Spawn_SetTimer0FromEntry(const u16 **entry, Vm2Block *vm, dk::Flags *ccr);	/* $027C92 regs(&a1,a4,&ccr) ccout(Z) clobbers(d0) */
void Spawn_AtScreenCentre(uint32_t *d0, uint32_t *d1, uint32_t *d4, uint32_t *d5, uint32_t *d7, uint32_t *a1, uint32_t *a2, uint32_t a3, uint32_t a4, uint32_t a5, uint32_t a6);	/* $008D12 regs(&d0,&d1,&d4,&d5,&d7,&a1,&a2,a3,a4,a5,a6) ccout(NVC) clobbers(a0) */
void Spawn_AtCameraOfs(uint32_t *d0, uint32_t *d1, uint32_t *d4, uint32_t *d5, uint32_t *d7, uint32_t *a1, uint32_t *a2, uint32_t a3, uint32_t a4, uint32_t a5, uint32_t a6);	/* $008D34 regs(&d0,&d1,&d4,&d5,&d7,&a1,&a2,a3,a4,a5,a6) ccout(NVC) clobbers(a0) */
void Spawn_AtAnchor(uint32_t *d0, uint32_t *d1, uint32_t *d4, uint32_t *d5, uint32_t *d7, uint32_t *a1, uint32_t *a2, uint32_t a3, uint32_t a4, uint32_t a5, uint32_t a6);	/* $008D52 regs(&d0,&d1,&d4,&d5,&d7,&a1,&a2,a3,a4,a5,a6) ccout(NVC) clobbers(a0) */
void Spawn_AtDrawPos(uint32_t *d0, uint32_t *d1, uint32_t *d4, uint32_t *d5, uint32_t *d7, uint32_t *a1, uint32_t *a2, uint32_t a3, uint32_t a4, uint32_t a5, uint32_t a6);	/* $008D5C regs(&d0,&d1,&d4,&d5,&d7,&a1,&a2,a3,a4,a5,a6) ccout(NVC) clobbers(a0) */
void Spawn_AtOwnPos(uint32_t *d0, uint32_t *d1, uint32_t *d4, uint32_t *d5, uint32_t *d7, uint32_t *a1, uint32_t *a2, uint32_t a3, uint32_t a4, uint32_t a5, uint32_t a6);	/* $008D66 regs(&d0,&d1,&d4,&d5,&d7,&a1,&a2,a3,a4,a5,a6) ccout(NVC) clobbers(a0) */
void Spawn_AtOwnOfs(uint32_t *d0, uint32_t *d1, uint32_t *d4, uint32_t *d5, uint32_t *d7, uint32_t *a1, uint32_t *a2, uint32_t a3, uint32_t a4, uint32_t a5, uint32_t a6);	/* $008D80 regs(&d0,&d1,&d4,&d5,&d7,&a1,&a2,a3,a4,a5,a6) ccout(NVC) clobbers(a0) */
void Spawn_SetTimer1FromEntry(const u16 **entry, Vm2Block *vm, dk::Flags *ccr);	/* $027C9A regs(&a1,a4,&ccr) ccout(Z) clobbers(d0) */
/* the routine a Spawn_At* callback calls (the script's dc.l, jsr (a0)): the enemy spawners of the call-site
 * log ($8D2A, $8D48, $8D76, $8DAC: 26 targets, each measured regs(d1,d4,d5,a3-a6) -> d0,d1,d7,a0,a2-a6), and a1 =
 * the script after the dc.l for one that reads arguments */
void SpawnRoutineFn(uint32_t code, uint32_t *d0, uint32_t *d1, uint32_t *d4, uint32_t *d5, uint32_t *d7, uint32_t *a0, uint32_t *a1, uint32_t *a2, uint32_t *a3, uint32_t *a4, uint32_t *a5, uint32_t *a6);	/* type regs(&d0,&d1,&d4,&d5,&d7,&a0,&a1,&a2,&a3,&a4,&a5,&a6) */
