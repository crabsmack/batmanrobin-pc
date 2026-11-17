/* funcs/Coll.h - prototypes of the Coll module: one line per function, its register
 * convention in the tag (dkit genesis recomp bridge). First written from the measured
 * contracts (dkit disasm regs); edited by the C's authors from then on. */
#pragma once
/* Coll_MoveScene's own contract ends at its `jmp (a0)` into the terrain routine (regs(a3)->a0); a
 * C caller sees the routine's results (d3-d7), so every register goes through the call, as
 * VM6_Run's. */
void Coll_MoveScene(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6, uint32_t *d7, uint32_t *a0, uint32_t *a1, uint32_t *a2, const Vm2Motion *mot, uint32_t *a4, uint32_t *a5, uint32_t *a6);	/* $010DCA regs(&d0,&d1,&d2,&d3,&d4,&d5,&d6,&d7,&a0,&a1,&a2,a3,&a4,&a5,&a6) */
void Coll_SetFlatFloor(uint16_t floor_y, dk::Flags *ccr);	/* $010E0A regs(d0,&ccr) ccout(NVC) */
void Coll_MoveBlockMap(uint32_t *d0, uint16_t *d1, uint16_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6, uint32_t *d7, uint32_t *a0, uint32_t *a1, uint32_t *a2, uint32_t *a3, uint32_t *a4, uint32_t *a5, uint32_t *a6);	/* $011D5E regs(&d0,&d1,&d2,&d3,&d4,&d5,&d6,&d7,&a0,&a1,&a2,&a3,&a4,&a5,&a6) */
void Coll_WalkBlockMap(uint32_t *d0, uint16_t d1, uint16_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6, uint32_t *d7);	/* $0120A2 regs(&d0,d1,&d3,&d4,&d5,&d6,&d7) clobbers(d1,a0,a3,a6) */
void Coll_MoveOnBlockMap(uint32_t *d0, uint32_t d1, uint32_t d2, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6, uint32_t *d7, uint32_t a0, uint32_t a1, uint32_t a2, const Vm2Motion *mot, uint32_t a4, uint32_t a5, uint32_t a6);	/* $010DDC regs(&d0,d1,d2,&d3,&d4,&d5,&d6,&d7,a0,a1,a2,a3,a4,a5,a6) */
void Coll_NoTerrain(uint16_t d1, uint16_t d2, uint16_t *d4, uint16_t *d5, uint32_t *d6, uint32_t *d7);	/* $010DFE regs(d1,d2,&d4,&d5,&d6,&d7) */
void Coll_ScriptSetFlatFloor(uint16_t *d0, const u16 **script);	/* $010E08 regs(&d0,&a2) ccout(NVC) */
void Coll_FlatFloor(uint16_t *d0, uint16_t d1, uint16_t d2, uint32_t *d3, uint16_t *d4, uint16_t *d5, uint32_t *d6, uint32_t *d7);	/* $010E20 regs(&d0,d1,d2,&d3,&d4,&d5,&d6,&d7) */
void Coll_FlatFloorRest(uint16_t *d0, uint16_t d1, uint16_t d2, uint32_t *d3, uint16_t *d4, uint16_t *d5, uint32_t *d6, uint32_t *d7);	/* $010E46 regs(&d0,d1,d2,&d3,&d4,&d5,&d6,&d7) */
void Coll_Stage1StreetFloors(uint32_t *d0, uint16_t d1, uint16_t d2, uint32_t *d3, uint16_t *d4, uint16_t *d5, uint32_t *d6, uint32_t *d7);	/* $010E72 regs(&d0,d1,d2,&d3,&d4,&d5,&d6,&d7) */
void Coll_HatterArenaFloor(uint16_t d1, uint16_t d2, uint32_t *d3, uint16_t *d4, uint16_t *d5, uint32_t *d6, uint32_t *d7);	/* $010F04 regs(d1,d2,&d3,&d4,&d5,&d6,&d7) */
void Coll_BlockMapResult(uint32_t *d0, uint16_t *d3, uint32_t *d4, uint32_t *d5);	/* $011E36 regs(&d0,&d3,&d4,&d5) */
void Coll_WalkBlockMapRight(uint32_t *d0, uint16_t d1, uint16_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6, uint32_t *d7, uint32_t a0, uint32_t a3, uint32_t a6);	/* $0121D4 regs(&d0,d1,&d3,&d4,&d5,&d6,&d7,a0,a3,a6) clobbers(d1,a0,a3) */
void Coll_WalkBlockMapLeft(uint32_t *d0, uint16_t d1, uint16_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6, uint32_t *d7, uint32_t a0, uint32_t a3, uint32_t a6);	/* $01238A regs(&d0,d1,&d3,&d4,&d5,&d6,&d7,a0,a3,a6) clobbers(d1,a0,a3) */
void Coll_Stage2TiltFloor(uint32_t *d0, uint16_t dx, uint16_t dy, uint32_t *d3, uint16_t *x, uint16_t *y, uint32_t *d6, uint32_t *mask);	/* $0247C6 regs(&d0,d1,d2,&d3,&d4,&d5,&d6,&d7) */
void Coll_Stage2TiltFloorRest(uint32_t *d0, uint16_t dx, uint16_t dy, uint32_t *d3, uint16_t *x, uint16_t *y, uint32_t *d6, uint32_t *mask);	/* $0247F8 regs(&d0,d1,d2,&d3,&d4,&d5,&d6,&d7) */
void Coll_MoveAirborneBlockMap(uint32_t *d0, uint32_t d1, uint32_t d2, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6, uint32_t *d7, uint32_t a0, uint32_t a1, uint32_t a2, const Vm2Motion *mot, uint32_t a4, uint32_t a5, uint32_t a6);	/* $010DE2 regs(&d0,d1,d2,&d3,&d4,&d5,&d6,&d7,a0,a1,a2,a3,a4,a5,a6) */
/* Coll_MoveGroundedBlockMap: not measured (no reference run enters it; a scene's terrain pointer,
 * entered by the stepped machine): Coll_MoveAirborneBlockMap's convention, its other side. */
