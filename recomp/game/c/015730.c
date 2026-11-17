#include "game.h"

/* the pad record g_DebugPadPtr names: +0 the buttons held, +2 the buttons newly pressed */
enum {
    PAD_UP = 0x01,
    PAD_DOWN = 0x02,
    PAD_LEFT = 0x04,
    PAD_RIGHT = 0x08,
    PAD_DIRS = 0x0F,
    PAD_C = 0x20,
    PAD_A = 0x40,
};
static constexpr int PAD_PRESSED = 2;

static constexpr uint32_t CURSOR_ACCEL = 0x2000;      /* 16.16: the speed grows 1/8 px a frame */
static constexpr int32_t CURSOR_TOP_SPEED = 0x40000;  /* ... while under 4 px a frame */
static constexpr uint16_t VIEW_CENTRE_X = 0xA0;       /* the screen centre from the camera */
static constexpr uint16_t VIEW_CENTRE_Y = 0x70;
static constexpr uint16_t SOLID_ALL = 0xFFFF;         /* Coll_MoveScene's d6/d7 masks: every terrain */
static constexpr uint16_t FRAME_CURSOR = 0xAA;        /* the cursor's cross */
static constexpr uint16_t FRAME_BLOCK = 0xB4;         /* the 16-px block outline under it */
static constexpr uint16_t CURSOR_ATTR = 0x8000;       /* Spr_AddSorted d6: high priority */
static constexpr uint16_t CURSOR_DEPTH = 0x7FFF;      /* d7: in front of everything */
static constexpr uint32_t BLOCK_MASK = 0xFFFFFFF0u;   /* andi.w #$FFF0: the low word only */
static constexpr uint16_t READOUT_BYTES = 2;

/* Debug_DrawHexBytes positions: column << 8 | row, 8-px cells */
enum {
    AT_LOCK_X = 0x2317,
    AT_OFFSET_X = 0x1E18,
    AT_OFFSET_Y = 0x2318,
    AT_CURSOR_X = 0x1E1A,
    AT_CURSOR_Y = 0x231A,
    AT_BLOCK_X = 0x1E19,
    AT_BLOCK_Y = 0x2319,
    AT_MAP_X = 0x1E15,
    AT_MAP_Y = 0x2315,
};

static uint16_t hi(uint32_t l) { return (uint16_t)(l >> 16); }
static uint32_t with_lo(uint32_t r, uint16_t w) { return (r & 0xFFFF0000u) | w; }
static uint32_t with_hi(uint32_t l, uint16_t w) { return ((uint32_t)w << 16) | (l & 0xFFFFu); }
static uint32_t swap_halves(uint32_t v) { return (v << 16) | (v >> 16); }

/* Debug_FreeCamStep - the debug free camera's frame (run instead of the objects while g_FreeCam):
 * C toggles a camera lock at the screen centre, the D-pad accelerates a cursor (through the
 * terrain's collision while A is held), the cursor and its 16-px block are drawn with nine hex
 * readouts, and the camera follows the cursor (or the lock) rightward
 * In:  g_DebugPadPtr = the pad record that opened the debug pause menu; a3 = the motion block
 *      Coll_MoveScene reads while A is held; the rest pass through to the calls
 * Out: g_DebugCursorX/Y moved, g_DebugCursorSpeed, g_CameraLockX, the readout RAM and
 *      g_DebugCursorFrame; a0 = the pad record, then as the calls hand the registers back; d0 =
 *      the cursor's (or the lock's) x from the screen centre, then Debug_FreeCamScroll (right of
 *      it) or Debug_FreeCamSyncLayer
 * Evidence: btst #5,2(a0) / tst.l g_CameraLockX / clr.l, else (g_CameraX + $A0) << 16; move.b (a0),d0
 *           / andi.w #$F / clr.l g_DebugCursorSpeed, else cmpi.l #$40000 / bge / addi.l #$2000 and
 *           the four btst / sub.l / add.l of d3; btst #6,(a0): add.l to the cursor, or the 16.16
 *           move through Coll_MoveScene (d6/d7 $FFFF, the integer deltas after the swaps, d4/d5 the
 *           new integer x/y); frames $AA and $B4 via Spr_ChangeFrame + Spr_AddSorted; nine
 *           Debug_DrawHexBytes; bpl Debug_FreeCamScroll / bra Debug_FreeCamSyncLayer. Static only:
 *           g_FreeCam's one writer is Debug_PauseMenu, which Pause_Update enters only when
 *           g_DebugPause is set, and its one writer stores 0 ($009E02). d0 an output (measured as
 *           a clobber): Debug_FreeCamScroll compares it
 */
