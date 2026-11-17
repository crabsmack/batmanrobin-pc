/* funcs/ObjPool.h - prototypes of the ObjPool module: one line per function, its register
 * convention in the tag (dkit genesis recomp bridge). First written from the measured
 * contracts (dkit disasm regs); edited by the C's authors from then on. */
#pragma once
void ObjPool_Init(uint16_t count, uint32_t pool);	/* $0086E6 regs(d0,a0) clobbers(d0,a0) */
void ObjPool_Alloc(uint32_t *result, uint32_t *block);	/* $0086FE regs(&d0,&a0) tst(d0.l) ccout(NZVC) */
void ObjPool_AllocClear(uint32_t *block);	/* $008718 regs(&a0) clobbers(d0) */
bool ObjPool_Free(uint32_t block);	/* $008764 regs(a0)->Z ccout(Z) */
void ObjPool_HasFree(uint32_t *wanted);	/* $008772 regs(&d0) tst(d0.l) ccout(Z) */
void ObjPool_Clear(uint32_t *zero, uint32_t *block);	/* $008780 regs(&d0,&a0) */
