/* funcs/StageCard.h - prototypes of the StageCard module: one line per function, its register
 * convention in the tag (dkit genesis recomp bridge). First written from the measured
 * contracts (dkit disasm regs); edited by the C's authors from then on. */
#pragma once
void StageCard_Stage1(uint32_t *d1, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6, uint32_t *a1, uint32_t *a2, uint32_t *a3, uint32_t *a4, uint32_t *a5, uint32_t *a6);	/* $04A216 regs(&d1,&d3,&d4,&d5,&d6,&a1,&a2,&a3,&a4,&a5,&a6) clobbers(d0,d2,d7,a0) */
void StageCard_SpawnDirector1(uint32_t *d0, uint16_t *d1, uint16_t d4, uint16_t d5, uint32_t *d7, uint32_t *a2, uint32_t *a3, uint32_t *a4, uint32_t a6);	/* $04A232 regs(&d0,&d1,d4,d5,&d7,&a2,&a3,&a4,a6) clobbers(a0,a5,a6) */
void StageCard_SpawnTitle1(uint32_t *d0, uint16_t *d1, uint16_t d4, uint16_t d5, uint32_t *d7, uint32_t a4, uint32_t a6);	/* $04A274 regs(&d0,&d1,d4,d5,&d7,a4,a6) clobbers(a0,a2,a3,a4,a5,a6) */
void StageCard_SpawnSubtitle1(uint32_t *d0, uint16_t *d1, uint16_t d4, uint16_t d5, uint32_t *d7, uint32_t a4, uint32_t a6);	/* $04A2A8 regs(&d0,&d1,d4,d5,&d7,a4,a6) clobbers(a0,a2,a3,a4,a5,a6) */
void StageCard_Stage2(uint32_t *d1, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6, uint32_t *a1, uint32_t *a2, uint32_t *a3, uint32_t *a4, uint32_t *a5, uint32_t *a6);	/* $04A2DC regs(&d1,&d3,&d4,&d5,&d6,&a1,&a2,&a3,&a4,&a5,&a6) clobbers(d0,d2,d7,a0) */
void StageCard_SpawnDirector2(uint32_t *d0, uint16_t *d1, uint16_t d4, uint16_t d5, uint32_t *d7, uint32_t *a2, uint32_t *a3, uint32_t *a4, uint32_t a6);	/* $04A2FA regs(&d0,&d1,d4,d5,&d7,&a2,&a3,&a4,a6) clobbers(a0,a5,a6) */
void StageCard_SpawnTitle2(uint32_t *d0, uint16_t *d1, uint16_t d4, uint16_t d5, uint32_t *d7, uint32_t a4, uint32_t a6);	/* $04A33C regs(&d0,&d1,d4,d5,&d7,a4,a6) clobbers(a0,a2,a3,a4,a5,a6) */
void StageCard_SpawnSubtitle2(uint32_t *d0, uint16_t *d1, uint16_t d4, uint16_t d5, uint32_t *d7, uint32_t a4, uint32_t a6);	/* $04A370 regs(&d0,&d1,d4,d5,&d7,a4,a6) clobbers(a0,a2,a3,a4,a5,a6) */
void StageCard_Stage3(uint32_t *d1, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6, uint32_t *a1, uint32_t *a2, uint32_t *a3, uint32_t *a4, uint32_t *a5, uint32_t *a6);	/* $04A3A4 regs(&d1,&d3,&d4,&d5,&d6,&a1,&a2,&a3,&a4,&a5,&a6) clobbers(d0,d2,d7,a0) */
void StageCard_SpawnDirector3(uint32_t *d0, uint16_t *d1, uint16_t d4, uint16_t d5, uint32_t *d7, uint32_t *a2, uint32_t *a3, uint32_t *a4, uint32_t a6);	/* $04A3C0 regs(&d0,&d1,d4,d5,&d7,&a2,&a3,&a4,a6) clobbers(a0,a5,a6) */
void StageCard_SpawnTitle3(uint32_t *d0, uint16_t *d1, uint16_t d4, uint16_t d5, uint32_t *d7, uint32_t a4, uint32_t a6);	/* $04A3FC regs(&d0,&d1,d4,d5,&d7,a4,a6) clobbers(a0,a2,a3,a4,a5,a6) */
void StageCard_SpawnSubtitle3(uint32_t *d0, uint16_t *d1, uint16_t d4, uint16_t d5, uint32_t *d7, uint32_t a4, uint32_t a6);	/* $04A430 regs(&d0,&d1,d4,d5,&d7,a4,a6) clobbers(a0,a2,a3,a4,a5,a6) */
void StageCard_Stage4(uint32_t *d1, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6, uint32_t *a1, uint32_t *a2, uint32_t *a3, uint32_t *a4, uint32_t *a5, uint32_t *a6);	/* $04A464 regs(&d1,&d3,&d4,&d5,&d6,&a1,&a2,&a3,&a4,&a5,&a6) clobbers(d0,d2,d7,a0) */
void StageCard_SpawnDirector4(uint32_t *d0, uint16_t *d1, uint16_t d4, uint16_t d5, uint32_t *d7, uint32_t *a2, uint32_t *a3, uint32_t *a4, uint32_t a6);	/* $04A480 regs(&d0,&d1,d4,d5,&d7,&a2,&a3,&a4,a6) clobbers(a0,a5,a6) */
void StageCard_SpawnTitle4(uint32_t *d0, uint16_t *d1, uint16_t d4, uint16_t d5, uint32_t *d7, uint32_t a4, uint32_t a6);	/* $04A4C2 regs(&d0,&d1,d4,d5,&d7,a4,a6) clobbers(a0,a2,a3,a4,a5,a6) */
void StageCard_SpawnSubtitle4(uint32_t *d0, uint16_t *d1, uint16_t d4, uint16_t d5, uint32_t *d7, uint32_t a4, uint32_t a6);	/* $04A4F6 regs(&d0,&d1,d4,d5,&d7,a4,a6) clobbers(a0,a2,a3,a4,a5,a6) */
