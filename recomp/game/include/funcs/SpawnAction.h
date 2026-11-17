/* funcs/SpawnAction.h - prototypes of the SpawnAction module: one line per function, its register
 * convention in the tag (dkit genesis recomp bridge). First written from the measured
 * contracts (dkit disasm regs); edited by the C's authors from then on. */
#pragma once
void SpawnAction_GroupMemberGone(uint16_t d1);	/* $006F9A regs(d1) clobbers(d0,a0) */
void SpawnAction_Run(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6, uint32_t *d7, uint32_t *a1, uint32_t a2, uint32_t a3, uint32_t a4, uint32_t a5, uint32_t a6);	/* $012ED0 regs(&d0,&d1,&d2,&d3,&d4,&d5,&d6,&d7,&a1,a2,a3,a4,a5,a6) clobbers(a0) */
void SpawnAction_GroupMemberReward(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6, uint32_t *d7, uint32_t *a1, uint32_t a2, uint32_t a3, uint32_t a4, uint32_t a5, uint32_t a6);	/* $006F70 regs(&d0,&d1,&d2,&d3,&d4,&d5,&d6,&d7,&a1,a2,a3,a4,a5,a6) clobbers(a0,a2,a3,a4,a5,a6) */
void SpawnAction_None(void);	/* $012EEE regs() */
/* a spawn action (Tbl_012EF0, `jsr (a0)` in SpawnAction_Run): d0 = the action x 4, d1 its argument,
 * d4/d5 = the dying object's x/y; the item spawners, the Drop_ actions and the group counters (the
 * call-site log at $012EE8). Every register in and out: SpawnAction_Run hands back what they leave. */
void SpawnActionFn(uint32_t code, uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6, uint32_t *d7, uint32_t *a0, uint32_t *a1, uint32_t *a2, uint32_t *a3, uint32_t *a4, uint32_t *a5, uint32_t *a6);	/* type regs(&d0,&d1,&d2,&d3,&d4,&d5,&d6,&d7,&a0,&a1,&a2,&a3,&a4,&a5,&a6) */
uint16_t SpawnAction_GroupCountGone(uint32_t a0, dk::Flags *ccr);	/* $006FA2 regs(a0,&ccr)->d0 ccout(NZVC) */
