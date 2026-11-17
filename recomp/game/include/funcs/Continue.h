/* funcs/Continue.h - prototypes of the Continue module: one line per function, its register
 * convention in the tag (dkit genesis recomp bridge). First written from the measured
 * contracts (dkit disasm regs); edited by the C's authors from then on. */
#pragma once
void Continue_Run(uint32_t *d1, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6, uint32_t *a2, uint32_t *a3, uint32_t *a4, uint32_t *a5, uint32_t *a6);	/* $04916E regs(&d1,&d3,&d4,&d5,&d6,&a2,&a3,&a4,&a5,&a6) clobbers(d0,d2,d7,a0,a1) */
void Continue_SpawnController(uint32_t *d0, uint16_t *d1, uint16_t d4, uint16_t d5, uint32_t *d7, uint32_t *a2, uint32_t *a3, uint32_t *a4, uint32_t a6);	/* $049194 regs(&d0,&d1,d4,d5,&d7,&a2,&a3,&a4,a6) clobbers(a0,a5,a6) */
void Continue_BranchHasCredits(uint32_t d0, uint32_t d1, uint32_t d2, uint32_t d3, uint32_t d4, uint32_t d5, uint32_t d6, uint32_t d7, uint32_t a1, uint32_t a3, uint32_t a4, uint32_t a5, uint32_t a6);	/* $0491BC regs(d0,d1,d2,d3,d4,d5,d6,d7,a1,a3,a4,a5,a6) */
void Continue_HasCredits(uint32_t d0, uint32_t d1, uint32_t d2, uint32_t d3, uint32_t d4, uint32_t d5, uint32_t d6, uint32_t d7, uint32_t a1, uint32_t a3, uint32_t a4, uint32_t a5, uint32_t a6);	/* $0491D2 regs(d0,d1,d2,d3,d4,d5,d6,d7,a1,a3,a4,a5,a6) */
void Continue_UseCredit(uint32_t d0, uint32_t d1, uint32_t d2, uint32_t d3, uint32_t d4, uint32_t d5, uint32_t d6, uint32_t d7, uint32_t a1, uint32_t a3, uint32_t a4, uint32_t a5, uint32_t a6);	/* $0491FC regs(d0,d1,d2,d3,d4,d5,d6,d7,a1,a3,a4,a5,a6) */
void Continue_DrawCreditsText(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6, uint32_t *d7, uint32_t *a1, uint32_t *a2, uint32_t *a3, uint32_t *a4, uint32_t *a5, uint32_t *a6);	/* $049212 regs(&d0,&d1,&d2,&d3,&d4,&d5,&d6,&d7,&a1,&a2,&a3,&a4,&a5,&a6) */
void Continue_SlowWhiteFade(uint32_t d0, uint32_t d1, uint32_t d2, uint32_t d3, uint32_t d4, uint32_t d5, uint32_t d6, uint32_t d7, uint32_t a1, uint32_t a3, uint32_t a4, uint32_t a5, uint32_t a6);	/* $0493D2 regs(d0,d1,d2,d3,d4,d5,d6,d7,a1,a3,a4,a5,a6) */
void Continue_ScrollPicture(uint16_t *d0, uint16_t *d1, const Vm2Obj *self);	/* $0493FA regs(&d0,&d1,a6) */
void Continue_DrawNoCreditsText(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6, uint32_t *d7, uint32_t *a1, uint32_t *a2, uint32_t *a3, uint32_t *a4, uint32_t *a5, uint32_t *a6);	/* $049236 regs(&d0,&d1,&d2,&d3,&d4,&d5,&d6,&d7,&a1,&a2,&a3,&a4,&a5,&a6) */
