#include "game.h"

/* VDP_FillLoop's loop, inline: a bridge call here would push a return address the asm's bra
   doesn't, into stack bytes verify compares (the top of RAM at Main_Start) */
static void fill_units(uint32_t fill, uint32_t units_minus1)
{
    for (uint32_t n = units_minus1 + 1; n != 0; n--)
        for (int i = 0; i < 4; i++)
            hw::port_write32(hw::VDP_DATA, fill);
}

/* VDP_ClearVSRAM - zeroes the 40 VSRAM words (every vertical scroll value).
 * In:  none
 * Out: VSRAM = 0 (a0 = the data port, d0 = 0, d7 = -1: clobbers)
 * Evidence: lea $C00000,a0 / move.w #4,d7 / move.l #$40000010,4(a0) / bra VDP_ClearLoop: 5 x 16
 *           bytes; only caller Scene_InitCommon; TAS 25 frames (scene starts)
 */
void VDP_ClearVSRAM(void)
{
    hw::port_write32(hw::VDP_CTRL, 0x40000010u);  /* VSRAM write at 0 */
    fill_units(0, 4);
}
