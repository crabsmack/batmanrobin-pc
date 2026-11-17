/* funcs/ScrollCmd.h - prototypes of the ScrollCmd module: one line per function, its register
 * convention in the tag (dkit genesis recomp bridge). First written from the measured
 * contracts (dkit disasm regs); edited by the C's authors from then on. */
#pragma once
void ScrollCmd_LayerA(uint32_t *d0, uint32_t d1, uint32_t *d2, uint32_t *d3, uint16_t *d6, uint16_t *d7, uint32_t *bands, const u16 **list, uint32_t *layer);	/* $009820 regs(&d0,d1,&d2,&d3,&d6,&d7,&a1,&a5,&a6) clobbers(a0) */
void ScrollCmd_CameraA(uint32_t *d0, uint32_t d1, uint32_t *x, uint32_t *y, uint16_t *d6, uint16_t *d7, uint32_t *bands, const u16 **list, uint32_t *layer);	/* $009848 regs(&d0,d1,&d2,&d3,&d6,&d7,&a1,&a5,&a6) clobbers(a0) */
void ScrollCmd_CameraB(uint32_t *d0, uint32_t d1, uint32_t *x, uint32_t *y, uint16_t *d6, uint16_t *d7, uint32_t *bands, const u16 **list, uint32_t *layer);	/* $009884 regs(&d0,d1,&d2,&d3,&d6,&d7,&a1,&a5,&a6) clobbers(a0) */
void ScrollCmd_CameraOfsB(uint32_t *d0, uint32_t d1, uint32_t *x, uint32_t *y, uint16_t *d6, uint16_t *d7, uint32_t *bands, const u16 **list, uint32_t *layer);	/* $0098A2 regs(&d0,d1,&d2,&d3,&d6,&d7,&a1,&a5,&a6) clobbers(a0) */
