/* funcs/Scroll.h - prototypes of the Scroll module: one line per function, its register
 * convention in the tag (dkit genesis recomp bridge). First written from the measured
 * contracts (dkit disasm regs); edited by the C's authors from then on. */
#pragma once
void Scroll_SetPlaneA(uint32_t *x, uint32_t *y, uint16_t *fine, uint32_t *layer);	/* $0096B6 regs(&d0,&d1,&d2,&a6) */
void Scroll_LayerToScroll(uint32_t *x, uint32_t *y, uint16_t *fine, const MapLayer *layer);	/* $0096DE regs(&d0,&d1,&d2,a6) */
void Scroll_RunParallax(uint32_t *d0, uint32_t d1, uint32_t *d2, uint32_t *d3, uint16_t *d6, uint16_t *d7, uint32_t *bands, const u16 **list, uint32_t *layer);	/* $0097F0 regs(&d0,d1,&d2,&d3,&d6,&d7,&a1,&a5,&a6) clobbers(a0) */
void Scroll_NextCmd(uint32_t *cmd, uint32_t d1, uint32_t *d2, uint32_t *d3, uint16_t *d6, uint16_t *d7, uint32_t *bands, const u16 **list, uint32_t *layer);	/* $0097F6 regs(&d0,d1,&d2,&d3,&d6,&d7,&a1,&a5,&a6) clobbers(a0) */
void Scroll_BandsFromLayer(uint32_t *d0, uint32_t d1, uint32_t *x, uint32_t *y, uint16_t *d6, uint16_t *fine, u16 *lines, const s16 **bands, const MapLayer *layer);	/* $0098FE regs(&d0,d1,&d2,&d3,&d6,&d7,a0,&a1,a6) clobbers(a0) */
void Scroll_FillBands(uint32_t *value, uint32_t d1, uint32_t *x, uint32_t y, uint16_t *left, uint16_t *count, u16 *lines, const s16 **bands);	/* $009928 regs(&d0,d1,&d2,d3,&d6,&d7,a0,&a1) clobbers(a0) */
void Scroll_SetMode(uint16_t d0);	/* $00AE44 regs(d0) */
void Scroll_DMALinePairs(uint32_t a0, uint32_t a2);	/* $00AFDE regs(a0,a2) */
void Scroll_DMALines(uint32_t a0, uint32_t a2);	/* $00AFFC regs(a0,a2) */
void Scroll_DMALinesA(uint32_t a0, uint32_t a2);	/* $00B03E regs(a0,a2) */
void Scroll_DMALinesB(uint32_t ctrl, uint32_t scratch);	/* $00B064 regs(a0,a2) */
void Scroll_DMAColumns(uint32_t ctrl, uint32_t scratch);	/* $00B08A regs(a0,a2) */
void Scroll_DMAColumnsAB(uint32_t ctrl, uint32_t scratch);	/* $00B0A8 regs(a0,a2) */
