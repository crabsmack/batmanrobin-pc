#include "game.h"

enum { Z80_FADE_BYTE = 0x0E67 };   /* the driver's fade / attenuation byte (Z80 address) */

/* Snd_WriteFadeByte - writes a byte into the Z80 driver's fade byte with the Z80 bus held,
 * repeating the store until it reads back
 * In:  value (d0.b)
 * Out: Z80 RAM $0E67 = value; the Z80 bus released; a1 saved on the stack around it
 * Evidence: movem.l a1,-(a7) / move.w #$100,$A11100 / btst #0,$A11100 / bne (wait for the grant) /
 *           lea $A00E67,a1 / move.b d0,(a1) / cmp.b (a1),d0 / bne / move.w #0,$A11100 /
 *           movem.l (a7)+,a1 / rts; callers Snd_ResetFadeBody (and the never-run $1E79D4 and stub
 *           $1E5022); 146 corpus runs. a1 only saved (dead stack, not an input)
 */
void Snd_WriteFadeByte(uint8_t value)
{
    hw::port_write16(hw::Z80_BUSREQ, 0x100);
    while (hw::port_read8(hw::Z80_BUSREQ) & 1) {
    }
    uint32_t fade = hw::Z80_RAM + Z80_FADE_BYTE;
    do
        hw::port_write8(fade, value);
    while (hw::port_read8(fade) != value);
    hw::port_write16(hw::Z80_BUSREQ, 0);
}
