#include "game.h"

/* The plane geometry presets ($012572-$0125D5): scene setup callbacks that set the VDP plane layout
 * the map layers draw into (read by Layer_Init into the layer, +$36-$40, and by the scene ops). */

/* the layout both presets share: plane A and B name tables, a plane's bytes, a redraw's blocks */
static void set_planes(uint16_t row_bytes, uint16_t cells_x, uint16_t cells_y)
{
    g_PlaneAVRAM = 0xC000;
    g_PlaneBVRAM = 0xE000;
    g_PlaneRowBytes = row_bytes;
    g_PlaneBytes = 0x2000;
    g_PlaneCellsX = cells_x;
    g_PlaneCellsY = cells_y;
    g_ScreenBlocksX = 0x16;                  /* 22 blocks of 16 px: the screen and a margin */
    g_ScreenBlocksY = 0x10;
}

/* Layer_SetPlanes64x64 - scene setup callback: VDP planes of 64x64 cells (plane A $C000, B $E000,
 * $80 bytes a row, $2000 a plane); a screen redraw is 22x16 blocks.
 * In:  none
 * Out: g_PlaneAVRAM/BVRAM, g_PlaneRowBytes, g_PlaneBytes, g_PlaneCellsX/Y = $40/$40,
 *      g_ScreenBlocksX/Y = $16/$10
 * Evidence: eight move.w #imm to the globals, in this order; 12 scene setups ($01FBD2, $03FF32,
 *           $047548, ...); every scene start
 */
void Layer_SetPlanes64x64(void)
{
    set_planes(0x80, 0x40, 0x40);
}

/* Layer_SetPlanes128x32 - scene setup callback: VDP planes of 128x32 cells ($100 bytes a row, $2000
 * a plane, A $C000, B $E000); a screen redraw is 22x16 blocks.
 * In:  none
 * Out: as Layer_SetPlanes64x64 with g_PlaneRowBytes = $100, g_PlaneCellsX/Y = $80/$20
 * Evidence: the same eight writes with $100/$80/$20; 12 scene setups ($01FA7E, $032E16, $0476F2,
 *           ...); 22 corpus runs incl. the TAS
 */
void Layer_SetPlanes128x32(void)
{
    set_planes(0x100, 0x80, 0x20);
}
