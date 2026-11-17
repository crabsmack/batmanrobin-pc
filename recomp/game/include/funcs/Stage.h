/* funcs/Stage.h - prototypes of the Stage module: one line per function, its register
 * convention in the tag (dkit genesis recomp bridge). First written from the measured
 * contracts (dkit disasm regs); edited by the C's authors from then on. */
#pragma once
void Stage_Intro1(uint32_t *d1, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6, uint32_t *list, uint32_t *a1, uint32_t *a2, uint32_t *a3, uint32_t *a4, uint32_t *a5, uint32_t *a6);	/* $009F74 regs(&d1,&d3,&d4,&d5,&d6,&a0,&a1,&a2,&a3,&a4,&a5,&a6) clobbers(d0,d2,d7) */
void Stage_Intro2(uint32_t *d1, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6, uint32_t *list, uint32_t *a1, uint32_t *a2, uint32_t *a3, uint32_t *a4, uint32_t *a5, uint32_t *a6);	/* $009F86 regs(&d1,&d3,&d4,&d5,&d6,&a0,&a1,&a2,&a3,&a4,&a5,&a6) clobbers(d0,d2,d7) */
void Stage_Intro3(uint32_t *d1, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6, uint32_t *list, uint32_t *a1, uint32_t *a2, uint32_t *a3, uint32_t *a4, uint32_t *a5, uint32_t *a6);	/* $009F98 regs(&d1,&d3,&d4,&d5,&d6,&a0,&a1,&a2,&a3,&a4,&a5,&a6) clobbers(d0,d2,d7) */
void Stage_Intro4(uint32_t *d1, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6, uint32_t *list, uint32_t *a1, uint32_t *a2, uint32_t *a3, uint32_t *a4, uint32_t *a5, uint32_t *a6);	/* $009FAA regs(&d1,&d3,&d4,&d5,&d6,&a0,&a1,&a2,&a3,&a4,&a5,&a6) clobbers(d0,d2,d7) */
