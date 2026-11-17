#include "game.h"

/* the jet trooper body loop after the native (the bytes after its jsr $7FE.w) */
static constexpr uint32_t Scr2_04242C = 0x04242Cu;

/* Enemy_JetTrooper_RideTubeScroll - native in the jetpack trooper's body loop ($0423CE): moves the
 * trooper with the ring tunnel's scroll (its 16.16 x += the tunnel speed), then back into the loop
 * In:  self = the trooper (a6); g_SceneVars = the tunnel's TubeScrollVars (Stage4_TubeScroll)
 * Out: d0 = the tunnel speed (16.16); self's 16.16 x + it; the script continues at Scr2_04242C
 * Evidence: move.l (g_SceneVars+$4).w,d0 / add.l d0,$3E(a6) / jsr $7FE.w; Op2_Native at $04241E in the
 *           body loop after Op2_BranchVar0None $0001 (taken when var 0 is set); no corpus run reaches it
 *           (raw-stubs 2026-10-30); the contract is the Op2_Native fragments' (Player_CopyPadToVar2's)
 *           with d0 written; 042420.syn checks it
 */
void Enemy_JetTrooper_RideTubeScroll(uint32_t *d0, uint32_t d1, uint32_t d2, uint32_t d3, uint32_t d4, uint32_t d5,
                                     uint32_t d6, uint32_t d7, uint32_t a1, uint32_t a3, uint32_t a4, uint32_t a5,
                                     Vm2Obj *self)
{
    const TubeScrollVars *tube = (const TubeScrollVars *)g_SceneVars;
    *d0 = (uint32_t)tube->speed;
    uint32_t x = ((uint32_t)(uint16_t)self->x << 16) | self->x_frac;
    x += *d0;
    self->x = (int16_t)(uint16_t)(x >> 16);
    self->x_frac = (uint16_t)x;
    dk::jump_script(at_VM2_Enter, Scr2_04242C);
}
