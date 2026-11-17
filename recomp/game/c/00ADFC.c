#include "game.h"

/* VDP_FillLoop's loop, inline: a bridge call here would push a return address the asm's bra
   doesn't, into stack bytes verify compares (the top of RAM at Main_Start) */
static void fill_units(uint32_t fill, uint32_t units_minus1)
{
    for (uint32_t n = units_minus1 + 1; n != 0; n--)
        for (int i = 0; i < 4; i++)
            hw::port_write32(hw::VDP_DATA, fill);
}

/* VDP_ClearHScroll - zeroes VRAM $BC00-$C3FF: the H-scroll table (and the tile $5FC that
 * VDP_ClearPlanes then fills the planes with).
 * In:  none
 * Out: VRAM $BC00-$C3FF = 0 (a0 = the data port, d0 = 0, d7 = -1: clobbers)
 * Evidence: lea $C00000,a0 / move.w #$7F,d7 / move.l #$7C000002,4(a0) / bra VDP_ClearLoop: 2 KB;
 *           reg 13 = $2F (H-scroll at $BC00) in Tbl_0155BC; only caller Scene_InitCommon
 */
void VDP_ClearHScroll(void)
{
    hw::port_write32(hw::VDP_CTRL, 0x7C000002u);  /* VRAM write at $BC00 */
    fill_units(0, 0x7F);
}
