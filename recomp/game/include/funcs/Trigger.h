/* funcs/Trigger.h - prototypes of the Trigger module: one line per function, its register
 * convention in the tag (dkit genesis recomp bridge). First written from the measured
 * contracts (dkit disasm regs); edited by the C's authors from then on. */
#pragma once
void Trigger_RunX(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t d3, uint32_t *d4, uint32_t *d5, uint32_t *left, uint32_t *right, uint32_t *a0, uint32_t *a1, const TriggerKey **next, const u8 **table, uint32_t *a4, uint32_t *a5, uint32_t *a6);	/* $012E02 regs(&d0,&d1,&d2,d3,&d4,&d5,&d6,&d7,&a0,&a1,&a2,&a3,&a4,&a5,&a6) */
void Trigger_Fire(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t d3, uint32_t *d4, uint32_t *d5, uint32_t d6, uint32_t d7, uint32_t *a0, uint32_t *a1, uint32_t a2, const u8 *table, uint32_t *a4, uint32_t *a5, uint32_t *a6, dk::Flags *ccr);	/* $012E36 regs(&d0,&d1,&d2,d3,&d4,&d5,d6,d7,&a0,&a1,a2,a3,&a4,&a5,&a6,&ccr) ccout(Z) */
void Trigger_LoadY(uint32_t *base, const u8 *table, TriggerKey **dst);	/* $012E5C regs(&d6,a0,&a3) clobbers(d0,d7,a0,a1,a2) */
void Trigger_LoadX(uint32_t *base, uint16_t *count, const u8 **table, TriggerKey **end, TriggerKey **src, TriggerKey **dst);	/* $012DC2 regs(&d6,&d7,&a0,&a1,&a2,&a3) clobbers(d0) */
void Trigger_RunY(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t d3, uint32_t *d4, uint32_t *d5, uint32_t *top, uint32_t *bottom, uint32_t *a1, const TriggerKey **next, const u8 **table, uint32_t *a4, uint32_t a5, uint32_t a6);	/* $012E9C regs(&d0,&d1,&d2,d3,&d4,&d5,&d6,&d7,&a1,&a2,&a3,&a4,a5,a6) clobbers(a0,a5,a6) */
/* a trigger's callback (`jsr (a0)` in Trigger_Fire): x/y in d4/d5, the result its Z flag (set =
 * done; clear = retry next frame) and a6 (the spawned object); Trigger_Fire saves d0/d6/d7/a2/a3
 * around it. Targets: Wave_StartFromEntry and the stage spawners (the call-site log at $012E48). */
void TriggerCallback(uint32_t code, uint32_t d0, uint32_t *d1, uint32_t *d2, uint32_t d3, uint32_t *d4, uint32_t *d5, uint32_t d6, uint32_t d7, uint32_t *a0, uint32_t *a1, uint32_t a2, uint32_t a3, uint32_t *a4, uint32_t *a5, uint32_t *a6, dk::Flags *ccr);	/* type regs(d0,&d1,&d2,d3,&d4,&d5,d6,d7,&a0,&a1,a2,a3,&a4,&a5,&a6,&ccr) */
