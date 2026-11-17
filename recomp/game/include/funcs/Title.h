/* funcs/Title.h - prototypes of the Title module: one line per function, its register
 * convention in the tag (dkit genesis recomp bridge). First written from the measured
 * contracts (dkit disasm regs); edited by the C's authors from then on. */
#pragma once
void Title_Run(uint32_t *d1, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6, uint32_t *a1, uint32_t *a2, uint32_t *a3, uint32_t *a4, uint32_t *a5, uint32_t *a6);	/* $0496A2 regs(&d1,&d3,&d4,&d5,&d6,&a1,&a2,&a3,&a4,&a5,&a6) clobbers(d0,d2,d7,a0) */
void Title_SpawnMenu(uint16_t *d1, uint16_t d4, uint16_t d5, uint32_t *a2, uint32_t *a3, uint32_t *a4, uint32_t *a6);	/* $0496CC regs(&d1,d4,d5,&a2,&a3,&a4,&a6) clobbers(d0,d7,a0,a5) */
void Title_ReadPadAndCursor(uint32_t *d0, uint32_t d1, uint32_t d2, uint32_t d3, uint32_t d4, uint32_t d5, uint32_t d6, uint32_t d7, uint32_t a1, uint32_t a3, Vm2Block *vm, uint32_t a5, uint32_t a6);	/* $0496EC regs(&d0,d1,d2,d3,d4,d5,d6,d7,a1,a3,a4,a5,a6) */
void Title_BackFromOptions(uint32_t *d0, uint32_t *d1, uint32_t d2, uint32_t d3, uint32_t d4, uint32_t d5, uint32_t d6, uint32_t *d7, uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4, uint32_t a5, uint32_t a6);	/* $049712 regs(&d0,&d1,d2,d3,d4,d5,d6,&d7,a1,a2,a3,a4,a5,a6) */
uint32_t Title_DimPicture(void);	/* $049770 regs()->d0 */
void Title_StartPaletteFade(uint32_t *d0, uint32_t *d7, uint32_t a6);	/* $049798 regs(&d0,&d7,a6) clobbers(a0,a1,a5,a6) */
void Title_PaletteFadeRun(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d6, uint32_t *d7, uint32_t *a0, uint32_t *a1, uint32_t *a2, uint32_t *a3, uint32_t *a4, TitleFadeObj *self);	/* $0497DA regs(&d0,&d1,&d2,&d6,&d7,&a0,&a1,&a2,&a3,&a4,a6) clobbers(a5) */
void Title_PaletteFadeChannel(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d6, uint32_t *a0, uint32_t *a1, uint32_t *a2, uint32_t *a3, uint32_t *a4, TitleFadeChannel *ch, const TitleFadeObj *self);	/* $049816 regs(&d0,&d1,&d2,&d6,&a0,&a1,&a2,&a3,&a4,a5,a6) clobbers(d7) */
void Title_MenuInit(uint32_t *d0, uint32_t *d7);	/* $0499B6 regs(&d0,&d7) clobbers(a0) */
void Title_MenuStep(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6, uint32_t *d7, uint32_t *a1, uint32_t *a2, uint32_t *a3, uint32_t *a4, uint32_t *a5, uint32_t *a6);	/* $0499C2 regs(&d0,&d1,&d2,&d3,&d4,&d5,&d6,&d7,&a1,&a2,&a3,&a4,&a5,&a6) clobbers(a0) */
