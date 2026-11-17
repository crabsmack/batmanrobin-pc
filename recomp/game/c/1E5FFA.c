#include "game.h"

enum { SNDINIT_DRIVER_BYTE = 0xE4 };   /* Snd_LoadDriver's byte for Z80 RAM $003B */

/* Snd_Init - boot-time sound init: releases the Z80 reset, loads the driver (d1 = $E4), switches
 * every music channel off and resets the fade
 * In:  none
 * Out: the Z80 running the driver, silent; a5 = Snd_LoadDriver's (the end of the Z80 program)
 * Evidence: move.w #$100,$A11200 / move.w #$E4,d1 / bsr.w Snd_LoadDriver / bsr.w Snd_StopChannels /
 *           bra.w Snd_ResetFade; only caller the main init $9E02; 146 corpus runs
 */
uint32_t Snd_Init(void)
{
    hw::port_write16(hw::Z80_RESET, 0x100);
    uint32_t a5 = Snd_LoadDriver(SNDINIT_DRIVER_BYTE);
    Snd_StopChannels();
    dk::jump(at_Snd_ResetFade);
    return a5;
}
