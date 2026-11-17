#include "game.h"

/* Op4_TakeHitPhaseA / Op4_TakeHitPhaseB ($006690-$0066AB): each new shot takes the next frame
 * phase from a toggling byte, so successive shots run their hit tests on alternate frames
 * (Op4_*OnPhase compare it with g_FrameParity). A: the player shots, B: the enemy bullets. */

/* Op4_TakeHitPhaseA - a4 op: gives the object the next frame phase from g_FxPhaseA and toggles it.
 * In:  script (no operands); self = a6
 * Out: self->hit_phase; g_FxPhaseA inverted; NEXT
 * Evidence: move.b (g_FxPhaseA).w,$18(a6) / not.b (g_FxPhaseA).w / NEXT; 36 uses, all in the player
 *           shot scripts Scr4_05034A (e.g. $5051E `6690 695E 0E00`); 151 corpus runs
 */
void Op4_TakeHitPhaseA(const u16 **script, Vm4Obj *self)
{
    self->hit_phase = g_FxPhaseA;
    g_FxPhaseA = (uint8_t)~g_FxPhaseA;
    dk::jump(dk::code((int16_t)(uint16_t)*(*script)++));
}

/* Op4_TakeHitPhaseB - a4 op: as Op4_TakeHitPhaseA from the second toggle g_FxPhaseB (enemy
 * bullets).
 * In:  script (no operands); self = a6
 * Out: self->hit_phase; g_FxPhaseB inverted; NEXT
 * Evidence: move.b (g_FxPhaseB).w,$18(a6) / not.b (g_FxPhaseB).w / NEXT; script uses $42966, $42F52,
 *           $46B52, each before Op4_HitPlayersOnPhase; 10 corpus runs
 */
void Op4_TakeHitPhaseB(const u16 **script, Vm4Obj *self)
{
    self->hit_phase = g_FxPhaseB;
    g_FxPhaseB = (uint8_t)~g_FxPhaseB;
    dk::jump(dk::code((int16_t)(uint16_t)*(*script)++));
}
