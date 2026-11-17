/* funcs/Drop.h - prototypes of the Drop module: one line per function, its register
 * convention in the tag (dkit genesis recomp bridge). First written from the measured
 * contracts (dkit disasm regs); edited by the C's authors from then on. */
#pragma once
void Drop_RandomHealthB(uint32_t *d0, uint16_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *a0, uint32_t *a4, uint32_t *a6, dk::Flags *ccr);	/* $01300E regs(&d0,&d1,&d2,&d3,&d4,&d5,&a0,&a4,&a6,&ccr) ccout(NZVC) */
void Drop_RandomHealthB_case4(uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *a0, uint32_t *a4, uint32_t *a6);	/* $013034 regs(&d2,&d3,&d4,&d5,&a0,&a4,&a6) ccout(NZVC) */
void Drop_HealthA1(uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *a0, uint32_t *a4, uint32_t *a6);	/* $013038 regs(&d2,&d3,&d4,&d5,&a0,&a4,&a6) ccout(NZVC) */
void Drop_HealthARow(uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *a0, uint32_t *a4, uint32_t *a6);	/* $01306A regs(&d2,&d3,&d4,&d5,&a0,&a4,&a6) ccout(NZVC) */
void Drop_RunFxScript(uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *a0, uint32_t *a4, uint32_t *a6);	/* $013288 regs(&d2,&d3,&d4,&d5,&a0,&a4,&a6) ccout(NZVC) */
void Drop_HealthA2(uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *a0, uint32_t *a4, uint32_t *a6);	/* $013042 regs(&d2,&d3,&d4,&d5,&a0,&a4,&a6) ccout(NZVC) */
void Drop_HealthA3(uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *a0, uint32_t *a4, uint32_t *a6);	/* $01304C regs(&d2,&d3,&d4,&d5,&a0,&a4,&a6) ccout(NZVC) */
void Drop_HealthA5(uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *a0, uint32_t *a4, uint32_t *a6);	/* $013056 regs(&d2,&d3,&d4,&d5,&a0,&a4,&a6) ccout(NZVC) */
void Drop_HealthB1(uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *a0, uint32_t *a4, uint32_t *a6);	/* $013074 regs(&d2,&d3,&d4,&d5,&a0,&a4,&a6) ccout(NZVC) */
void Drop_HealthB2(uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *a0, uint32_t *a4, uint32_t *a6);	/* $01307E regs(&d2,&d3,&d4,&d5,&a0,&a4,&a6) ccout(NZVC) */
void Drop_HealthB3(uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *a0, uint32_t *a4, uint32_t *a6);	/* $013088 regs(&d2,&d3,&d4,&d5,&a0,&a4,&a6) ccout(NZVC) */
void Drop_HealthB5(uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *a0, uint32_t *a4, uint32_t *a6);	/* $013092 regs(&d2,&d3,&d4,&d5,&a0,&a4,&a6) ccout(NZVC) */
void Drop_HealthB9(uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *a0, uint32_t *a4, uint32_t *a6);	/* $01309C regs(&d2,&d3,&d4,&d5,&a0,&a4,&a6) ccout(NZVC) */
void Drop_HealthBRow(uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *a0, uint32_t *a4, uint32_t *a6);	/* $0130A6 regs(&d2,&d3,&d4,&d5,&a0,&a4,&a6) ccout(NZVC) */
const void *Drop_CollectHealth(const HitBox *box, dk::Flags *ccr);	/* $013296 regs(a2,&ccr)->a1 ccout(NZVC) clobbers(a0) */
void Drop_RandomHealthB_case0(uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *a0, uint32_t *a4, uint32_t *a6);	/* $013030 regs(&d2,&d3,&d4,&d5,&a0,&a4,&a6) ccout(NZVC) */
