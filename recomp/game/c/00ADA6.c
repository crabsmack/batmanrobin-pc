#include "game.h"

/* VDP_FillLoop's loop, inline: a bridge call here would push a return address the asm's bra
   doesn't, into stack bytes verify compares (the top of RAM at Main_Start) */
static void fill_units(uint32_t fill, uint32_t units_minus1)
{
    for (uint32_t n = units_minus1 + 1; n != 0; n--)
        for (int i = 0; i < 4; i++)
            hw::port_write32(hw::VDP_DATA, fill);
}

/* VDP_ClearCRAM - sets all 64 CRAM colours to black.
 * In:  none
 * Out: CRAM = 0 (a0 = the data port, d0 = 0, d7 = -1: clobbers)
 * Evidence: lea $C00000,a0 / move.w #7,d7 / move.l #$C0000000,4(a0) / bra VDP_ClearLoop: 8 x 16
 *           bytes; callers Main_Start and Scene_InitCommon (g_FadeLevel < $E); TAS 26 frames
 */
void VDP_ClearCRAM(void)
{
    hw::port_write32(hw::VDP_CTRL, 0xC0000000u);  /* CRAM write at 0 */
    fill_units(0, 7);
}
