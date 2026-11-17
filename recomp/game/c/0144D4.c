#include "game.h"

/* The text-map screen helpers ($0144D4-$01458B): clearing the plane image, queueing its DMA to
 * VRAM, and moving a menu cursor. */

enum {
    TILE_BLANK_PAIR = 0x05FC05FCu,   /* two blank font tiles (tile $5FC) */
    IMAGE_ROWS = 28,
    ROW_WORDS = 40,
};

/* the low word of a data register, the upper word kept (a .w move) */
static inline void set_low_word(uint32_t *r, uint16_t v) { *r = (*r & 0xFFFF0000u) | v; }

/* TextMap_Clear - fills the 40x28-word plane image g_SceneWorkBuf with the blank tile $5FC.
 * In:  none
 * Out: d0 = $05FC05FC (the flags as its last move.l: tst(d0.l)); d7 = $FFFF after the dbra (its
 *      upper word 0, moveq)
 * Evidence: lea (g_SceneWorkBuf).l,a0 / move.l #$5FC05FC,d0 / moveq #$37,d7 / 10 x move.l d0,(a0)+ /
 *           dbra: 56 x 10 longs = 1120 words = 40x28; called by the menu scenes before their first
 *           TextMap_Run ($499B6, $499E8); scenario-ending
 */
void TextMap_Clear(uint32_t *d0, uint32_t *d7)
{
    *d0 = TILE_BLANK_PAIR;
    u32 *image = (u32 *)g_SceneWorkBuf;
    for (int i = 0; i < IMAGE_ROWS * ROW_WORDS / 2; i++)
        image[i] = TILE_BLANK_PAIR;
    *d7 = 0x0000FFFFu;
}

/* TextMap_UploadRows - queues DMAs of rows of the plane image to VRAM, 40 words a row, the
 * plane's row stride apart; stops early when the DMA queue or this frame's budget is short.
 * In:  d2 = the VRAM address (low word), d7 = rows - 1
 * Out: d2 = the VDP write command of the row after the last queued; d3/d4/d5 = the DMA source
 *      registers ($95/$96/$97) of the next row; d6 = 4 x g_PlaneRowBytes; d7 = the dbra count left
 *      ($FFFF when all went); d0/d1 = DMA_Reserve's (d0 = -1 on the early stop)
 * Evidence: andi.l #$FFFF,d2 / lsl.l #2 / lsr.w #2 / swap / ori.l #$40000080 (VDP write command +
 *           DMA); move.w #$9500/$96C8/$977F (source $FF9000 >> 1); DMA_Reserve(1, 3) / bmi; queue
 *           entry $93289400 (length $28 words), d3-d5, d2; move.w a0,(g_DMAQueuePtr).w; addi.b
 *           #$28,d3 with the carry into d4, d5; $49212 sends 3 rows to g_PlaneBVRAM + $C00
 */
void TextMap_UploadRows(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint16_t *d3, uint16_t *d4, uint16_t *d5,
                        uint16_t *d6, uint16_t *d7, uint32_t a0)
{
    uint32_t cmd = (*d2 & 0xFFFFu) << 2;
    cmd = (cmd & 0xFFFF0000u) | ((cmd & 0xFFFFu) >> 2);
    cmd = (cmd << 16) | (cmd >> 16);
    *d2 = cmd | 0x40000080u;
    *d3 = 0x9500;
    *d4 = 0x96C8;
    *d5 = 0x977F;
    *d6 = (uint16_t)(g_PlaneRowBytes * 4);
    for (;;) {
        *d0 = 1;
        *d1 = 3;
        DMA_Reserve(d0, (uint16_t)*d1, &a0);
        if ((int32_t)*d0 < 0)                /* bmi: d0 = -1, nothing reserved */
            return;
        PTR(u16) entry;
        entry.v = a0;
        u16 *q = entry.get();
        q[0] = 0x9328;                       /* length $28 words */
        q[1] = 0x9400;
        q[2] = *d3;                          /* source address registers */
        q[3] = *d4;
        q[4] = *d5;
        q[5] = (uint16_t)(*d2 >> 16);        /* the VDP command */
        q[6] = (uint16_t)*d2;
        g_DMAQueuePtr = (uint16_t)dk::addr(q + 7);
        /* the source moves on 40 words: $95 += $28, carried into $96 and $97 (byte adds) */
        uint16_t lo = (uint16_t)((*d3 & 0xFF) + 0x28);
        *d3 = (uint16_t)((*d3 & 0xFF00) | (lo & 0xFF));
        if (lo > 0xFF) {
            uint16_t mid = (uint16_t)((*d4 & 0xFF) + 1);
            *d4 = (uint16_t)((*d4 & 0xFF00) | (mid & 0xFF));
            if (mid > 0xFF)
                *d5 = (uint16_t)((*d5 & 0xFF00) | ((*d5 + 1) & 0xFF));
        }
        /* the next row's VRAM address: the command's address bits rotated back, the stride added */
        uint32_t c = (*d2 << 16) | (*d2 >> 16);
        uint16_t w = (uint16_t)c;
        w = (uint16_t)((w << 2) | (w >> 14));
        w = (uint16_t)(w + *d6);
        w = (uint16_t)((w >> 2) | (w << 14));
        c = (c & 0xFFFF0000u) | w;
        *d2 = (c << 16) | (c >> 16);
        if ((*d7)-- == 0)                    /* dbra d7 */
            return;
    }
}

/* TextMap_Upload - queues DMAs of the 28 rows of the plane image to plane A (g_PlaneAVRAM).
 * In:  none
 * Out: d7 = $1B (moveq) going in, d2 low word = g_PlaneAVRAM; then as TextMap_UploadRows
 * Evidence: moveq #$1B,d7 / move.w (g_PlaneAVRAM).w,d2, falling into TextMap_UploadRows; every frame
 *           after TextMap_Run in the menu scenes
 */
void TextMap_Upload(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint16_t *d3, uint16_t *d4, uint16_t *d5, uint16_t *d6,
                    uint32_t *d7, uint32_t a0)
{
    *d7 = IMAGE_ROWS - 1;
    set_low_word(d2, g_PlaneAVRAM);
    uint16_t rows = (uint16_t)*d7;
    TextMap_UploadRows(d0, d1, d2, d3, d4, d5, d6, &rows, a0);
    set_low_word(d7, rows);
}

/* Menu_MoveCursor - moves the menu cursor g_MenuCursor with the D-pad's new presses: Up one back
 * (not below 0), Down one on (not past the last item).
 * In:  last = the last item
 * Out: d0 = the cursor (its low word; moveq #0 when clamped at 0), pad = pad 1's newly pressed
 *      byte; g_MenuCursor
 * Evidence: move.w (g_MenuCursor).w,d0 / move.b (g_Pad1+2).w,d2 / btst #0 / subq / bpl / moveq #0 /
 *           btst #1 / addq / cmp.w d1,d0 / ble / move.w d1,d0; callers $499C2 (d1 = 1), Menu_Update
 */
void Menu_MoveCursor(uint32_t *d0, uint16_t last, uint8_t *pad)
{
    set_low_word(d0, g_MenuCursor);
    *pad = g_Pad1[2];
    if (*pad & 0x01) {                       /* up */
        set_low_word(d0, (uint16_t)(*d0 - 1));
        if ((int16_t)*d0 < 0)
            *d0 = 0;
    } else if (*pad & 0x02) {                /* down */
        set_low_word(d0, (uint16_t)(*d0 + 1));
        if ((int16_t)*d0 > (int16_t)last)
            set_low_word(d0, last);
    }
    g_MenuCursor = (uint16_t)*d0;
}
