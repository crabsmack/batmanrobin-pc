#include "game.h"

/* HUD_ClearTiles - clears d7 more tiles of the panel's top row (one per $60-byte column): the
 * padding after (player 1) or before (player 2) the score's digits.
 * In:  a2 = the buffer position, d7 = the tiles to clear (low word; 0 clears none)
 * Out: a2 advanced, d7 = $FFFF (both clobbers)
 * Evidence: bra.s to the dbra d7 / 8 x clr.l (a2)+ / lea $40(a2),a2; called by HUD_StepP1Panel /
 *           HUD_StepP2Panel right after HUD_DrawScore with its d7
 */
void HUD_ClearTiles(uint16_t d7, uint32_t a2)
{
    for (uint16_t n = d7; n != 0; n--) {
        PTR(u8) tile;
        tile.v = a2;
        dk::mem_set(tile.get(), 0, 32);
        a2 += 0x60;
    }
}
