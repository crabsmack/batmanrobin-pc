#include "game.h"

/* VDP_FillLoop's loop, inline: a bridge call here would push a return address the asm's bra
   doesn't, into stack bytes verify compares (the top of RAM at Main_Start) */
static void fill_units(uint32_t fill, uint32_t units_minus1)
{
    for (uint32_t n = units_minus1 + 1; n != 0; n--)
        for (int i = 0; i < 4; i++)
            hw::port_write32(hw::VDP_DATA, fill);
}

/* VDP_ClearSAT - zeroes the sprite attribute table (VRAM $0000-$027F, 80 sprites).
 * In:  none
 * Out: VRAM 0-$27F = 0 (a0 = the data port, d0 = 0, d7 = -1: clobbers)
 * Evidence: lea $C00000,a0 / move.w #$27,d7 / move.l #$40000000,4(a0) / bra VDP_ClearLoop: $280
 *           bytes; reg 5 = 0 in Tbl_0155BC puts the SAT at VRAM 0; only caller Scene_InitCommon
 */
void VDP_ClearSAT(void)
{
    hw::port_write32(hw::VDP_CTRL, 0x40000000u);  /* VRAM write at 0 */
    fill_units(0, 0x27);
#if DK_MODE_NATIVE
    spr_host_table_cleared();   /* phase 6 (R10): the host's sprites past the 80 with it */
    ws_draw_cleared();          /* R11: and the margin pieces */
#endif
}
