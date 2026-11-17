#include "game.h"

/* The boss meter's digit, DMA, sprite, start and end ($011C5E-$011D5D). */

static constexpr uint32_t Tbl_058712 = 0x058712u;   /* digit -> tile index */
static constexpr uint32_t Art_058AA2 = 0x058AA2u;   /* the HUD's tiles (a digit's mask $140 bytes on) */
static constexpr uint32_t Tbl_0080B5 = 0x0080B5u;   /* VDP VRAM write commands by first tile (loc_0080B4+1) */
static constexpr uint32_t BOSSMETER_GLYPH_MASK = 0x140;
static constexpr uint16_t BOSSMETER_SPRITE_SIZE = 0xD00;   /* 4 wide x 2 tall */
static constexpr uint16_t BOSSMETER_CENTRE_OFFSET = 0xA2;

/* BossMeter_DrawDigit - masks one 7-row digit glyph into a tile: each row long = (dest & mask) |
 * glyph.
 * In:  d0 low word = the digit, a2 = the destination (4 bytes a row)
 * Out: 7 rows rewritten; d0 low word = the glyph's tile offset, d1 = the last row, a1 past the
 *      mask's 7 rows
 * Evidence: lea Tbl_058712,a0 / add.w d0,d0 / adda.w d0,a0 / move.w (a0)+,d0 / lsl.w #5 / lea
 *           Art_058AA2,a0 / adda.w d0,a0 / lea $140(a0),a1 / 7 x (move.l (a2),d1 / and.l (a1)+ /
 *           or.l (a0)+ / move.l d1,(a2)+); called twice by BossMeter_DrawPercent
 */
void BossMeter_DrawDigit(uint32_t *d0, uint32_t *d1, uint32_t *a1, uint32_t a2)
{
    PTR(const u16) index;
    index.v = Tbl_058712 + (uint32_t)(int32_t)(int16_t)(uint16_t)(*d0 * 2);
    uint16_t offset = (uint16_t)(*index.get() << 5);
    *d0 = (*d0 & 0xFFFF0000u) | offset;
    PTR(const u32) glyph;
    glyph.v = Art_058AA2 + (uint32_t)(int32_t)(int16_t)offset;
    PTR(const u32) mask;
    mask.v = glyph.v + BOSSMETER_GLYPH_MASK;
    PTR(u32) dst;
    dst.v = a2;
    for (int row = 0; row < 7; row++) {
        *d1 = (dst.get()[row] & mask.get()[row]) | glyph.get()[row];
        dst.get()[row] = *d1;
    }
    *a1 = mask.v + 7 * 4;
}

/* BossMeter_QueueTileDMA - queues the meter's tile buffer (g_BossMeterTiles, $100 words) for DMA
 * to its VRAM block; nothing when the queue is full.
 * In:  g_BossMeterVRAM = the VDP write command
 * Out: one DMA queue entry; d0 = 0 / -1, d1 = 8
 * Evidence: moveq #1,d0 / moveq #8,d1 / jsr DMA_Reserve / bmi / $93009401 / $953296F6 / $977F /
 *           move.l (g_BossMeterVRAM).w,(a0)+ / move.w a0,(g_DMAQueuePtr).w; op $2CB2 call in
 *           Scr2_011B3E
 */
void BossMeter_QueueTileDMA(uint32_t *d0, uint32_t *d1, uint32_t a0)
{
    *d0 = 1;
    *d1 = 8;
    DMA_Reserve(d0, (uint16_t)*d1, &a0);
    if (*d0 != 0)   /* bmi: d0 = -1 */
        return;
    PTR(u16) q;
    q.v = a0;
    *q++ = 0x9300;   /* $100 words: regs $93 / $94 */
    *q++ = 0x9401;
    *q++ = 0x9532;   /* the source g_BossMeterTiles ($FFEC64) >> 1 */
    *q++ = 0x96F6;
    *q++ = 0x977F;
    uint32_t command = g_BossMeterVRAM;
    *q++ = (uint16_t)(command >> 16);
    *q++ = (uint16_t)command;
    g_DMAQueuePtr = (uint16_t)q.raw();
}

/* BossMeter_DrawSprite - writes the meter's 4x2-tile sprite straight into the SAT buffer at the
 * object's position (the link byte = the sprite count's low byte, stepped by addq.b).
 * In:  spr = the meter's sprite block (a5, +$38 attributes), self = the meter object (a6)
 * Out: one SAT entry; g_SATPtr / g_SprCount advanced; d0 = the tile word written
 * Evidence: movea.w (g_SATPtr).w,a0 / move.w $42(a6),(a0)+ / move.w (g_SprCount).w,d0 / addq.b #1 /
 *           move.w d0,(g_SprCount).w / ori.w #$D00 / move.w (g_BossMeterTile).w,d0 / or.w $38(a5) /
 *           move.w $3E(a6),(a0)+ / move.w a0,(g_SATPtr).w; op $2CB2 call in Scr2_011B3E
 */
