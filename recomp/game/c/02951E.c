#include "game.h"

/* the coroutine's script after this native (the bytes after its jsr $7FE.w) */
static constexpr uint32_t Scr2_02952E = 0x02952Eu;

/* Boss_BeamShip_SetSpotVertex - native in the beam-spot coroutine: while the spot's position is
 * swapped into the ship's x/y (Op2_SwapSavedPos), stores it as span vertex 1 (the disc centre)
 * In:  ship = the ship with the spot in its x/y (a6); the rest its registers
 * Out: g_TriX1/g_TriY1 = the spot; the script continues at Scr2_02952E (jsr $7FE.w), which swaps
 *      back
 * Evidence: move.w $3E(a6),g_TriX1 / move.w $42(a6),g_TriY1 / jsr $7FE.w; Op2_Native at $02951C
 *           in Scr2_0294A8/0294EC after the spot's move; $33A88 draws the beam cone to (g_TriX1,
 *           g_TriY1) and the disc there; TAS f77306-f79013 (1663 frames)
 */
void Boss_BeamShip_SetSpotVertex(uint32_t d0, uint32_t d1, uint32_t d2, uint32_t d3, uint32_t d4, uint32_t d5, uint32_t d6,
                                 uint32_t d7, uint32_t a1, uint32_t a3, uint32_t a4, uint32_t a5, const Vm2Obj *ship)
{
    g_TriX1 = (u16)ship->x;
    g_TriY1 = (u16)ship->y;
    dk::jump_script(at_VM2_Enter, Scr2_02952E);
}
