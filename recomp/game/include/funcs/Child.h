/* funcs/Child.h - prototypes of the Child module: one line per function, its register
 * convention in the tag (dkit genesis recomp bridge). First written from the measured
 * contracts (dkit disasm regs); edited by the C's authors from then on. */
#pragma once
void Child_GetA4(uint16_t *field, uint16_t *child, uint32_t *a0, const u16 **script, const Vm2Obj *self);	/* $00267C regs(&d0,&d1,&a0,&a2,a6) */
uint32_t Child_A4OfHandle(uint16_t child);	/* $0026AA regs(d1)->a0 */
void Child_GetA4Skip2(uint16_t *field, uint16_t *child, uint32_t *a0, const u16 **script, const Vm2Obj *self);	/* $00268A regs(&d0,&d1,&a0,&a2,a6) */
