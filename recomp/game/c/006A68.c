#include "game.h"

/* $006A68-$006AA7: the a5 VM's yield and end, and the spawn-interval ops. A wave script runs
 * inside VM5_RunObject, which saved the stack pointer in g_VM5SavedSP: an op yields by restoring
 * it and returning to the object list walk. */

static void next(const u16 **script)
{
    dk::jump(dk::code((int16_t)(uint16_t)*(*script)++));
}

/* VM5_Yield - the a5 VM's yield tail (not dispatched): ends this frame's run of the wave script,
 * saving the script position; the timer the op before set decides when it resumes.
 * In:  a5 = the script; a6 = the wave object
 * Out: self->pc; rts through g_VM5SavedSP to the object list walk
 * Evidence: move.l a5,$1E(a6) / movea.l (g_VM5SavedSP).w,a7 / rts; entered only by bra from Op5_Wait /
 *           Op5_WaitRandom
 */
void VM5_Yield(const u16 *script, Vm5Wave *self)
{
    self->pc = script;
    dk::set_stack_pointer(g_VM5SavedSP);
}

/* Op5_End - a5 op: ends the wave: restores the saved stack, then VM5_EndWave.
 * In:  script (no operands); a6 = the wave object
 * Out: as VM5_EndWave (the rts goes to the object list walk; the object is gone)
 * Evidence: movea.l (g_VM5SavedSP).w,a7, falling into VM5_EndWave; Wave_Start's default quota
 *           address $939E holds a lone $6A72; 95 script uses, e.g. $21D76, $21D86; 35 corpus runs
 */
void Op5_End(uint16_t *d0, uint32_t *a0, uint32_t *a1, Vm5Wave *self, dk::Flags *ccr)
{
    dk::set_stack_pointer(g_VM5SavedSP);
    VM5_EndWave(d0, a0, a1, self, ccr);
}

/* VM5_EndWave - ends a wave director: closes its kill group (VM5_CloseGroup), unlinks and frees
 * the object (Obj_UnlinkFree) and decrements g_WaveCount (floored at 0).
 * In:  a6 = the wave object
 * Out: d0 = VM5_CloseGroup's; a0 / a1 = Obj_UnlinkFree's; g_WaveCount; the condition codes of
 *      the subq (or the clr at 0); rts
 * Evidence: bsr.w VM5_CloseGroup / bsr.w Obj_UnlinkFree / subq.w #1,(g_WaveCount).w / bge /
 *           clr.w (g_WaveCount).w; entered from VM5_RunObject when g_WaveGen > the serial and from
 *           Op5_End
 */
void VM5_EndWave(uint16_t *d0, uint32_t *a0, uint32_t *a1, Vm5Wave *self, dk::Flags *ccr)
{
    *d0 = VM5_CloseGroup(dk::addr(self));
    dk::Flags unlink_ccr{};
    Obj_UnlinkFree(a0, a1, dk::addr(self), &unlink_ccr);
    uint16_t old = g_WaveCount;
    uint16_t left = (uint16_t)(old - 1);
    /* subq.w #1: X = C = the borrow, V = $8000 - 1 */
    *ccr = dk::Flags{ old == 0, (left & 0x8000) != 0, left == 0, old == 0x8000, old == 0 };
    g_WaveCount = left;
    if (ccr->n != ccr->v) {   /* bge not taken */
        g_WaveCount = 0;
        *ccr = dk::Flags{ ccr->x, false, true, false, false };   /* clr.w: X kept (the subq's) */
    }
}

/* Op5_ClearInterval - a5 op: clears the spawn interval and the timer, so the spawn ops continue
 * on the next frame.
 * In:  script (no operands); a6 = the wave object
 * Out: self->interval = self->timer = 0; NEXT
 * Evidence: clr.l $2A(a6) / clr.l $2E(a6) / NEXT; the spawn ops reload $2E from $2A ($7052); 29
 *           script uses, e.g. $21D60 (`6A8A 6D2A`); 28 corpus runs
 */
void Op5_ClearInterval(const u16 **script, Vm5Wave *self)
{
    self->interval = 0;
    self->timer = 0;
    next(script);
}

/* Op5_SetInterval - a5 op: sets the wait after every spawn op: interval = t * 16 (1/16-frame
 * units at the normal g_WaveTimerStep $100), timer = 0.
 * In:  script = word t (1/16 frames); a6 = the wave object
 * Out: d0 = t * 16; self->interval; self->timer = 0; NEXT
 * Evidence: moveq #0,d0 / move.w (a5)+,d0 / asl.l #4,d0 / move.l d0,$2A(a6) / clr.l $2E(a6); 43
 *           script uses, e.g. $16B48 (`6A96 01E0`: 30 frames), $21E0A (`6A96 03C0`); 24 corpus runs
 */
void Op5_SetInterval(uint32_t *d0, const u16 **script, Vm5Wave *self)
{
    *d0 = (uint32_t)(uint16_t)*(*script)++ << 4;
    self->interval = (int32_t)*d0;
    self->timer = 0;
    next(script);
}
