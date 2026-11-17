/* funcs/Debug.h - prototypes of the Debug module: one line per function, its register
 * convention in the tag (dkit genesis recomp bridge). First written from the measured
 * contracts (dkit disasm regs); edited by the C's authors from then on. */
#pragma once
void Debug_PauseMenu(const u8 *pad, uint32_t *player);	/* $01569C regs(a0,&a1) clobbers(d0) */
void Debug_PlacePlayerAtCursor(uint32_t *player);	/* $0156FC regs(&a1) */
void Debug_FreeCamStep(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6, uint32_t *d7, uint32_t *a0, uint32_t *a1, uint32_t *a2, uint32_t a3, uint32_t a4, uint32_t a5, uint32_t *a6);	/* $015730 regs(&d0,&d1,&d2,&d3,&d4,&d5,&d6,&d7,&a0,&a1,&a2,a3,a4,a5,&a6) */
void Debug_FreeCamScroll(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *a6);	/* $015916 regs(&d0,&d1,&d2,&a6) */
void Debug_DrawHexBytes(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint16_t *d4, uint16_t *d5, uint16_t *d6, uint16_t *d7, uint32_t *a0, uint32_t *a1, uint32_t *a2, uint32_t *a6);	/* $015940 regs(&d0,&d1,&d2,&d3,&d4,&d5,&d6,&d7,&a0,&a1,&a2,&a6) */
void Debug_CentreCursor(void);	/* $015716 regs() clobbers(d0) */
void Debug_FreeCamSyncLayer(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *a6);	/* $01592A regs(&d0,&d1,&d2,&a6) */
