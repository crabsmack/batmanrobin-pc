#include "game.h"

/* Spr_EmitClipped ($007D76-$007EB7).
 *
 * The same piece loop as Spr_BuildSAT (four copies in the original, one per flip combination),
 * with the clip window and bias from RAM (g_SprDirectMaxY/MaxX, g_SprDirectBiasY/BiasX: the scene
 * init sets Spr_BuildSAT's $100/$160 and $60). */

static uint32_t sext16(uint16_t w) { return (uint32_t)(int32_t)(int16_t)w; }
static uint32_t swap_halves(uint32_t v) { return (v << 16) | (v >> 16); }
static void set_lo(uint32_t *r, uint16_t v) { *r = (*r & 0xFFFF0000u) | v; }

template <class T> static T *at(uint32_t a)
{
    PTR(T) p;
    p.v = a;
    return p;
}

#if DK_MODE_NATIVE
/* Phase 6 (ruling R11, criterion E3): with widescreen on, a piece past g_SprDirectMaxX that the
 * wide view shows goes to ws_draw as a margin piece (drawn by the host outside the 4:3 view's
 * columns; ws_draw.h). Only with the scene's standard window ($160 / bias $60: Spr_BuildSAT's), so
 * a deliberately narrowed window stays narrow. Nothing the game sees changes. (py, px) are the
 * piece's y and x after the offset, before the bias, as the loop has them. */
static void ws_direct_piece(uint16_t py, uint16_t px, const SprPiece *p, uint16_t attr, const ArtDesc *d)
{
    if (!ws_draw_on() || g_SprDirectMaxX != 0x160 || g_SprDirectBiasX != 0x60 || g_SprDirectBiasY != 0x60)
        return;
    /* a pinned HUD item (the GO! sign, pinned to the right edge: x + 40) has its clipped pieces
       (x >= 320) past the wide view's right edge: not drawn */
    if (ws_hud_direct_kind())
        return;
    int sx = (int16_t)px - 0x20, sy = (int16_t)py - 0x20;
    if (sx < -112 || sx >= 400)
        return;
    WsDrawPiece m;
    m.x = (int16_t)sx;
    m.y = (int16_t)sy;
    m.size = (uint8_t)((uint16_t)p->size >> 8);
    m.from_mem = 0;
    m.src = 0;
    const u16 *tile = (const u16 *)((const u8 *)d->tiles + (uint8_t)p->tile_slot);
    m.attr = (uint16_t)((uint16_t)*tile + (uint16_t)((attr & 0xFF00u) | (uint8_t)p->tile_add));
    ws_draw_piece(&m);
}
#endif

/* Spr_EmitClipped - emits the pieces of one frame into the SAT buffer, dropping those outside the
 * window g_SprDirectMaxY/MaxX and biasing the rest by g_SprDirectBiasY/BiasX; stops at 80 sprites.
 * In:  entry = the frame-table entry (a0); sat = the SAT write pointer (a1); count = the sprite
 *      count (d2.w); slot = 0 (d3.w); x/y (d4/d5); attr (d6.w: bits 11/12 the flips)
 * Out: sat and count advanced; d0 = the last piece's y long with the tile (or y) in its low word,
 *      d1 = its x long (flipped, the low word biased when drawn; as it came when no piece was read),
 *      slot/attr = the last piece's tile slot / tile byte in their low bytes, d7 = the piece
 *      counter as left, entry = past the pieces read, art = the descriptor (a2). Nothing when the
 *      count is 80 already; entry + 2 and art = the handle's address when the art is not resident
 * Evidence: cmpi.b #$50,d2 / beq, movea.w (a0)+,a2 / move.w (a2),d0 / beq, movea.l (a0),a0 /
 *           move.w (a0)+,d7, btst #$B / #$C, the loops' cmp.w (g_SprDirectMaxY).w,d0 / bcc and
 *           add.w (g_SprDirectBiasY).w,d0; only caller Spr_DrawDirect
 */
void Spr_EmitClipped(uint32_t *d0, uint32_t *d1, uint16_t *count, uint16_t *slot, uint16_t x, uint16_t y, uint16_t *attr,
                     uint16_t *d7, uint32_t *entry, uint32_t *sat, uint32_t *art)
{
    if ((uint8_t)*count == SPR_MAX_SPRITES)
        return;
    const SprFrame *frame = at<const SprFrame>(*entry);
    *art = sext16(frame->art_handle);
    *entry += 2;
    set_lo(d0, *at<u16>(*art));
    if ((uint16_t)*d0 == 0)
        return;                                       /* the art is not in VRAM */
    *art = sext16((uint16_t)*d0);
    const u16 *list = at<const u16>(frame->pieces);
    *d7 = *list;
    const SprPiece *piece = (const SprPiece *)(list + 1);
    bool hflip = (*attr & SPR_ATTR_HFLIP) != 0;
    bool vflip = (*attr & SPR_ATTR_VFLIP) != 0;
    for (;;) {
        const SprPiece *p = piece++;
        *d0 = ((uint32_t)(uint16_t)p->y_vflip << 16) | (uint16_t)p->y;
        *d1 = ((uint32_t)(uint16_t)p->x_hflip << 16) | (uint16_t)p->x;
        *count = (uint16_t)((uint16_t)(int16_t)(int8_t)(uint8_t)*count | p->size);   /* ext.w / or.w */
        *slot = (uint16_t)((*slot & 0xFF00u) | p->tile_slot);
        *attr = (uint16_t)((*attr & 0xFF00u) | p->tile_add);
        if (vflip)
            *d0 = swap_halves(*d0);
        if (hflip)
            *d1 = swap_halves(*d1);
        set_lo(d0, (uint16_t)((uint16_t)*d0 + y));
        bool drawn = false;
        if ((uint16_t)*d0 < g_SprDirectMaxY) {
            set_lo(d1, (uint16_t)((uint16_t)*d1 + x));
            if ((uint16_t)*d1 < g_SprDirectMaxX) {
                set_lo(d0, (uint16_t)((uint16_t)*d0 + g_SprDirectBiasY));
                set_lo(d1, (uint16_t)((uint16_t)*d1 + g_SprDirectBiasX));
                *count = (uint16_t)(*count + 1);
                SprSatEntry *e = at<SprSatEntry>(*sat);
                e->y = (uint16_t)*d0;
                e->size = (uint8_t)(*count >> 8);    /* the piece size; link = the new sprite count */
                e->link = (uint8_t)*count;
                const u16 *tile = (const u16 *)((const u8 *)at<ArtDesc>(*art)->tiles + (int16_t)*slot);
                set_lo(d0, (uint16_t)(*tile + *attr));
                e->attr = (uint16_t)*d0;
                e->x = (uint16_t)*d1;
                *sat += sizeof(SprSatEntry);
                drawn = true;
            }
#if DK_MODE_NATIVE
            else
                ws_direct_piece((uint16_t)*d0, (uint16_t)*d1, p, *attr, at<const ArtDesc>(*art));
#endif
        }
        if (drawn && (uint8_t)*count == SPR_MAX_SPRITES)
            break;                                    /* dbeq: the 80th sprite */
        if ((*d7 = (uint16_t)(*d7 - 1)) == 0xFFFF)
            break;
    }
    *entry = ADDR68(piece);
}
