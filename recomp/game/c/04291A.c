#include "game.h"

/* the a4 script after the native (its jsr $53C6.w: VM4_Enter): Op4_MoveXY, Op4_Draw */
static constexpr uint32_t Scr4_042926 = 0x042926u;

/* Fx_RideTubeScroll - a4 native in an effect script of the ring tunnel ($042900): moves the effect with
 * the tunnel's scroll (its x += the tunnel speed's integer word), then the move and draw ops
 * In:  fx = the effect object (a6); g_SceneVars = the tunnel's TubeScrollVars (Stage4_TubeScroll)
 * Out: d0's low word = the speed's integer word; fx->x + it; the script continues at Scr4_042926
 * Evidence: move.w (g_SceneVars+$4).w,d0 / add.w d0,$2E(a6) / jsr $53C6.w; Op4_Native at $042918 after
 *           Op4_JitterX, Op4_RangeVelY, Op4_SetDepth, Op4_SetAnim; no corpus run reaches it (raw-stubs
 *           2026-10-30); the contract is the a4 natives' (Boss_Hatter_LittleHatFollow's registers) with d0
 *           written; 04291A.syn checks it
 */
void Fx_RideTubeScroll(uint32_t *d0, uint32_t d1, uint32_t d2, uint32_t d3, uint32_t d4, uint32_t d5, uint32_t d6,
                       uint32_t d7, uint32_t a0, uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a5, Vm4Obj *fx)
{
    const TubeScrollVars *tube = (const TubeScrollVars *)g_SceneVars;
    uint16_t step = (uint16_t)((uint32_t)tube->speed >> 16);
    *d0 = (*d0 & 0xFFFF0000u) | step;
    fx->x = (int16_t)(uint16_t)(fx->x + step);
    dk::jump_script(at_VM4_Enter, Scr4_042926);
}
