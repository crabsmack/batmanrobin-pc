/* funcs/Rot3D.h - prototypes of the Rot3D module: one line per function, its register
 * convention in the tag (dkit genesis recomp bridge). First written from the measured
 * contracts (dkit disasm regs); edited by the C's authors from then on. */
#pragma once
void Rot3D_ProjectPoint(uint32_t *d0, uint32_t *d1, uint32_t *x, uint32_t *y, uint32_t *depth, const Rot3D *rec, const s16 **point);	/* $0153CE regs(&d0,&d1,&d4,&d5,&d7,a0,&a1) */
void Rot3D_TransformPoint(uint32_t *d0, uint32_t *d1, uint32_t *x, uint32_t *y, uint32_t *z, const Rot3D *rec, const s16 **point);	/* $01543E regs(&d0,&d1,&d4,&d5,&d7,a0,&a1) */
void Rot3D_BuildMatrix(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, Rot3D *rec, uint32_t *a2, uint32_t *sine_at, uint32_t *cosine_at);	/* $0154A6 regs(&d0,&d1,&d2,&d3,a0,&a2,&a3,&a4) clobbers(d4,d5,d6,d7,a1) */
