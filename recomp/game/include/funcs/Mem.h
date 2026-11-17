/* funcs/Mem.h - prototypes of the Mem module: one line per function, its register
 * convention in the tag (dkit genesis recomp bridge). First written from the measured
 * contracts (dkit disasm regs); edited by the C's authors from then on. */
#pragma once
void Mem_Clear(uint16_t *odd, uint16_t *size, u8 *dst);	/* $00999E regs(&d6,&d7,a0) clobbers(d0,a0) */
void Mem_FillRampRun(uint32_t *acc, uint32_t step, uint32_t *dst);	/* $00B232 regs(&d0,d1,&a0) */
void Mem_FillWords(uint32_t *value, uint32_t d1, uint32_t *d2, uint32_t d3, uint16_t *count, uint32_t *dst, uint32_t *a1);	/* $00C908 regs(&d0,d1,&d2,d3,&d7,&a0,&a1) */
void Mem_FillWords2Run(uint16_t value_a, uint16_t value_b, uint32_t *dst_a, uint32_t *dst_b);	/* $00CB7A regs(d0,d2,&a0,&a1) */
void Mem_FillRamp(uint32_t *acc, uint32_t step, uint32_t *d2, uint32_t d3, uint16_t count, uint32_t *dst, uint32_t *a1);	/* $00B934 regs(&d0,d1,&d2,d3,d7,&a0,&a1) clobbers(d7) */
void Mem_FillWordsRun(uint16_t value, uint32_t *dst);	/* $00C746 regs(d0,&a0) */
void Mem_FillLongsRun(uint32_t value, uint32_t *dst);	/* $00C910 regs(d0,&a0) */
void Mem_FillLongs(uint32_t value, uint32_t d1, uint32_t *d2, uint32_t d3, uint16_t *count, uint32_t *dst, uint32_t *a1);	/* $00C9F2 regs(d0,d1,&d2,d3,&d7,&a0,&a1) clobbers(d0) */
void Mem_FillWords2(uint16_t value_a, uint16_t *value_b, uint16_t *count, uint32_t *dst_a, uint32_t *dst_b);	/* $00CD7C regs(d0,&d2,&d7,&a0,&a1) */