void Debug_FreeCamStep(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5,
                       uint32_t *d6, uint32_t *d7, uint32_t *a0, uint32_t *a1, uint32_t *a2, uint32_t a3, uint32_t a4,
                       uint32_t a5, uint32_t *a6)
{
    *a0 = (uint32_t)(int32_t)(int16_t)g_DebugPadPtr;   /* movea.w */
    PTR(const u8) record;
    record.v = *a0;
    const u8 *pad = record;
    if (pad[PAD_PRESSED] & PAD_C) {
        if (g_CameraLockX) {
            g_CameraLockX = 0;
        } else {
            *d0 = (uint32_t)(uint16_t)(hi(g_CameraX) + VIEW_CENTRE_X) << 16;
            g_CameraLockX = *d0;
        }
    }
    *d0 = with_lo(*d0, pad[0] & PAD_DIRS);   /* move.b / andi.w: the word's high byte cleared too */
    uint16_t dirs = (uint16_t)*d0;
    if (!dirs) {
        g_DebugCursorSpeed = 0;
    } else {
        *d1 = 0;
        *d2 = 0;
        *d3 = g_DebugCursorSpeed;
        if ((int32_t)*d3 < CURSOR_TOP_SPEED)
            g_DebugCursorSpeed += CURSOR_ACCEL;
        if (dirs & PAD_LEFT)
            *d1 -= *d3;
        if (dirs & PAD_RIGHT)
            *d1 += *d3;
        if (dirs & PAD_UP)
            *d2 -= *d3;
        if (dirs & PAD_DOWN)
            *d2 += *d3;
        if (!(pad[0] & PAD_A)) {
            g_DebugCursorX += *d1;
            g_DebugCursorY += *d2;
        } else {
            /* the fractions move freely; the integer step goes through the terrain */
            *d4 = g_DebugCursorX;
            *d5 = g_DebugCursorY;
            *d1 += *d4;
            *d2 += *d5;
            g_DebugCursorX = (g_DebugCursorX & 0xFFFF0000u) | (uint16_t)*d1;
            g_DebugCursorY = (g_DebugCursorY & 0xFFFF0000u) | (uint16_t)*d2;
            *d1 = swap_halves(*d1);
            *d2 = swap_halves(*d2);
            *d4 = swap_halves(*d4);
            *d5 = swap_halves(*d5);
            *d1 = with_lo(*d1, (uint16_t)(*d1 - *d4));
            *d2 = with_lo(*d2, (uint16_t)(*d2 - *d5));
            *d6 = with_lo(*d6, SOLID_ALL);
            *d7 = with_lo(*d7, SOLID_ALL);
            uint32_t r4 = a4, r5 = a5;
            PTR(const Vm2Motion) mot;
            mot.v = a3;
            Coll_MoveScene(d0, d1, d2, d3, d4, d5, d6, d7, a0, a1, a2, mot, &r4, &r5, a6);
            g_DebugCursorX = with_hi(g_DebugCursorX, (uint16_t)*d4);
            g_DebugCursorY = with_hi(g_DebugCursorY, (uint16_t)*d5);
        }
    }

    auto draw_sprite = [&](uint16_t frame) {
        *d1 = with_lo(*d1, frame);
        *d2 = with_lo(*d2, g_DebugCursorFrame);
        Spr_ChangeFrame(d0, d1, d2, d3, a0, a1);
        g_DebugCursorFrame = (uint16_t)*d1;
        uint16_t w0 = (uint16_t)*d0;
        dk::Flags ccr{};
        Spr_AddSorted(&w0, d1, d2, (uint16_t)*d4, (uint16_t)*d5, (uint16_t)*d6, (uint16_t)*d7, *a0, *a1, *a2, &ccr);
        *d0 = with_lo(*d0, w0);
    };
    *d4 = with_lo(*d4, hi(g_DebugCursorX));
    *d5 = with_lo(*d5, hi(g_DebugCursorY));
    *d6 = with_lo(*d6, CURSOR_ATTR);
    *d7 = with_lo(*d7, CURSOR_DEPTH);
    draw_sprite(FRAME_CURSOR);
    *d4 &= BLOCK_MASK;
    *d5 &= BLOCK_MASK;
    g_DebugCursorBlockX = (uint16_t)*d4;
    g_DebugCursorBlockY = (uint16_t)*d5;
    draw_sprite(FRAME_BLOCK);

    auto readout = [&](uint32_t bytes, uint16_t at) {
        *a6 = bytes;
        *d0 = with_lo(*d0, at);
        *d1 = with_lo(*d1, READOUT_BYTES);
        uint16_t w4 = (uint16_t)*d4, w5 = (uint16_t)*d5, w6 = (uint16_t)*d6, w7 = (uint16_t)*d7;
        Debug_DrawHexBytes(d0, d1, d2, d3, &w4, &w5, &w6, &w7, a0, a1, a2, a6);
        *d4 = with_lo(*d4, w4);
        *d5 = with_lo(*d5, w5);
        *d6 = with_lo(*d6, w6);
        *d7 = with_lo(*d7, w7);
    };
    readout(ADDR68(&g_CameraLockX), AT_LOCK_X);
    *d0 = with_lo(*d0, (uint16_t)(hi(g_DebugCursorX) - hi(g_CameraX) - VIEW_CENTRE_X));
    g_DebugReadout = (uint16_t)*d0;
    readout(ADDR68(&g_DebugReadout), AT_OFFSET_X);
    *d0 = with_lo(*d0, (uint16_t)(hi(g_DebugCursorY) - hi(g_CameraY) - VIEW_CENTRE_Y));
    g_DebugReadout = (uint16_t)*d0;
    readout(ADDR68(&g_DebugReadout), AT_OFFSET_Y);
    readout(ADDR68(&g_DebugCursorX), AT_CURSOR_X);
    readout(ADDR68(&g_DebugCursorY), AT_CURSOR_Y);
    readout(ADDR68(&g_DebugCursorBlockX), AT_BLOCK_X);
    readout(ADDR68(&g_DebugCursorBlockY), AT_BLOCK_Y);
    *d0 = g_MapScrollX + g_DebugCursorX;
    g_DebugCursorMapX = *d0;
    readout(ADDR68(&g_DebugCursorMapX), AT_MAP_X);
    *d0 = g_MapScrollY + g_DebugCursorY;
    g_DebugCursorMapY = *d0;
    readout(ADDR68(&g_DebugCursorMapY), AT_MAP_Y);

    /* the camera follows the lock x, or the cursor's */
    *d0 = with_lo(*d0, hi(g_CameraLockX) ? hi(g_CameraLockX) : hi(g_DebugCursorX));
    uint16_t off = (uint16_t)((uint16_t)*d0 - VIEW_CENTRE_X - hi(g_CameraX));
    *d0 = with_lo(*d0, off);
    if ((int16_t)off >= 0)
        dk::jump(at_Debug_FreeCamScroll);
    else
        dk::jump(at_Debug_FreeCamSyncLayer);
}
