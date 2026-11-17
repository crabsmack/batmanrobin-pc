/* funcs/Boot.h - prototypes of the Boot module: one line per function, its register
 * convention in the tag (dkit genesis recomp bridge). First written from the measured
 * contracts (dkit disasm regs); edited by the C's authors from then on. */
#pragma once
void Boot_Reset(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6, uint32_t *d7, uint32_t *a0, uint32_t *a1, uint32_t *a2, uint32_t *a3, uint32_t *a4, uint32_t *a5, uint32_t *a6);	/* $000300 regs(&d0,&d1,&d2,&d3,&d4,&d5,&d6,&d7,&a0,&a1,&a2,&a3,&a4,&a5,&a6) noreturn */
void Boot_CheckRegion(uint32_t d1, uint32_t d3, uint32_t d4, uint32_t d5, uint32_t d6, uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4, uint32_t a6);	/* $0003FA regs(d1,d3,d4,d5,d6,a1,a2,a3,a4,a6) noreturn */
void Boot_DrawLockoutText(uint8_t column, uint8_t row, const u8 *text, uint32_t data_port, uint32_t ctrl_port);	/* $000500 regs(d0,d1,a0,a4,a5) clobbers(d2,d3,a0) */
void Boot_InitIOPorts(uint32_t d1, uint32_t d3, uint32_t d4, uint32_t d5, uint32_t d6, uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4, uint32_t a6);	/* $000790 regs(d1,d3,d4,d5,d6,a1,a2,a3,a4,a6) noreturn */
