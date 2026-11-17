/* funcs/VM2.h - prototypes of the VM2 module: one line per function, its register
 * convention in the tag (dkit genesis recomp bridge). First written from the measured
 * contracts (dkit disasm regs); edited by the C's authors from then on. */
#pragma once
void VM2_RunObject(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6, uint32_t *d7, uint32_t *handler, uint32_t *a1, const u16 **script, void **motion, Vm2Block **vm, void **sprite, Vm2Obj *self);	/* $0007D2 regs(&d0,&d1,&d2,&d3,&d4,&d5,&d6,&d7,&a0,&a1,&a2,&a3,&a4,&a5,a6) */
void VM2_Next(uint32_t *handler, const u16 **script);	/* $0007E0 regs(&a0,&a2) */
void VM2_Enter(uint32_t *handler, const u16 **script);	/* $0007FE regs(&a0,&a2) */