uint16_t BossMeter_DrawSprite(const Vm2Sprite *spr, const Vm2Obj *self)
{
    PTR(u16) sat;
    sat.v = (uint32_t)(int32_t)(int16_t)(uint16_t)g_SATPtr;
    *sat++ = (uint16_t)self->y;
    uint16_t count = g_SprCount;
    count = (uint16_t)((count & 0xFF00u) | (uint8_t)(count + 1));
    g_SprCount = count;
    *sat++ = (uint16_t)(count | BOSSMETER_SPRITE_SIZE);
    uint16_t tile = (uint16_t)(g_BossMeterTile | spr->attr);
    *sat++ = tile;
    *sat++ = (uint16_t)self->x;
#if DK_MODE_NATIVE
    /* phase 6 (E3): the entry tagged for the widescreen view's pinned HUD (ws_draw.h) */
    ws_hud_tag(g_SATPtr, (uint16_t)sat.raw(), WS_HUD_METER);
#endif
    g_SATPtr = (uint16_t)sat.raw();
    return tile;
}

/* BossMeter_Init - the meter's first op: takes a 16-tile VRAM block for its tiles (ArtCache_Acquire
 * on g_BossMeterArt; a new block gives the VRAM command and first tile), and centres the meter
 * between the two players' panels.
 * In:  self = the meter object (a6); a3 (restored by the movem)
 * Out: d0 = 0 and self x = the mean of the panels' home x + $A2; d0 = 1 when no block was free;
 *      d1-d7/a1 as ArtCache_Acquire leaves them
 * Evidence: movem.l a2-a3,-(a7) / lea g_BossMeterArt,a0 / moveq #1,d1 / jsr ArtCache_Acquire / bmi
 *           / bne / move.w (a3)+,d0 / lea loc_0080B4+1,a0 / move.l (a0,d0.w),(g_BossMeterVRAM).w /
 *           move.w d0,(g_BossMeterTile).w / movem.l (a7)+,a2-a3 / (g_Player1+$A) + (g_Player2+$A) /
 *           asr.w #1 / addi.w #$A2 / move.w d0,$3E(a6) / moveq #0,d0; first op of Scr2_011B3E (op
 *           $2D30)
 */
void BossMeter_Init(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6,
                    uint32_t *d7, uint32_t *a1, uint32_t a3, Vm2Obj *self)
{
    dk::push_long(a3);   /* movem.l a2-a3,-(a7): a2 is not an input */
    dk::push_long(0);
    uint16_t blocks = 1;
    *d1 = 1;
    uint32_t list = a3;
    dk::Flags flags{};
    ArtCache_Acquire(&blocks, d2, d3, d4, d5, d6, d7, dk::addr(&g_BossMeterArt), a1, &list, &flags);
    *d1 = (*d1 & 0xFFFF0000u) | blocks;
    if (flags.n) {   /* bmi: no block free */
        dk::pop_long();
        dk::pop_long();
        *d0 = 1;
        return;
    }
    if (flags.z) {   /* a new block (bne: already resident, its command and tile kept) */
        PTR(const u16) first;
        first.v = list;
        uint16_t tile = *first.get();
        PTR(const u32) command;
        command.v = Tbl_0080B5 + (uint32_t)(int32_t)(int16_t)tile;
        g_BossMeterVRAM = *command.get();
        g_BossMeterTile = tile;
    }
    dk::pop_long();
    dk::pop_long();
    const PlayerRec *p1 = (const PlayerRec *)g_Player1, *p2 = (const PlayerRec *)g_Player2;
    uint16_t sum = (uint16_t)(p1->hud_x + p2->hud_x);
    self->x = (int16_t)((int16_t)sum >> 1) + BOSSMETER_CENTRE_OFFSET;
    *d0 = 0;
}

/* BossMeter_Free - removes the meter: clears g_BossMeterObj and releases its VRAM block.
 * In:  none
 * Out: as ArtCache_Release on g_BossMeterArt
 * Evidence: clr.w (g_BossMeterObj).w / lea g_BossMeterArt,a0 / jmp ArtCache_Release (a call here: a0,
 *           its input, is a clobber of this contract); op $2CB2 call in Scr2_011B8E before Op2_Kill
 */
void BossMeter_Free(uint32_t *d0, uint16_t *d1)
{
    g_BossMeterObj = 0;
    ArtCache_Release(d0, d1, dk::addr(&g_BossMeterArt));
}
