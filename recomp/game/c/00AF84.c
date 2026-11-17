#include "game.h"

/* The V-int scroll modes $28-$3C ($00AF84-$00AFD9): the line-scroll DMAs, most ending in the H-int
 * raster's table swap and re-arm (HInt_SwapRearm turns the display back on). V-int code: D2
 * checks it. */

enum {
    VINT_RASTER_DISPLAY_ON = 0x8164,   /* reg 1: display on, V-int on, DMA off */
};
static constexpr uint32_t VINT_RASTER_HSCROLL_WRITE = 0x7C000002u;   /* VRAM write $BC00 */
static constexpr uint32_t VINT_RASTER_VSRAM_WRITE = 0x40000010u;     /* VSRAM write 0 */

/* both planes' scroll words as the one long the asm moves (plane A high, plane B low) */
static uint32_t raster_scroll_pair(const u16 &a, const u16 &b)
{
    return (uint32_t)(uint16_t)a << 16 | (uint16_t)b;
}

/* VInt_ScrollLinesARaster - scroll mode $28: plane A's line H-scroll DMA, then the H-int raster
 * swapped and re-armed.
 * In:  a0 = VDP control port, a2 = the DMA command scratch
 * Out: as HInt_SwapRearm
 * Evidence: bsr Scroll_DMALinesA / bra HInt_SwapRearm; 6 corpus runs, TAS frames 113312-116145
 */
void VInt_ScrollLinesARaster(uint32_t ctrl, uint32_t scratch)
{
    Scroll_DMALinesA(ctrl, scratch);
    dk::jump(at_HInt_SwapRearm);
}

/* VInt_ScrollLinesColumns - scroll mode $2C: both planes' line H-scroll and their 2-cell column
 * V-scroll.
 * In:  a0 = VDP control port, a2 = the DMA command scratch
 * Out: reg 1 = $64
 * Evidence: bsr Scroll_DMALines / bsr Scroll_DMAColumnsAB / move.w #$8164,(a0); $3BB88 also jmps
 *           here directly; 14 corpus runs
 */
void VInt_ScrollLinesColumns(uint32_t ctrl, uint32_t scratch)
{
    Scroll_DMALines(ctrl, scratch);
    Scroll_DMAColumnsAB(ctrl, scratch);
    hw::port_write16(ctrl, VINT_RASTER_DISPLAY_ON);
}

/* VInt_ScrollLinesA - scroll mode $30: plane A's line H-scroll DMA only.
 * In:  a0 = VDP control port, a2 = the DMA command scratch
 * Out: reg 1 = $64
 * Evidence: bsr Scroll_DMALinesA / move.w #$8164,(a0); 5 corpus runs
 */
void VInt_ScrollLinesA(uint32_t ctrl, uint32_t scratch)
{
    Scroll_DMALinesA(ctrl, scratch);
    hw::port_write16(ctrl, VINT_RASTER_DISPLAY_ON);
}

/* VInt_ScrollLinesBRaster - scroll mode $34: plane B's line H-scroll DMA, then the H-int raster
 * swapped and re-armed.
 * In:  a0 = VDP control port, a2 = the DMA command scratch
 * Out: as HInt_SwapRearm
 * Evidence: bsr Scroll_DMALinesB / bra HInt_SwapRearm; 16 corpus runs (stage 2)
 */
void VInt_ScrollLinesBRaster(uint32_t ctrl, uint32_t scratch)
{
    Scroll_DMALinesB(ctrl, scratch);
    dk::jump(at_HInt_SwapRearm);
}

/* VInt_ScrollLinesRaster - scroll mode $38: the V-scroll long, both planes' line H-scroll DMA,
 * then the H-int raster swapped and re-armed.
 * In:  a0/a1 = VDP ports, a2 = the DMA command scratch
 * Out: as HInt_SwapRearm
 * Evidence: move.l #$40000010,(a0) / move.l g_VScrollA,(a1) / bsr Scroll_DMALines / bra
 *           HInt_SwapRearm; set with HInt_InstallVScrollA by $334D2 and $471C0; 15 corpus runs
 */
void VInt_ScrollLinesRaster(uint32_t ctrl, uint32_t data, uint32_t scratch)
{
    hw::port_write32(ctrl, VINT_RASTER_VSRAM_WRITE);
    hw::port_write32(data, raster_scroll_pair(g_VScrollA, g_VScrollB));
    Scroll_DMALines(ctrl, scratch);
    dk::jump(at_HInt_SwapRearm);
}

/* VInt_ScrollPlanesRaster - scroll mode $3C: the whole-plane V- and H-scroll longs, then the
 * H-int raster swapped and re-armed.
 * In:  a0/a1 = VDP ports
 * Out: as HInt_SwapRearm
 * Evidence: move.l #$40000010,(a0) / move.l g_VScrollA,(a1) / move.l #$7C000002,(a0) / move.l
 *           g_HScrollA,(a1) / bra HInt_SwapRearm; $3BB88 also jmps here directly; 4 corpus runs
 */
void VInt_ScrollPlanesRaster(uint32_t ctrl, uint32_t data)
{
    hw::port_write32(ctrl, VINT_RASTER_VSRAM_WRITE);
    hw::port_write32(data, raster_scroll_pair(g_VScrollA, g_VScrollB));
    hw::port_write32(ctrl, VINT_RASTER_HSCROLL_WRITE);
    hw::port_write32(data, raster_scroll_pair(g_HScrollA, g_HScrollB));
    dk::jump(at_HInt_SwapRearm);
}