void Coll_MoveGroundedBlockMap(uint32_t *d0, uint32_t d1, uint32_t d2, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6, uint32_t *d7, uint32_t a0, uint32_t a1, uint32_t a2, const Vm2Motion *mot, uint32_t a4, uint32_t a5, uint32_t a6);	/* $010DF0 regs(&d0,d1,d2,&d3,&d4,&d5,&d6,&d7,a0,a1,a2,a3,a4,a5,a6) */
/* Coll_BlockMapStep: the dbra target of Coll_MoveBlockMap's step routines (not measured: entered only
 * inside the move); its registers are Coll_MoveBlockMap's, every one through. */
void Coll_BlockMapStep(uint32_t *d0, uint16_t *d1, uint16_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6, uint32_t *d7, uint32_t *a0, uint32_t *a1, uint32_t *a2, uint32_t *a3, uint32_t *a4, uint32_t *a5, uint32_t *a6);	/* $011E2C regs(&d0,&d1,&d2,&d3,&d4,&d5,&d6,&d7,&a0,&a1,&a2,&a3,&a4,&a5,&a6) */
/* Coll_WalkBlockMapEnd / Probe: the walk loops' exits (not measured: entered only inside the walk); the
 * walk loops' registers without the pixel count. */
void Coll_WalkBlockMapEnd(uint32_t *d0, uint16_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6, uint32_t *d7, uint32_t a0, uint32_t a3, uint32_t a6);	/* $01211C regs(&d0,&d3,&d4,&d5,&d6,&d7,a0,a3,a6) clobbers(a0,a3) */
void Coll_WalkBlockMapProbe(uint32_t *d0, uint16_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6, uint32_t *d7, uint32_t a0, uint32_t a3, uint32_t a6);	/* $01211E regs(&d0,&d3,&d4,&d5,&d6,&d7,a0,a3,a6) clobbers(a0,a3) */
