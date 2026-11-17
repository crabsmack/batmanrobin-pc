#include "game.h"

enum {
    REVEAL_TILE_BASE = 0x4CA,      /* the picture's sprite tiles in VRAM (tile number base) */
    REVEAL_NAME_BASE = 0x637C,     /* the plane-B name word base (palette, priority, tile base) */
    REVEAL_MAP_COLUMNS = 0x28,     /* the cell map: 40 columns (plane B's name table: 128) */
    REVEAL_SCREEN_OFS = 0x80,      /* sprite coordinates: screen + $80 */
    REVEAL_SCREEN_BOTTOM = 0x170,  /* a piece below this (sprite y) is deleted */
    REVEAL_SPRITE_LIMIT = 0x3C,    /* the SAT holds at most 60 sprites here */
    LINE3_COLOURS = 15,            /* palette line 3 colours 1-15 */
    LINE3_RED_MAX = 0xE, LINE3_GREEN_MAX = 0xE0, LINE3_BLUE_MAX = 0xE00,
};

/* the word at byte index i of the 68000 buffer at address base (the picture's maps: a header
   word pair, then the cells) */
static uint16_t map_word(uint32_t base, int16_t i)
{
    PTR(const u16) w;
    w.v = base + 4 + (uint32_t)(int32_t)i;
    return *w;
}

/* Stage3_RevealPlaceTile - a4 native right after the piece script's Op4_Spawn: places the new
 * piece on its 8x8 cell, gives it the picture's sprite tile, marks the cell taken in the cell map
 * and queues the plane-B name-table write of the cell for Stage3_RevealVIntFlush
 * In:  piece = the new piece (a5; cell column/row in its x/y fraction words); g_BufPtrs+4 / +$10
 *      = the picture's maps (128 / 40 cells wide); g_PlaneBVRAM
 * Out: piece x/y = cell * 8 + camera, tile; the cell map word = 1; one queued write; d0 = the
 *      cell's name-table byte offset, d1 = the name word, d2 = the VDP command, d4/d5 = the
 *      piece's x/y; the condition codes of the name word (move.w d1,(a0)+)
 * Evidence: move.w $30(a5),d4 / $34(a5),d5 / mulu.w #$28 / asl.w #7 / move.w #1,(a0,d1.w) /
 *           movea.l (g_BufPtrs+4).w / addi.w #$4CA / movea.l (g_BufPtrs+$10).w / addi.w #$637C /
 *           lsl.l #2 / lsr.w #2 / ori.w #$4000 / swap / adda.w (g_SceneVars+$A).w / addq.w #6;
 *           Op4_CallNative in Scr4_03CE3E; TAS f97917-f100672 (453 frames)
 */
void Stage3_RevealPlaceTile(uint16_t *d0, uint32_t *d1, uint32_t *d2, uint16_t *d4, uint16_t *d5,
                            RevealPiece *piece, dk::Flags *ccr)
{
    uint16_t col = piece->cell_col;
    uint16_t row = piece->cell_row;
    uint32_t cell = (uint32_t)row * REVEAL_MAP_COLUMNS;        /* mulu.w: the long result */
    uint16_t name_ofs = (uint16_t)(row << 7);                  /* plane B is 128 cells wide */
    name_ofs = (uint16_t)(name_ofs + col);
    cell = (cell & 0xFFFF0000u) | (uint16_t)(cell + col);
    name_ofs = (uint16_t)(name_ofs * 2);
    cell = (cell & 0xFFFF0000u) | (uint16_t)(cell * 2);
    uint16_t x = (uint16_t)((col << 3) + (uint16_t)(g_CameraX >> 16));
    uint16_t y = (uint16_t)((row << 3) + (uint16_t)(g_CameraY >> 16));
    piece->x = (int16_t)x;
    piece->y = (int16_t)y;

    PTR(u16) mark;
    mark.v = dk::addr(g_SceneWorkBuf) + (uint32_t)(int32_t)(int16_t)cell;
    *mark = 1;                                                  /* the cell is taken */

    const u32 *bufs = (const u32 *)g_BufPtrs;
    uint16_t tile = (uint16_t)(map_word(bufs[1], (int16_t)name_ofs) + REVEAL_TILE_BASE);
    piece->tile = tile;
    uint16_t name = (uint16_t)(map_word(bufs[4], (int16_t)cell) + REVEAL_NAME_BASE);
    cell = (cell & 0xFFFF0000u) | name;

    /* the VDP write command of plane B's cell: address bits 15-14 in the low word */
    uint32_t cmd = (uint16_t)(g_PlaneBVRAM + name_ofs);
    cmd <<= 2;
    cmd = (cmd & 0xFFFF0000u) | (uint16_t)((uint16_t)cmd >> 2);
    cmd |= 0x4000;
    cmd = cmd << 16 | cmd >> 16;

    ClearingReveal *rv = clearing_reveal();
    PTR(RevealCellWrite) slot;
    slot.v = dk::addr(rv->queue) + (uint32_t)(int32_t)(int16_t)rv->queue_bytes;
    rv->queue_bytes = (uint16_t)(rv->queue_bytes + sizeof(RevealCellWrite));
    slot->vdp_cmd = cmd;
    slot->name = name;

    *d0 = name_ofs;
    *d1 = cell;
    *d2 = cmd;
    *d4 = x;
    *d5 = y;
    *ccr = dk::Flags::test(name, 2, ccr->x);
}

