#include "game.h"

/* Arc_HeadAtMark - a4 native: points the running a4 object's heading at the marked point
 * (g_ArcTargetX/Y, stored by Op2_FxMarkPlayerTarget): heading = Mot_Atan2 of the mark minus the
 * object's x / y.
 * In:  a6 = the a4 object (Vm4Obj: x, y)
 * Out: heading (+$2A) = Mot_Atan2's d2; d0/d1 low words = the x / y offsets (as Mot_Atan2 left
 *      them), d2 = the heading; the condition codes of the move.w d2 (tst.w)
 * Evidence: move.w g_ArcTargetX,d0 / move.w g_ArcTargetY,d1 / sub.w $2E(a6),d0 / sub.w $32(a6),d1 /
 *           jsr Mot_Atan2 / move.w d2,$2A(a6) / rts; Op4_CallNative's dc.l at $0298DA and $02996E
 *           (stage 2 a4 spawn scripts after Op2_FxMarkPlayerTarget); no corpus run reaches it
 *           (crane-crash's scan of script call operands, 2026-10-30); 00924A.syn checks it.
 *           Prototype: &ccr added (the move.w's flags, as Arc_Step's)
 */
void Arc_HeadAtMark(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t a6, dk::Flags *ccr)
{
    PTR(Vm4Obj) obj;
    obj.v = a6;
    Vm4Obj *self = obj;
    *d0 = (*d0 & 0xFFFF0000u) | (uint16_t)((uint16_t)g_ArcTargetX - (uint16_t)self->x);
    *d1 = (*d1 & 0xFFFF0000u) | (uint16_t)((uint16_t)g_ArcTargetY - (uint16_t)self->y);
    Mot_Atan2(d0, d1, d2);
    self->heading = (int16_t)(uint16_t)*d2;
    *ccr = dk::Flags::test((uint16_t)*d2, 2, ccr->x);
}
