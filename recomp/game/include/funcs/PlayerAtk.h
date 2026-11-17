/* funcs/PlayerAtk.h - prototypes of the PlayerAtk module: one line per function, its register
 * convention in the tag (dkit genesis recomp bridge). First written from the measured
 * contracts (dkit disasm regs); edited by the C's authors from then on. */
#pragma once
void PlayerAtk_HitPause(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t d3, uint32_t d4, uint32_t d5, uint32_t d6, uint32_t d7, const HitBox *attack, uint32_t *a1, HitBox *box, Vm2Motion *mot, uint32_t *a4, const Vm2Sprite *spr, uint32_t *a6);	/* $010818 regs(&d0,&d1,&d2,d3,d4,d5,d6,d7,a0,&a1,a2,a3,&a4,a5,&a6) */
void PlayerAtk_HitLongPause(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t d3, uint32_t d4, uint32_t d5, uint32_t d6, uint32_t d7, const HitBox *attack, uint32_t *a1, HitBox *box, Vm2Motion *mot, uint32_t *a4, const Vm2Sprite *spr, uint32_t *a6);	/* $010836 regs(&d0,&d1,&d2,d3,d4,d5,d6,d7,a0,&a1,a2,a3,&a4,a5,&a6) */
