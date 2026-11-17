#include "game.h"

/* VDP_FillLoop's loop, inline: a bridge call here would push a return address the asm's bra
   doesn't, into stack bytes verify compares (the top of RAM at Main_Start) */
static void fill_units(uint32_t fill, uint32_t units_minus1)
{
    for (uint32_t n = units_minus1 + 1; n != 0; n--)
        for (int i = 0; i < 4; i++)
            hw::port_write32(hw::VDP_DATA, fill);
}

enum { VDP_BLANK_TILE_PAIR = 0x05FC05FC };   /* two name words of tile $5FC (VRAM $BF80, zeroed) */

/* VDP_ClearPlanes - fills planes A and B (VRAM $C000-$FFFF) with tile $5FC, a blank tile.
 * In:  none
 * Out: VRAM $C000-$FFFF = $05FC; d0 = $05FC05FC (a measured output); a0 =
 *      the data port, d7 = -1: clobbers
 * Evidence: lea $C00000,a0 / move.w #$3FF,d7 / move.l #$40000003,4(a0) / move.l #$5FC05FC,d0 /
 *           bra.w VDP_FillLoop: 16 KB; regs 2 = $30, 4 = $07 in Tbl_0155BC; only caller
 *           Scene_InitCommon; TAS 25 frames
 */
uint32_t VDP_ClearPlanes(void)
{
    hw::port_write32(hw::VDP_CTRL, 0x40000003u);  /* VRAM write at $C000 */
    fill_units(VDP_BLANK_TILE_PAIR, 0x3FF);
    return VDP_BLANK_TILE_PAIR;
}
