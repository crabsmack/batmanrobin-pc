#include "game.h"
#include "platform.h"

/* Boot_DrawLockoutText - writes one 0-terminated lockout-screen string to the plane at VRAM
 * $C000 (64 cells a row) at (column, row), a tile per character (tile = char - $20).
 * In:  column / row (bytes), text, the VDP data and control ports (a4 / a5)
 * Out: the VRAM words; nothing returned (a0 stops at the terminator; d2/d3 clobbered)
 * Evidence: VDP address = $40000003 + row << 23 + column << 17 (VRAM write at $C000 + row * $80 +
 *           column * 2); subi.b #$20 / andi.w #$FF / move.w d2,(a4); callers in Boot_CheckRegion;
 *           never run in the corpus (lockout only)
 */
void Boot_DrawLockoutText(uint8_t column, uint8_t row, const u8 *text, uint32_t data_port, uint32_t ctrl_port)
{
    hw::port_write32(ctrl_port, ((uint32_t)row << 23) + ((uint32_t)column << 17) + 0x40000003u);
    while (*text)
        hw::port_write16(data_port, (uint8_t)(*text++ - 0x20));
}
