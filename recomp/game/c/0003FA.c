#include "game.h"
#include "platform.h"

/* ROM data the boot region check reads (labels in batmanrobin.asm / src/header.asm) */
static constexpr uint32_t Hdr_Regions = 0x0001F0u;            /* the header's region string ("JU", spaces) */
static constexpr uint32_t Tbl_RegionByVersion = 0x000536u;    /* region letter per version bits 7-6: J, 0, U, E */
static constexpr uint32_t Tbl_RegionLockout = 0x00053Au;      /* (word letter, long string offset from
                                                                 Tbl_RegionByVersion) entries, 0-terminated */
static constexpr uint32_t Str_LockoutDeveloped = 0x00054Eu;   /* each string: a column byte, then the text */
static constexpr uint32_t Str_LockoutAmp = 0x00056Bu;
static constexpr uint32_t Str_LockoutSystems = 0x00056Eu;
static constexpr uint32_t Art_LockoutFont = 0x0005B8u;        /* 1bpp 8x8, 59 glyphs from ' ' */

static const u8 *rom_bytes(uint32_t a)
{
    PTR(const u8) p;
    p.v = a;
    return p;
}

/* the lockout screen hangs: the 68000 spins with interrupts masked (the reset left SR = $2700) */
static void hang(void)
{
    for (;;)
        IRQ_WAIT();
}

/* one lockout line: the string's first byte is its column; the row is d1's low byte */
static void draw_line(uint32_t str, uint8_t row)
{
    const u8 *s = rom_bytes(str);
    Boot_DrawLockoutText(s[0], row, s + 1, hw::VDP_DATA, hw::VDP_CTRL);
}

/* Boot_CheckRegion - compares the console's region (the version register's bits 7-6, mapped by
 * Tbl_RegionByVersion) with the header's region string: a match continues the boot
 * (Boot_InitIOPorts); otherwise the lockout screen ("DEVELOPED FOR USE ONLY WITH <the header's
 * consoles> SYSTEMS.") is drawn and the console hangs. A version that maps to 0 hangs at once.
 * In:  none (from Boot_Reset); the other registers pass through
 * Out: never returns: Boot_InitIOPorts, jumped to, or the hang
 * Evidence: tst.w ($C00004) / move.b ($A10001),d0 / lsr.b #6 / move.b (a0,d0.w) / cmp.b (a0),d0 /
 *           beq.w Boot_InitIOPorts over 16 header bytes; the lockout path sets VDP regs $8164 $8230
 *           $8C81 $8F02 $9001, CRAM colour 1 = $0EEE, expands Art_LockoutFont to VRAM 0 and prints
 *           through Boot_DrawLockoutText; the TAS ('JU', US console) passes on frame 3; the lockout
 *           never runs in the corpus
 */
void Boot_CheckRegion(uint32_t d1, uint32_t d3, uint32_t d4, uint32_t d5, uint32_t d6, uint32_t a1, uint32_t a2,
                      uint32_t a3, uint32_t a4, uint32_t a6)
{
    (void)hw::port_read16(hw::VDP_CTRL);   /* tst.w: a status read resets the VDP's command latch */
    uint8_t version = (uint8_t)((hw::port_read8(hw::IO_VERSION) >> 6) & 3);
    uint8_t region = rom_bytes(Tbl_RegionByVersion)[version];
    if (region == 0) {
        hang();
        return;
    }
    const u8 *header = rom_bytes(Hdr_Regions);
    for (int i = 0; i < 16; i++) {
        if (header[i] == region) {
            dk::jump(at_Boot_InitIOPorts);
            return;
        }
    }

    /* the lockout screen: display on, plane A at $C000, H40, a 64x32 plane, auto-increment 2 */
    static const uint16_t regs[] = { 0x8164, 0x8230, 0x8C81, 0x8F02, 0x9001 };
    for (uint16_t r : regs)
        hw::port_write16(hw::VDP_CTRL, r);
    hw::port_write32(hw::VDP_CTRL, 0xC0020000u);   /* CRAM write, colour 1 */
    hw::port_write16(hw::VDP_DATA, 0x0EEE);
    hw::port_write32(hw::VDP_CTRL, 0x40000000u);   /* VRAM write at 0: the font's tiles */
    const u8 *font = rom_bytes(Art_LockoutFont);
    uint32_t nibble = 0x10000000u;
    for (int glyph = 0; glyph < 59; glyph++) {
        for (int line = 0; line < 8; line++) {
            uint8_t bits = *font++;
            uint32_t tile_row = 0;
            for (int px = 0; px < 8; px++) {   /* ror.b: bit 0 first, into the lowest nibble */
                nibble = nibble << 4 | nibble >> 28;
                if (bits & 1)
                    tile_row |= nibble;
                bits = (uint8_t)(bits >> 1 | bits << 7);
            }
            hw::port_write32(hw::VDP_DATA, tile_row);
        }
    }

    /* d1's low byte is the row: 8 for the first line, + 1 per line after it */
    uint8_t row = 8;
    draw_line(Str_LockoutDeveloped, row);
    const u8 *letter = header;
    for (; *letter != ' '; letter++) {
        const u8 *entry = rom_bytes(Tbl_RegionLockout);
        for (;; entry += 6) {
            uint8_t code = entry[1];   /* the low byte of the entry's word */
            if (code == 0)
                break;
            if (code != *letter)
                continue;
            if (letter[1] == ' ' && letter != header)
                draw_line(Str_LockoutAmp, ++row);   /* "&" before the last of several names */
            uint32_t offset = (uint32_t)entry[2] << 24 | (uint32_t)entry[3] << 16 | (uint32_t)entry[4] << 8 | entry[5];
            draw_line(Tbl_RegionByVersion + offset, ++row);
            break;
        }
    }
    draw_line(Str_LockoutSystems, ++row);
    hang();
}
