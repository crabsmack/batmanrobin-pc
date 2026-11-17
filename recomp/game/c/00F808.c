#include "game.h"

/* The mask column: 7 sprites of 2x4 cells (16x32 pixels) stacked at screen x 0 from y 0 down,
 * high priority, palette 0, tiles from $5D8 (VRAM $BB00): the 8 solid colour-6 tiles of
 * Art_00F86A, DMA'd there once. */
static constexpr int MASK_SPRITES = 7;
static constexpr uint32_t MASK_FIRST_POS = 0x00600700u;   /* y $60 (one step before $80), size 7 */
static constexpr uint32_t MASK_POS_STEP = 0x00200001u;    /* y + 32, link + 1 */
static constexpr uint32_t MASK_TILE_X = 0x85D80080u;      /* attr $85D8, x $80 */

/* Spr_MaskLeftColumn - when the scene asks for it (g_SceneFixedSpr), blanks the leftmost 16 screen
 * pixels (the column the 2-cell column V-scroll can't scroll cleanly): queues the solid tile block
 * once, then appends a column of 7 high-priority sprites at screen x 0 to the SAT buffer every
 * frame.
 * In:  g_SATPtr / g_SprCountLo = the SAT write position and link counter
 * Out: pos = the last entry's y/size/link long (d0), tile = $85D80080 (d1), count = $FFFF (the
 *      dbra's end over moveq #6, d7); when the art's DMA can't be reserved: pos = -1 (DMA_Reserve),
 *      tile = 8, count unchanged; with the flag clear nothing changes
 * Evidence: tst.b g_SceneFixedSpr / tst.b g_LeftMaskArtSent / moveq #1,d0 / moveq #8,d1 /
 *           bsr DMA_Reserve / bne; the queue entry $93809400 $9535967C $9700 $7B000082 ($80
 *           words from $00F86A to VRAM $BB00); move.l #$600700,d0 / move.b g_SprCountLo,d0 /
 *           addi.l #$200001,d0 x 7; called by Frame_Begin / Frame_BeginStage every frame
 */
void Spr_MaskLeftColumn(uint32_t *pos, uint32_t *tile, uint32_t *count, uint32_t a0)
{
    (void)a0;   /* clobbered: the queue and SAT pointers */
    if (!g_SceneFixedSpr)
        return;
    if (!g_LeftMaskArtSent) {
        *pos = 1;                  /* one queue entry */
        *tile = 8;                 /* of this frame's DMA budget */
        uint32_t entry = 0;
        DMA_Reserve(pos, 8, &entry);
        if (*pos != 0)             /* d0 = -1: no room this frame; try again next frame */
            return;
        PTR(u16) q;
        q.v = entry;
        u16 *w = q;
        *w++ = 0x9380;             /* length $0080 words */
        *w++ = 0x9400;
        *w++ = 0x9535;             /* source Art_00F86A >> 1 = $007C35 */
        *w++ = 0x967C;
        *w++ = 0x9700;
        *w++ = 0x7B00;             /* VRAM write $BB00 + DMA */
        *w++ = 0x0082;
        g_DMAQueuePtr = (uint16_t)ADDR68(w);
        g_LeftMaskArtSent = 0xFF;
    }
#if DK_MODE_NATIVE
    spr_host_cover();   /* phase 6 (E5): a frame with the cover drawn (a counter) */
#endif
    PTR(SprSatEntry) sat;
    sat.v = (uint32_t)(int32_t)(int16_t)(uint16_t)g_SATPtr;   /* movea.w: sign-extended */
    SprSatEntry *e = sat;
    uint32_t p = (MASK_FIRST_POS & 0xFFFFFF00u) | (uint8_t)g_SprCountLo;
    for (int i = 0; i < MASK_SPRITES; i++, e++) {
        p += MASK_POS_STEP;
        e->y = (uint16_t)(p >> 16);
        e->size = (uint8_t)(p >> 8);
        e->link = (uint8_t)p;
        e->attr = (uint16_t)(MASK_TILE_X >> 16);
        e->x = (uint16_t)MASK_TILE_X;
    }
    g_SprCountLo = (uint8_t)p;
    g_SATPtr = (uint16_t)ADDR68(e);
    *pos = p;
    *tile = MASK_TILE_X;
    *count = 0x0000FFFFu;
}
