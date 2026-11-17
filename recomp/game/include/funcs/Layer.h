/* funcs/Layer.h - prototypes of the Layer module: one line per function, its register
 * convention in the tag (dkit genesis recomp bridge). First written from the measured
 * contracts (dkit disasm regs); edited by the C's authors from then on. */
#pragma once
void Layer_SetPlanes64x64(void);	/* $012572 regs() */
void Layer_SetAnimTiles(uint16_t tile, const s32 *header, MapLayer *layer);	/* $0126D2 regs(d0,a0,a6) clobbers(d0,a0,a1) */
void Layer_Init(uint16_t *tile, uint16_t *d1, uint32_t *d2, uint16_t *d6, const s32 *header, uint32_t *a1, uint32_t *a2, uint32_t *a3, uint32_t *a4, MapLayer *layer);	/* $0127D0 regs(&d0,&d1,&d2,&d6,a0,&a1,&a2,&a3,&a4,a6) clobbers(a0) */
void Layer_MoveX(uint16_t dx, MapLayer *layer);	/* $01289C regs(d0,a6) clobbers(d0,d1,d2) */
void Layer_MoveY(uint16_t *dy, uint16_t *d1, uint16_t *d2, MapLayer *layer);	/* $0128F0 regs(&d0,&d1,&d2,a6) */
void Layer_ScrollStep(uint32_t *d1, uint32_t *d2, uint32_t *d3, uint16_t *d4, uint32_t *d5, uint16_t *d6, uint16_t *d7, uint32_t *a0, uint32_t *a2, uint32_t *a3, MapLayer *layer);	/* $012A28 regs(&d1,&d2,&d3,&d4,&d5,&d6,&d7,&a0,&a2,&a3,a6) clobbers(d0) */
void Layer_ScrollStepX(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint16_t *d4, uint32_t *d5, uint16_t *d6, uint16_t *d7, uint32_t *a0, uint32_t *a2, uint32_t *a3, MapLayer *layer);	/* $012A30 regs(&d0,&d1,&d2,&d3,&d4,&d5,&d6,&d7,&a0,&a2,&a3,a6) */
void Layer_QueueColumn(uint16_t step, uint32_t *d1, uint16_t *d2, uint32_t *d3, uint16_t *d4, uint32_t *d5, uint16_t *d6, uint16_t *d7, uint32_t *a0, uint32_t *a2, uint32_t *a3, uint32_t a6);	/* $012A84 regs(d0,&d1,&d2,&d3,&d4,&d5,&d6,&d7,&a0,&a2,&a3,a6) clobbers(d0) */
u16 *Layer_ResetTileQueue(void);	/* $012CC6 regs()->a4 clobbers(a3) */
void Layer_FlushTileQueue(uint16_t *displacement, uint32_t ctrl, uint32_t data, u16 **queue, u16 **old_end);	/* $012CDA regs(&d0,a0,a1,&a3,&a4) clobbers(d1) */
void Layer_WriteTiles(uint32_t ctrl, uint32_t data, const u16 **queue);	/* $012D06 regs(a0,a1,&a3) clobbers(d0,d1) */
void Layer_SetPlanes128x32(void);	/* $0125A4 regs() */
void Layer_AnimateTiles(uint16_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint16_t *d5, uint32_t a0, uint32_t *art, uint32_t *entry, MapLayer *layer);	/* $0126F2 regs(&d1,&d2,&d3,&d4,&d5,a0,&a4,&a5,a6) clobbers(d0,a0) */
void Layer_MoveYWithCamera(uint16_t dy, uint32_t layer);	/* $0128EA regs(d0,a6) clobbers(d0,d1,d2) */
void Layer_DrawScreen(uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6, uint32_t d7, uint32_t *a3, uint32_t *a4, uint32_t a5, uint32_t *layer);	/* $012942 regs(&d2,&d3,&d4,&d5,&d6,d7,&a3,&a4,a5,&a6) clobbers(d0,d1,d7,a0,a1,a2,a5) */
void Layer_ScrollStepY(uint32_t *d1, uint32_t *d2, uint32_t *d3, uint16_t *d4, uint32_t *d5, uint16_t *d6, uint16_t *d7, uint32_t *a0, uint32_t *a2, uint32_t *a3, uint32_t a6);	/* $012B74 regs(&d1,&d2,&d3,&d4,&d5,&d6,&d7,&a0,&a2,&a3,a6) clobbers(d0) */
void Layer_FlushTileQueueIfIdle(uint32_t d1, uint32_t d2, uint32_t d3, uint32_t d4, uint32_t d5, uint32_t d6, uint32_t d7, uint32_t *a0, uint32_t *a1, uint32_t *a2, uint32_t a5, uint32_t *a6);	/* $012CBE regs(d1,d2,d3,d4,d5,d6,d7,&a0,&a1,&a2,a5,&a6) clobbers(d0,d1,d2,d4,d5,d6,d7,a3,a4,a5) */
void Layer_WriteTilesStart(int16_t displacement, uint32_t ctrl, uint32_t data, const u16 **queue);	/* $012DBA regs(d0,a0,a1,&a3) clobbers(d0,d1) */
void Layer_WriteTilesEnd(void);	/* $012DC0 regs() */
