/* funcs/Menu.h - prototypes of the Menu module: one line per function, its register
 * convention in the tag (dkit genesis recomp bridge). First written from the measured
 * contracts (dkit disasm regs); edited by the C's authors from then on. */
#pragma once
void Menu_Update(uint32_t *d0, uint16_t *last, uint8_t *pad, uint32_t table, uint32_t *value_at);	/* $01458C regs(&d0,&d1,&d2,a0,&a1) clobbers(a0) */
void Menu_MoveCursor(uint32_t *d0, uint16_t last, uint8_t *pad);	/* $014562 regs(&d0,d1,&d2) */