/* Stage3_RevealDrawTile - a4 native draw op of a reveal piece: puts it straight into the SAT
 * buffer as one 8x8 hardware sprite at its screen position; deletes the piece (the script's
 * return dropped, Op4_Kill) when it is below the screen or the SAT already holds 60 sprites
 * In:  piece = the piece (a6: tile); d4/d5 = its world x/y (from the motion op before it)
 * Out: drawn: one SAT entry at g_SATPtr (advanced), g_SprCountLo; d4/d5 = the sprite x/y, d6 =
 *      the link word, a0 = past the entry; the condition codes of the new count byte. Deleted:
 *      Op4_Kill's results
 * Evidence: sub.w (g_CameraX).w,d4 / addi.w #$80 / cmpi.w #$170,d5 / bcs / addq.l #4,a7 / jmp
 *           Op4_Kill / move.w (g_SprCount).w,d6 / cmpi.b #$3C,d6 / bge / movea.w (g_SATPtr).w,a0 /
 *           four move.w (a0)+ / move.b d6,(g_SprCountLo).w; Op4_CallNative dc.l $03CF72 six times
 *           in Scr4_03CE3E; 3 corpus runs
 */
void Stage3_RevealDrawTile(uint16_t *d0, uint16_t *d2, uint16_t *d4, uint16_t *d5, uint16_t *d6, uint32_t *a0,
                           uint32_t *a1, const RevealPiece *piece, dk::Flags *ccr)
{
    *d4 = (uint16_t)(*d4 - (uint16_t)(g_CameraX >> 16) + REVEAL_SCREEN_OFS);
    *d5 = (uint16_t)(*d5 - (uint16_t)(g_CameraY >> 16) + REVEAL_SCREEN_OFS);
    if (*d5 >= REVEAL_SCREEN_BOTTOM) {         /* bcs not taken: unsigned */
        dk::drop_return();
        dk::jump(at_Op4_Kill);
        return;
    }
    uint16_t count = g_SprCount;
    *d6 = count;
    if ((int8_t)(uint8_t)count >= REVEAL_SPRITE_LIMIT) {   /* cmpi.b / bge: signed byte */
        dk::drop_return();
        dk::jump(at_Op4_Kill);
        return;
    }
    PTR(SprSatEntry) sat;
    sat.v = (uint32_t)(int32_t)(int16_t)(uint16_t)g_SATPtr;   /* movea.w: sign-extended */
    SprSatEntry *e = sat;
    uint8_t link = (uint8_t)(count + 1);
    count = (uint16_t)((count & 0xFF00u) | link);
    e->y = *d5;
    e->size = (uint8_t)(count >> 8);
    e->link = link;
    e->attr = piece->tile;
    e->x = *d4;
    e++;
    *d6 = count;
    *a0 = ADDR68(e);
    g_SATPtr = (uint16_t)ADDR68(e);
    g_SprCountLo = link;
    *ccr = dk::Flags::test(link, 1, ccr->x);
}

