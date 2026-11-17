/* funcs/Span.h - prototypes of the Span module: one line per function, its register
 * convention in the tag (dkit genesis recomp bridge). First written from the measured
 * contracts (dkit disasm regs); edited by the C's authors from then on. */
#pragma once
void Span_ClearLinesRun(uint16_t hscroll, uint16_t *vscroll, uint32_t *dst_h, uint32_t *dst_v);	/* $00D2DC regs(d0,&d2,&a0,&a1) */
void Span_ClearLines(uint16_t hscroll, uint16_t *vscroll, uint16_t *count, uint32_t *dst_h, uint32_t *dst_v);	/* $00D81E regs(d0,&d2,&d7,&a0,&a1) */
void Span_FillRampsDither0Run(uint32_t *d0, uint32_t d1, uint32_t *d2, uint32_t d3, uint32_t *d4, uint16_t d5, uint16_t d6, uint32_t *a0, uint32_t *a1);	/* $00DAB4 regs(&d0,d1,&d2,d3,&d4,d5,d6,&a0,&a1) */
void Span_FillRampsDither1Run(uint32_t *h, uint32_t h_step, uint32_t *v, uint32_t v_step, uint32_t *last, uint16_t clear, uint16_t set, uint32_t *dst_h, uint32_t *dst_v);	/* $00EAA2 regs(&d0,d1,&d2,d3,&d4,d5,d6,&a0,&a1) */
void Span_ClearScreen(uint32_t *hscroll, uint32_t *vscroll, uint16_t *lines, uint32_t a0, uint32_t a1);	/* $00F96A regs(&d0,&d2,&d7,a0,a1) clobbers(a0,a1) */
void Span_DrawDiscDither(uint16_t radius, uint16_t centre_x, uint16_t centre_y, uint16_t *edge, uint16_t *vsum, uint16_t *clear, uint32_t *line, uint16_t *rows, uint32_t h_lines, uint32_t v_lines, uint32_t *top, uint32_t *bottom);	/* $00FE0E regs(d0,d1,d2,&d3,&d4,&d5,&d6,&d7,a0,a1,&a2,&a3) clobbers(d0,d2,a0,a1) */
void Span_ClearDrawDisc(uint16_t radius, uint16_t centre_x, uint16_t centre_y, uint16_t *edge, uint16_t *vsum, uint16_t *clear, uint32_t *line, uint16_t *rows, uint32_t h_lines, uint32_t v_lines, uint32_t *top, uint32_t *bottom);	/* $01010A regs(d0,d1,d2,&d3,&d4,&d5,&d6,&d7,a0,a1,&a2,&a3) clobbers(d0,d2,a0,a1) */
void Span_BuildCircle(uint16_t radius, uint32_t *err, uint32_t *step, uint16_t *dstep);	/* $010122 regs(d0,&d2,&d3,&d4) clobbers(d0,d1,a0,a1) */
void Span_FillRampsRun(uint32_t *h, uint32_t h_step, uint32_t *v, uint32_t v_step, uint32_t *h_dst, uint32_t *v_dst);	/* $00B93C regs(&d0,d1,&d2,d3,&a0,&a1) */
void Span_FillRamps(uint32_t *h, uint32_t h_step, uint32_t *v, uint32_t v_step, uint16_t *count, uint32_t *h_dst, uint32_t *v_dst);	/* $00C73E regs(&d0,d1,&d2,d3,&d7,&a0,&a1) */
void Span_FillRampsDither0(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t d3, uint32_t *d4, uint16_t *d5, uint32_t *d7, uint32_t *a0, uint32_t *a1);	/* $00E7EE regs(&d0,&d1,&d2,d3,&d4,&d5,&d7,&a0,&a1) clobbers(d6) */
void Span_FillRampsDither1Odd(uint32_t *h, uint32_t h_step, uint32_t *v, uint32_t v_step, uint32_t *last, uint16_t clear, uint16_t set, uint32_t *pairs, uint32_t *dst_h, uint32_t *dst_v);	/* $00E80A regs(&d0,d1,&d2,d3,&d4,d5,d6,&d7,&a0,&a1) */
void Span_FillRampsDither1(uint32_t *h, uint32_t *h_step, uint32_t *v, uint32_t v_step, uint32_t *last, uint16_t *clear, uint32_t *lines, uint32_t *dst_h, uint32_t *dst_v);	/* $00F7DC regs(&d0,&d1,&d2,d3,&d4,&d5,&d7,&a0,&a1) clobbers(d6) */
void Span_DrawTriangle(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6, uint32_t *d7, uint32_t *a0, uint32_t *a1, uint32_t *a2, uint32_t *a3);	/* $00F976 regs(&d0,&d1,&d2,&d3,&d4,&d5,&d6,&d7,&a0,&a1,&a2,&a3) */
void Span_ClearScreenAlt(uint32_t *hscroll, uint32_t *vscroll, uint32_t *lines, uint32_t a0, uint32_t a1);	/* $00FBA4 regs(&d0,&d2,&d7,a0,a1) clobbers(a0,a1) */
void Span_DrawTriangleDither(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d4, uint32_t *d5, uint32_t d6, uint32_t *d7, uint32_t *a0, uint32_t *a1);	/* $00FBB0 regs(&d0,&d1,&d2,&d4,&d5,d6,&d7,&a0,&a1) clobbers(d3,d6,a2,a3) */
void Span_FillRampsDither0Odd(uint32_t *h, uint32_t *h_step, uint32_t *v, uint32_t v_step, uint32_t *last, uint16_t *clear, uint16_t set, uint32_t *pairs, uint32_t *dst_h, uint32_t *dst_v);	/* $00F7F8 regs(&d0,&d1,&d2,d3,&d4,&d5,d6,&d7,&a0,&a1) clobbers(d6) */
