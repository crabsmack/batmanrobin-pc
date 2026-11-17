/* funcs/Text.h - prototypes of the Text module: one line per function, its register
 * convention in the tag (dkit genesis recomp bridge). First written from the measured
 * contracts (dkit disasm regs); edited by the C's authors from then on. */
#pragma once
void Text_DrawAtObject(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint16_t *x, uint16_t *y, uint16_t *attr, const PTR(const u8) **script, const Vm2Sprite *sprite, const Vm2Obj *self, dk::Flags *ccr);	/* $01410E regs(&d0,&d1,&d2,&d3,&d4,&d5,&d6,&a2,a5,a6,&ccr) ccout(NVC) clobbers(a0) */
void Text_DrawSprites(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint16_t *x, uint16_t y, uint16_t attr, const u8 *text, dk::Flags *ccr);	/* $01412E regs(&d0,&d1,&d2,&d3,&d4,d5,d6,a2,&ccr) ccout(NVC) clobbers(a0,a1,a2) */
void Text_DrawSpritesVertical(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *x, uint32_t *y, uint16_t attr, uint32_t string);	/* $01416A regs(&d0,&d1,&d2,&d3,&d4,&d5,d6,a2) clobbers(a0,a1,a2) */
