#include "game.h"

/* the menu font's map (a word per ASCII code from $20) and palette line (ROM) */
static constexpr uint32_t Tbl_0586F2 = 0x0586F2u;
static constexpr uint32_t Tbl_015634 = 0x015634u;

/* TextMap_LoadFont - loads the menu font: DMAs its 132 tiles ($58AA2) to VRAM at the tile the base
 * word names, with the display off during it; sets the font map and tile base TextMap_Run uses;
 * then palette line 3 from Tbl_015634.
 * In:  d0 = the tile base word (priority / palette bits + tile index, e.g. $E1C9)
 * Out: g_TextMapTileBase = it, g_TextMapFont = Tbl_0586F2; g_VDPCmdBuf = the VRAM write command;
 *      table = Tbl_015634, then Pal_LoadLine3
 * Evidence: VDP regs $8134 (display off), $93/$94 = $0840 words, $95-$97 = $02C551 (the word address
 *           of $58AA2), the command from (d0 & $7FF) * 32 sent from g_VDPCmdBuf as two words, $8164;
 *           lea Tbl_015634,a0 / jmp Pal_LoadLine3; callers the menu / text scene inits ($4916E,
 *           $496A2, $49F62, $4A5AA, $4A626); TAS f164, f404
 */
void TextMap_LoadFont(uint16_t base, uint32_t *table)
{
    g_TextMapTileBase = base;
    g_TextMapFont = Tbl_0586F2;
    hw::port_write16(hw::VDP_CTRL, 0x8134);           /* display off, DMA on */
    hw::port_write32(hw::VDP_CTRL, 0x93409408u);      /* length $0840 words */
    hw::port_write32(hw::VDP_CTRL, 0x955196C5u);      /* source $58AA2 >> 1 */
    hw::port_write16(hw::VDP_CTRL, 0x9702);
    uint32_t cmd = ((uint32_t)base & 0x7FFu) << 7;    /* tile x 32 bytes, shifted 2 for the split */
    cmd = (cmd & 0xFFFF0000u) | ((cmd & 0xFFFFu) >> 2);
    cmd = (cmd << 16) | (cmd >> 16);
    g_VDPCmdBuf = cmd | 0x40000080u;                  /* VRAM write + DMA */
    hw::port_write16(hw::VDP_CTRL, (uint16_t)(g_VDPCmdBuf >> 16));
    hw::port_write16(hw::VDP_CTRL, (uint16_t)g_VDPCmdBuf);   /* the second word starts the DMA */
    hw::port_write16(hw::VDP_CTRL, 0x8164);           /* display on */
    *table = Tbl_015634;
    dk::jump(at_Pal_LoadLine3);
}
