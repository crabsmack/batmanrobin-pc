#include "game.h"

/* The VDP fills ($00AD84-$00ADA5): VDP_ClearVRAM, and the loop every clear and fill ends in. The
 * callers set the VDP's write address first (control port, 4(a0)). */

/* the loop: 4 longs per 16-byte unit, units_minus1 + 1 units (inline in each entry: a bridge
   call would push a return address the asm doesn't, into stack bytes verify compares) */
static void fill_units(uint32_t fill, uint32_t units_minus1, uint32_t port)
{
    for (uint32_t n = units_minus1 + 1; n != 0; n--)
        for (int i = 0; i < 4; i++)
            hw::port_write32(port, fill);
}

/* VDP_ClearVRAM - zeroes all 64 KB of VRAM.
 * In:  none
 * Out: VRAM 0-$FFFF = 0 (a0 = the data port, d0 = 0, d7 = -1: clobbers)
 * Evidence: lea $C00000,a0 / move.w #$FFF,d7 / move.l #$40000000,4(a0), falling into
 *           VDP_ClearLoop: $1000 x 16 bytes; only caller Main_Start; TAS frame 6, once
 */
void VDP_ClearVRAM(void)
{
    hw::port_write32(hw::VDP_CTRL, 0x40000000u);  /* VRAM write at 0 */
    fill_units(0, 0x0FFF, hw::VDP_DATA);   /* falling into VDP_ClearLoop */
}

/* VDP_ClearLoop - the shared tail of the clears: VDP_FillLoop with a fill of 0.
 * In:  units (d7) = 16-byte units - 1, port (a0) = the VDP data port, its address already set
 * Out: as VDP_FillLoop (d0 = 0)
 * Evidence: moveq #0,d0, falling into VDP_FillLoop; the bra target of VDP_ClearCRAM,
 *           VDP_ClearVSRAM, VDP_ClearSAT, VDP_ClearHScroll; VDP_ClearVRAM falls into it
 */
void VDP_ClearLoop(uint16_t units, uint32_t port)
{
    fill_units(0, units, port);
}

/* VDP_FillLoop - writes the long fill to the VDP data port 4 times per unit: fills units + 1
 * 16-byte units from the address already set.
 * In:  fill (d0), units (d7) = 16-byte units - 1 (a word: dbra), port (a0) = the data port
 * Out: d7 = -1 (clobbered)
 * Evidence: 4x move.l d0,(a0) / dbra d7; the bra target of VDP_WhiteCRAM ($0EEE0EEE) and
 *           VDP_ClearPlanes ($05FC05FC)
 */
void VDP_FillLoop(uint32_t fill, uint16_t units, uint32_t port)
{
    fill_units(fill, units, port);
}
