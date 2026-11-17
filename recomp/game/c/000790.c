#include "game.h"
#include "platform.h"

/* Boot_InitIOPorts - after the region check passes: waits for any VDP DMA to finish, clears the
 * three ports' serial control registers, makes TH an output on all three (control $40) and enters
 * the game's main init.
 * In:  none (from Boot_CheckRegion); the other registers pass through to Main_Start
 * Out: never returns: Main_Start, jumped to
 * Evidence: move.w ($C00004).l,d0 / btst #1,d0 / bne (DMA busy); move.b #0 to $A10013/$A10019/
 *           $A1001F, #$40 to $A10009/$A1000B/$A1000D; jmp Main_Start; runs once on TAS frame 3
 */
void Boot_InitIOPorts(uint32_t d1, uint32_t d3, uint32_t d4, uint32_t d5, uint32_t d6, uint32_t a1, uint32_t a2,
                      uint32_t a3, uint32_t a4, uint32_t a6)
{
    while (hw::port_read16(hw::VDP_CTRL) & 0x02)
        ;   /* the status register's DMA-busy bit */
    hw::port_write8(hw::IO_SCTRL1, 0x00);
    hw::port_write8(hw::IO_SCTRL2, 0x00);
    hw::port_write8(hw::IO_SCTRL3, 0x00);
    hw::port_write8(hw::IO_CTRL1, 0x40);
    hw::port_write8(hw::IO_CTRL2, 0x40);
    hw::port_write8(hw::IO_CTRL3, 0x40);
    dk::jump(at_Main_Start);
}
