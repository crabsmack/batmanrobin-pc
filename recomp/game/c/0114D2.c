#include "game.h"

static constexpr uint8_t HUD_CHARGE_FULL = 0x2B;       /* the charge byte of a charged shot */
static constexpr uint16_t HUD_RAPID_BLINK = 0x96;      /* the rapid charge's last 150 frames blink */
static constexpr uint16_t HUD_BAR_TYPE_STRIDE = 0x480; /* bytes per weapon type in the bar map */

/* HUD_DrawWeaponBar - draws the weapon charge bar under the score into the panel buffer
 * (g_HUDTileBuf+$20, 6 columns x 2 tiles): the bar's frame is the charge byte (+$8 high byte,
 * at most $2B; full: +2 while the rapid charge +$26 runs, +1 on every other draw), its colour set
 * by the weapon type +$28 (0, 1, 2+), and during the rapid charge's last 150 frames every other
 * draw keeps the map's first frame (a blink).
 * In:  a0 = the bar's tile-index map (Tbl_051792 player 1, Tbl_054812 player 2), a1 = its tiles;
 *      a6 = the player record
 * Out: 12 tiles at g_HUDTileBuf+$20 (column stride $60); +$8 clamped, +$25 bits 4/5 toggled;
 *      a0 = the map + frame + 12, a2/a3/a4 past the last copy, d6/d7 low words $FFFF (dbra)
 * Evidence: move.b $8(a6),d0 / ext.w / cmpi.w #$2B / blt / move.b d0,$8(a6) / tst.w $26 / addq #2 /
 *           bchg #4 / beq / addq #1; move.w $26(a6),d1 / cmpi.w #$96 / bge / bchg #5 / beq; cmpi.w
 *           #1,$28 / bcs / adda.w #$480 / beq (the cmpi's Z) / adda.w #$480; the frame offset
 *           (n & 7) x $90 + (n & ~7) x 1.5; the copy loop lea $48(a3) / lea $20(a2) / addq #2,a0;
 *           called by HUD_StepP1Panel / HUD_StepP2Panel
 */
void HUD_DrawWeaponBar(uint32_t *d6, uint32_t *d7, uint32_t *a0, uint32_t a1, uint32_t *a2, uint32_t *a3,
                       uint32_t *a4, uint32_t a6)
{
    PTR(PlayerRec) p;
    p.v = a6;
    PlayerRec *rec = p;

    int16_t frame = (int8_t)(uint8_t)(rec->weapon_charge >> 8);
    if (frame >= HUD_CHARGE_FULL) {
        frame = HUD_CHARGE_FULL;
        rec->weapon_charge = (uint16_t)((HUD_CHARGE_FULL << 8) | (rec->weapon_charge & 0xFF));
        if (rec->rapid_charge != 0)
            frame += 2;
        bool flashed = rec->flags & PLAYER_BAR_FLASH;
        rec->flags = (uint8_t)(rec->flags ^ PLAYER_BAR_FLASH);
        if (flashed)
            frame += 1;
    }

    bool blink_off = false;
    uint16_t rapid = rec->rapid_charge;
    if (rapid != 0 && (int16_t)rapid < (int16_t)HUD_RAPID_BLINK) {
        bool blinked = rec->flags & PLAYER_BAR_BLINK;
        rec->flags = (uint8_t)(rec->flags ^ PLAYER_BAR_BLINK);
        blink_off = !blinked;
    }
    if (!blink_off) {
        uint16_t type = rec->weapon_type;
        if (type >= 1) {
            *a0 += (uint32_t)(int32_t)(int16_t)HUD_BAR_TYPE_STRIDE;
            if (type != 1)
                *a0 += (uint32_t)(int32_t)(int16_t)HUD_BAR_TYPE_STRIDE;
        }
        uint16_t n = (uint16_t)frame;
        uint16_t low = n & 7, high = n ^ low;
        uint16_t offset = (uint16_t)(low * 0x90 + high + (high >> 1));
        *a0 += (uint32_t)(int32_t)(int16_t)offset;
    }

    *a2 = dk::addr(g_HUDTileBuf + 0x20);
    *d6 = (*d6 & 0xFFFF0000u) | 5;
    for (int col = 0; col < 6; col++) {
        *d7 = (*d7 & 0xFFFF0000u) | 1;
        *a3 = *a0;
        for (int row = 0; row < 2; row++) {
            PTR(const u16) index;
            index.v = *a3;
            uint16_t tile = (uint16_t)(*index.get() << 5);
            *a4 = a1 + (uint32_t)(int32_t)(int16_t)tile;
            PTR(const u8) src;
            src.v = *a4;
            PTR(u8) dst;
            dst.v = *a2;
            dk::mem_copy(dst.get(), src.get(), 32);
            *a4 += 32;
            *a2 += 32;
            *a3 += 0x48;
        }
        *a2 += 0x20;
        *a0 += 2;
    }
    *d6 |= 0xFFFFu;
    *d7 |= 0xFFFFu;
}
