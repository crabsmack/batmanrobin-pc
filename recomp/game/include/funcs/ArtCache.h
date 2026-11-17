/* funcs/ArtCache.h - prototypes of the ArtCache module: one line per function, its register
 * convention in the tag (dkit genesis recomp bridge). First written from the measured
 * contracts (dkit disasm regs); edited by the C's authors from then on. */
#pragma once
void ArtCache_Init(uint16_t *blocks, uint32_t *descs, uint32_t *desc_end, uint32_t *rel_ring);	/* $008514 regs(&d1,&d2,&a2,&a3) clobbers(d0,a0,a1) */
void ArtCache_AddBlocksInline(uint16_t *count, uint16_t *ring, uint16_t *tile, uint32_t *script);	/* $008580 regs(&d0,&d1,&d2,&a6) clobbers(a0) */
void ArtCache_AddBlocks(uint16_t *count, uint16_t *ring, uint16_t *tile);	/* $008586 regs(&d0,&d1,&d2) clobbers(a0) */
void ArtCache_Alloc(uint16_t *blocks, uint32_t *rel_rd, uint32_t *desc_wr, uint32_t *desc_rd, uint32_t *blk_wr, uint32_t *blk_rd, uint32_t *spare, uint32_t handle, uint32_t *a1, uint32_t *list);	/* $0085F8 regs(&d1,&d2,&d3,&d4,&d5,&d6,&d7,a0,&a1,&a3) clobbers(d0,a0,a2) */
void ArtCache_Release(uint32_t *result, uint16_t *d1, uint32_t handle);	/* $0086AA regs(&d0,&d1,a0) clobbers(a0) */
void ArtCache_Acquire(uint16_t *blocks, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6, uint32_t *d7, uint32_t handle, uint32_t *a1, uint32_t *list, dk::Flags *ccr);	/* $0085B0 regs(&d1,&d2,&d3,&d4,&d5,&d6,&d7,a0,&a1,&a3,&ccr) ccout(NZ) clobbers(d0,a0,a2) */
