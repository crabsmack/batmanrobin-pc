#include "game.h"

/* The boss meter's case callback and its ring drawing ($011BBA-$011C5D). */

static constexpr uint32_t Tbl_057892 = 0x057892u;   /* the ring's tile numbers: 16 frames of 2 rows x 4 */
static constexpr uint32_t Tbl_057992 = 0x057992u;   /* the ring's tiles */

/* the meter script block's flags byte (+$25, the low byte of var 1) */
enum { BOSSMETER_BLANK_AT_ZERO = 1 << 1 };

/* BossMeter_Exists - native case callback: case 1 when the boss energy meter object exists, else
 * case 0.
 * In:  none
 * Out: d0 = 1 (moveq) with a meter; without, d0's low word = 0 (the upper word kept)
 * Evidence: move.w (g_BossMeterObj).w,d0 / beq / moveq #1,d0; two op $2D40 pointers in the
 *           coroutine at $030FB8; TAS frames 75519-76134
 */
void BossMeter_Exists(uint32_t *d0)
{
    uint16_t meter = g_BossMeterObj;
    *d0 = (*d0 & 0xFFFF0000u) | meter;
    if (meter != 0)
        *d0 = 1;
}

/* BossMeter_AnimRing - advances the meter ring's 16-frame animation (var 0 in steps of 8 mod $80)
 * and copies that frame's 8 tiles, 2 rows of 4, into g_BossMeterTiles (a row's tiles every $40
 * bytes, the second row $20 after the first).
 * In:  vm = the meter's script block (a4)
 * Out: var 0 stepped; g_BossMeterTiles rewritten; d0 low word = the last tile's offset, d6 = d7 =
 *      $FFFF (moveq, dbra), a1 = Tbl_057992; a2/a3 preserved
 * Evidence: move.w $22(a4),d0 / addq.w #8 / andi.w #$78 / move.w d0,$22(a4) / movem.l a2-a3,-(a7) /
 *           lea Tbl_057892,a0 / lea Tbl_057992,a1 / lea g_BossMeterTiles,a2 / adda.w d0,a0 / moveq
 *           #1,d6 / moveq #3,d7 / 8 x move.l / lea $20(a2) / lea -$E0(a2) / lea $78(a0); op $2CB2
 *           call in Scr2_011B3E
 */
void BossMeter_AnimRing(uint16_t *d0, uint32_t *d6, uint32_t *d7, uint32_t *a1, Vm2Block *vm)
{
    uint16_t frame = (uint16_t)((vm->var[0] + 8) & 0x78);
    vm->var[0] = frame;
    *d0 = frame;
    /* movem.l a2-a3,-(a7): neither is an input (both preserved) */
    dk::push_long(0);
    dk::push_long(0);
    uint32_t a0 = Tbl_057892 + frame;
    *a1 = Tbl_057992;
    PTR(u8) dst;
    dst.v = dk::addr(g_BossMeterTiles);
    for (int row = 0; row < 2; row++) {
        for (int col = 0; col < 4; col++) {
            PTR(const u16) index;
            index.v = a0;
            a0 += 2;
            *d0 = (uint16_t)(*index.get() << 5);
            PTR(const u8) src;
            src.v = *a1 + (uint32_t)(int32_t)(int16_t)*d0;
            dk::mem_copy(dst.get(), src.get(), 32);
            dst.v = dst.v + 32 + 0x20;
        }
        dst.v = dst.v - 0xE0;
        a0 += 0x78;
    }
    dk::pop_long();
    dk::pop_long();
    *d6 = 0xFFFFu;
    *d7 = 0xFFFFu;
}

/* BossMeter_DrawPercent - draws the boss's energy left as a two-digit percentage over the ring
 * tiles: energy x 99 / full (at least 1 while it lives); at 0 energy, "0" and the digit d0's upper
 * word holds (the original's: only the low word is loaded), unless var 1's bit 1 asks for nothing.
 * In:  vm = the meter's script block (a4); d0's upper word (the zero-energy case's ones digit)
 * Out: the tens at g_BossMeterTiles+$50, the ones at +$90 (BossMeter_DrawDigit); d0 = the two
 *      digits' glyph offsets swapped (as the second swap leaves them), d1/a1 as the last digit's
 * Evidence: move.w (g_BossEnergy).w,d0 / bne / btst #1,$25(a4) / beq / rts; mulu.w #$63 / divu.w
 *           (g_BossEnergyMax).w / ext.l / bne / moveq #1 / divu.w #$A; move.l a2,-(a7) / lea
 *           g_BossMeterTiles+$50,a2 / bsr BossMeter_DrawDigit / lea +$90 / swap d0 / bsr / movea.l
 *           (a7)+,a2; op $2CB2 call in Scr2_011B3E; $16C0 of $2200 at TAS f4800 shows 66
 */
void BossMeter_DrawPercent(uint32_t *d0, uint32_t *d1, uint32_t *a1, const Vm2Block *vm)
{
    uint16_t energy = g_BossEnergy;
    *d0 = (*d0 & 0xFFFF0000u) | energy;
    if (energy == 0) {
        if ((uint16_t)vm->var[1] & BOSSMETER_BLANK_AT_ZERO)
            return;
    } else {
        *d0 = (uint32_t)energy * 99;
        uint16_t full = g_BossEnergyMax;
        if (full == 0)   /* the 68000 takes its divide-by-zero trap */
            dk::fatal("BossMeter_DrawPercent: divu by g_BossEnergyMax = 0");
        uint32_t quotient = *d0 / full;
        if (quotient <= 0xFFFFu)   /* divu.w: an overflow leaves d0 as it was */
            *d0 = ((*d0 % full) << 16) | quotient;
        *d0 = (uint32_t)(int32_t)(int16_t)(uint16_t)*d0;   /* ext.l */
        if (*d0 == 0)
            *d0 = 1;
        quotient = *d0 / 10;
        if (quotient <= 0xFFFFu)
            *d0 = ((*d0 % 10) << 16) | quotient;
    }
    dk::push_long(0);   /* move.l a2,-(a7): a2 is not an input */
    BossMeter_DrawDigit(d0, d1, a1, dk::addr(g_BossMeterTiles + 0x50));
    *d0 = (*d0 << 16) | (*d0 >> 16);   /* swap: the ones */
    BossMeter_DrawDigit(d0, d1, a1, dk::addr(g_BossMeterTiles + 0x90));
    dk::pop_long();
}