/* Stage3_InitLine3Cycle - sets the start of the clearing's palette-line-3 colour walker: red $A,
 * green $60, blue $E00, steps 2 / $20 / -$200
 * In:  -
 * Out: g_SceneVars2+$22..+$2D; d0-d5 (low words) = those six values
 * Evidence: move.w #$A,d0 / #$60,d1 / #$E00,d2 / #2,d3 / #$20,d4 / #$FE00,d5 / movem.w
 *           d0-d5,(g_SceneVars2+$22).w; Op2_CallNativeRel at $03CBB4; TAS f97888
 */
void Stage3_InitLine3Cycle(uint16_t *d0, uint16_t *d1, uint16_t *d2, uint16_t *d3, uint16_t *d4, uint16_t *d5)
{
    ClearingColourCycle *cc = clearing_colours();
    *d0 = 0xA;
    *d1 = 0x60;
    *d2 = 0xE00;
    *d3 = 2;
    *d4 = 0x20;
    *d5 = 0xFE00;
    cc->red = *d0;
    cc->green = *d1;
    cc->blue = *d2;
    cc->red_step = *d3;
    cc->green_step = *d4;
    cc->blue_step = *d5;
}

/* Stage3_StepLine3Cycle - per-frame colour cycle of palette line 3: advances the walker one step,
 * then fills colours 1-15 of g_PalSource3 with the next 15 steps and flags the line for reload
 * In:  the walker (g_SceneVars2+$22..+$2D, Stage3_InitLine3Cycle); d6's upper word stays (the
 *      colours are built in the low word)
 * Out: g_PalSource3+2..+$1F, g_Pal3Reload = $FF; the walker one step on; d0-d5 = the walker 16
 *      steps on (the movem.w load sign-extended the upper words), d6 = the last colour, d7 =
 *      $FFFF (dbra)
 * Evidence: lea (g_PalSource3+2).w,a0 / movem.w (g_SceneVars2+$22).w,d0-d5 / bsr
 *           Stage3_Line3CycleStep / movem.w d0-d5 back / moveq #$E,d7 / or.w / move.w d6,(a0)+ /
 *           bsr / dbra / st.b (g_Pal3Reload).w; Op2_CallNativeRel at $03CBCC; 5 corpus runs
 */
void Stage3_StepLine3Cycle(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5,
                           uint16_t *d6, uint32_t *d7)
{
    ClearingColourCycle *cc = clearing_colours();
    uint16_t v[6] = { cc->red, cc->green, cc->blue, cc->red_step, cc->green_step, cc->blue_step };
    uint32_t upper[6];
    for (int i = 0; i < 6; i++)   /* movem.w into data registers: sign-extended from the loaded words */
        upper[i] = (uint32_t)(int32_t)(int16_t)v[i] & 0xFFFF0000u;
    Stage3_Line3CycleStep(&v[0], &v[1], &v[2], &v[3], &v[4], &v[5]);
    cc->red = v[0];
    cc->green = v[1];
    cc->blue = v[2];
    cc->red_step = v[3];
    cc->green_step = v[4];
    cc->blue_step = v[5];
    u16 *colour = (u16 *)(g_PalSource3 + 2);
    for (int i = 0; i < LINE3_COLOURS; i++) {
        *d6 = (uint16_t)(v[0] | v[1] | v[2]);
        *colour++ = *d6;
        Stage3_Line3CycleStep(&v[0], &v[1], &v[2], &v[3], &v[4], &v[5]);
    }
    *d7 = 0x0000FFFFu;   /* moveq #$E / dbra: the low word ends at -1 */
    g_Pal3Reload = 0xFF;
    uint32_t *out[6] = { d0, d1, d2, d3, d4, d5 };
    for (int i = 0; i < 6; i++)
        *out[i] = upper[i] | v[i];
}
