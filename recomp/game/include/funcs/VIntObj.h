/* funcs/VIntObj.h - prototypes of the VIntObj module: one line per function, its register
 * convention in the tag (dkit genesis recomp bridge). First written from the measured
 * contracts (dkit disasm regs); edited by the C's authors from then on. */
#pragma once
void VIntObj_RunList(uint32_t d1, uint32_t d2, uint32_t d3, uint32_t d4, uint32_t d5, uint32_t d6, uint32_t d7, uint32_t *a0, uint32_t *a1, uint32_t *a2);	/* $008ADA regs(d1,d2,d3,d4,d5,d6,d7,&a0,&a1,&a2) clobbers(d0,d1,d2,d3,d4,d5,d6,d7,a3,a4,a5,a6) */
void VIntObj_ListEnd(void);	/* $008B22 regs() */
void VIntObj_InitList(void);	/* $008B26 regs() clobbers(a0,a1) */
void VIntObj_New(uint32_t *result, uint32_t handler, uint32_t *tail, uint32_t *obj);	/* $008B7E regs(&d0,a0,&a1,&a6) tst(d0.l) ccout(Z) clobbers(a0) */
void VIntObj_NewIdle(uint32_t *result, uint32_t *last, uint32_t *tail, uint32_t *obj);	/* $008BB8 regs(&d0,&a0,&a1,&a6) tst(d0.l) ccout(Z) */
uint32_t VIntObj_UnlinkFree(uint32_t a6);	/* $008BF6 regs(a6)->a1 clobbers(a0) */
void VIntObj_IdleHandler(void);	/* $008BEC regs() */
