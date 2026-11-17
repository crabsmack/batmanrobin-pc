/* funcs/VM4.h - prototypes of the VM4 module: one line per function, its register
 * convention in the tag (dkit genesis recomp bridge). First written from the measured
 * contracts (dkit disasm regs); edited by the C's authors from then on. */
#pragma once
void VM4_RunObject(uint32_t *handler, uint32_t *script, uint32_t *child, Vm4Obj *self);	/* $0053B4 regs(&a0,&a4,&a5,a6) */
void VM4_Next(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6, uint32_t *d7, uint32_t *a0, uint32_t *a1, uint32_t *a2, uint32_t *a3, uint32_t *a4, uint32_t *a5, uint32_t *a6);	/* $0053BA regs(&d0,&d1,&d2,&d3,&d4,&d5,&d6,&d7,&a0,&a1,&a2,&a3,&a4,&a5,&a6) */
uint32_t VM4_ResumeA5(const Vm4Obj *obj);	/* $0055C2 regs(a5)->a4 clobbers(a0) */
void VM4_Fork(uint32_t *d0, uint32_t d2, uint32_t d3, uint32_t d4, uint32_t d5, uint32_t *a0, uint32_t *a1, uint32_t a3, uint32_t *a5, const Vm4Obj *parent, dk::Flags *ccr);	/* $0060C4 regs(&d0,d2,d3,d4,d5,&a0,&a1,a3,&a5,a6,&ccr) ccout(NZVC) */
void VM4_SpawnChild(uint32_t *d0, uint32_t *d1, uint32_t d2, uint32_t d3, uint32_t d4, uint32_t d5, uint32_t *a0, uint32_t *a1, uint32_t *a3, uint32_t *a4, uint32_t *a5, uint32_t a6, dk::Flags *ccr);	/* $00613A regs(&d0,&d1,d2,d3,d4,d5,&a0,&a1,&a3,&a4,&a5,a6,&ccr) ccout(NZVC) */
void VM4_CalcDrawPos(uint32_t *d0, uint32_t *d1, uint32_t *d4, uint32_t *d5, const Vm4Obj *self);	/* $00624C regs(&d0,&d1,&d4,&d5,a6) clobbers(a0) */
uint32_t VM4_Enter(void);	/* $0053C6 regs()->a4 clobbers(a0) */
void VM4_ReinitSelf(uint32_t *d0, uint32_t *d1, uint32_t d2, uint32_t d3, uint32_t d4, uint32_t d5, uint32_t *a0, uint32_t a1, uint32_t *a3, uint32_t *a4, uint32_t *a5, uint32_t a6, dk::Flags *ccr);	/* $006156 regs(&d0,&d1,d2,d3,d4,d5,&a0,a1,&a3,&a4,&a5,a6,&ccr) ccout(NZVC) */
/* the two interior entries of VM4_SpawnChild (branch targets only, no measured contract: VM4_SpawnChild's registers) */
void VM4_SpawnChild_Init(uint32_t *d0, uint32_t *d1, uint32_t d2, uint32_t d3, uint32_t d4, uint32_t d5, uint32_t *a0, uint32_t *a3, uint32_t *a4, Vm4Obj *child, const Vm4Obj *parent, dk::Flags *ccr);	/* $00616A regs(&d0,&d1,d2,d3,d4,d5,&a0,&a3,&a4,a5,a6,&ccr) ccout(NZVC) */
void VM4_SpawnChild_InitMotion(uint32_t *d0, uint32_t *d1, uint32_t d2, uint32_t d3, uint32_t d4, uint32_t d5, uint32_t *a0, uint32_t *a3, uint32_t *a4, Vm4Obj *obj, dk::Flags *ccr);	/* $006182 regs(&d0,&d1,d2,d3,d4,d5,&a0,&a3,&a4,a5,&ccr) ccout(NZVC) */
