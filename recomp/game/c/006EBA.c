#include "game.h"

/* The a5 population waits with the count in the op ($6EBA-$6EDD) and their wait tail, the wave
 * ending ops and the kill group start ($006EBA-$006F55). */

/* Op5_WaitYield - the a5 population waits' tail: clears the wave timer (the script runs again next
 * frame), leaves the pc on the waiting op and yields.
 * In:  a5 = the op to resume at; a6 = the wave
 * Out: timer = 0, pc = a5; the stack pointer VM5_RunObject saved, then its caller's return; the
 *      condition codes of the move.l a5
 * Evidence: clr.l $2E(a6) / move.l a5,$1E(a6) / movea.l (g_VM5SavedSP).w,a7 / rts; bra from
 *           Op5_WaitPopulation, the tail of Op5_WaitPopulation0-6; 86 corpus runs
 */
void Op5_WaitYield(uint32_t a5, Vm5Wave *wave, dk::Flags *ccr)
{
    wave->timer = 0;
    wave->pc.v = a5;
    *ccr = dk::Flags::test(a5, 4, ccr->x);
    dk::set_stack_pointer(g_VM5SavedSP);
}

/* the fixed-count waits: continue past the op when g_ObjPopulation <= n, else back to the op and
   wait (cmp.w (g_ObjPopulation).w,d0 / bge VM5_Next / subq.l #2,a5) */
static void wait_population(uint32_t n, uint32_t *d0, uint32_t *a0, uint32_t *a5, uint32_t a6, dk::Flags *ccr)
{
    (void)a0;
    *d0 = n;
    *ccr = dk::Flags::compare(*d0, g_ObjPopulation, 2, ccr->x);
    if ((int16_t)(uint16_t)*d0 >= (int16_t)g_ObjPopulation) {
        dk::jump(at_VM5_Next);
        return;
    }
    *a5 -= 2;
    PTR(Vm5Wave) wave;
    wave.v = a6;
    Op5_WaitYield(*a5, wave, ccr);
}

/* Op5_WaitPopulation6 - a5 op: waits until g_ObjPopulation is at most 6.
 * In:  a5 = script (no operands)
 * Out: d0 = 6; NEXT, or yields with the pc on this op (Op5_WaitYield)
 * Evidence: moveq #6,d0 then Op5_WaitPopulation0's test; use $34550 (`6EBA 6D10`); 3 corpus runs
 */
void Op5_WaitPopulation6(uint32_t *d0, uint32_t *a0, uint32_t *a5, uint32_t a6, dk::Flags *ccr)
{
    wait_population(6, d0, a0, a5, a6, ccr);
}

/* Op5_WaitPopulation5 - a5 op: waits until g_ObjPopulation is at most 5.
 * In:  a5 = script (no operands)
 * Out: d0 = 5; NEXT, or yields
 * Evidence: moveq #5,d0; uses $34A24, $34A40; 1 corpus run
 */
void Op5_WaitPopulation5(uint32_t *d0, uint32_t *a0, uint32_t *a5, uint32_t a6, dk::Flags *ccr)
{
    wait_population(5, d0, a0, a5, a6, ccr);
}

/* Op5_WaitPopulation4 - a5 op: waits until g_ObjPopulation is at most 4.
 * In:  a5 = script (no operands)
 * Out: d0 = 4; NEXT, or yields
 * Evidence: moveq #4,d0; uses $34226, $3432A; 7 corpus runs
 */
void Op5_WaitPopulation4(uint32_t *d0, uint32_t *a0, uint32_t *a5, uint32_t a6, dk::Flags *ccr)
{
    wait_population(4, d0, a0, a5, a6, ccr);
}

/* Op5_WaitPopulation3 - a5 op: waits until g_ObjPopulation is at most 3.
 * In:  a5 = script (no operands)
 * Out: d0 = 3; NEXT, or yields
 * Evidence: moveq #3,d0; uses $41A26, $41A74; 3 corpus runs
 */
void Op5_WaitPopulation3(uint32_t *d0, uint32_t *a0, uint32_t *a5, uint32_t a6, dk::Flags *ccr)
{
    wait_population(3, d0, a0, a5, a6, ccr);
}

/* Op5_WaitPopulation2 - a5 op: waits until g_ObjPopulation is at most 2.
 * In:  a5 = script (no operands)
 * Out: d0 = 2; NEXT, or yields
 * Evidence: moveq #2,d0; uses $16E44, $3423E; 3 corpus runs
 */
void Op5_WaitPopulation2(uint32_t *d0, uint32_t *a0, uint32_t *a5, uint32_t a6, dk::Flags *ccr)
{
    wait_population(2, d0, a0, a5, a6, ccr);
}

/* Op5_WaitPopulation1 - a5 op: waits until g_ObjPopulation is at most 1.
 * In:  a5 = script (no operands)
 * Out: d0 = 1; NEXT, or yields
 * Evidence: moveq #1,d0; 39 uses, e.g. $16B42 (`6ECE 6AC4`); 13 corpus runs
 */
void Op5_WaitPopulation1(uint32_t *d0, uint32_t *a0, uint32_t *a5, uint32_t a6, dk::Flags *ccr)
{
    wait_population(1, d0, a0, a5, a6, ccr);
}

/* Op5_WaitPopulation0 - a5 op: waits until g_ObjPopulation is 0 (every counted enemy gone).
 * In:  a5 = script (no operands)
 * Out: d0 = 0; NEXT, or yields with the pc on this op
 * Evidence: moveq #0,d0 / cmp.w (g_ObjPopulation).w,d0 / bge VM5_Next / subq.l #2,a5 then
 *           Op5_WaitYield; 259 uses, e.g. $16B86 (`6ED2 6AC4 03C0 6E9A 6A72`); 86 corpus runs
 */
void Op5_WaitPopulation0(uint32_t *d0, uint32_t *a0, uint32_t *a5, uint32_t a6, dk::Flags *ccr)
{
    wait_population(0, d0, a0, a5, a6, ccr);
}

/* Op5_EndOtherWaves - a5 op: ends every other wave director: a fresh serial (++g_WaveSerial)
 * becomes both g_WaveGen and this wave's, so all the others (serial below g_WaveGen) end on their
 * next frame.
 * In:  a5 = script (no operands); a6 = the wave
 * Out: d0's low word = the serial; NEXT
 * Evidence: addq.w #1,(g_WaveSerial).w / move.w (g_WaveSerial).w,d0 / move.w d0,(g_WaveGen).w /
 *           move.w d0,$A(a6) / NEXT; uses $2201E, $22042; 3 corpus runs
 */
void Op5_EndOtherWaves(uint16_t *d0, uint32_t *a0, uint32_t *a5, Vm5Wave *wave)
{
    g_WaveSerial = (uint16_t)(g_WaveSerial + 1);
    *d0 = g_WaveSerial;
    g_WaveGen = *d0;
    wave->serial = *d0;
    vm5_next(a0, a5);
}

/* Op5_EndOlderWaves - a5 op: ends the wave directors started before this one (g_WaveGen = this
 * wave's serial; newer ones keep running).
 * In:  a5 = script (no operands); a6 = the wave
 * Out: NEXT
 * Evidence: move.w $A(a6),(g_WaveGen).w / NEXT; 244 uses, usually the first op, e.g. $1693A
 *           (`6F00 6DD4 0003 0004 6FB2`); 40 corpus runs
 */
void Op5_EndOlderWaves(uint32_t *a0, uint32_t *a5, Vm5Wave *wave)
{
    g_WaveGen = wave->serial;
    vm5_next(a0, a5);
}

/* Op5_StartGroup - a5 op: starts a kill group for the spawns that follow: a pool block (members,
 * kills left, reward action, deadline) the wave links; an older group still waiting for members
 * is closed at its spawned count first, one whose members are all gone is reused.
 * In:  a5 = script: word members, word reward action (Tbl_012EF0), word time limit in frames
 *      (0 = none); a6 = the wave
 * Out: the group block (spawned / dead = 0, deadline = g_FrameCounter + limit or 0), d0 = the
 *      deadline; NEXT. On a full pool the wave has no group and only 4 operand bytes are skipped,
 *      so the limit word then runs as an op (the original's, kept; never seen)
 * Evidence: move.w $32(a6),d0 / beq / cmp.w 8(a0),d0 / ble (reuse) / move.w d0,(a0) / bsr
 *           ObjPool_Alloc / bne / move.w a0,$32(a6) / move.w (a5),(a0) / move.w (a5)+,2(a0) / ... /
 *           add.l (g_FrameCounter).w,d0 / clr.w 6 / 8; clr.w $32(a6) / addq.l #4,a5; uses $16B66
 *           (`6F0A 0005 001B 00B4`: kill 5 within 180 frames -> action $1B), $17012; 27 corpus runs
 */
void Op5_StartGroup(uint32_t *d0, uint32_t *a0, uint32_t *a5, Vm5Wave *wave)
{
    *d0 = (*d0 & 0xFFFF0000u) | (uint16_t)wave->group;
    bool reuse = false;
    if ((uint16_t)*d0 != 0) {
        *a0 = (uint32_t)(int32_t)(int16_t)*d0;
        PTR(Vm5Group) old;
        old.v = *a0;
        *d0 = (*d0 & 0xFFFF0000u) | old->spawned;
        if ((int16_t)old->spawned <= (int16_t)old->dead)
            reuse = true;   /* every member spawned so far is gone: start over in this block */
        else
            old->members = (uint16_t)*d0;   /* close it at the members it has */
    }
    if (!reuse) {
        ObjPool_Alloc(d0, a0);
        if (*d0 != 0) {
            wave->group = 0;
            *a5 += 4;
            vm5_next(a0, a5);
            return;
        }
        wave->group = (int16_t)(uint16_t)*a0;
    }
    PTR(Vm5Group) group;
    group.v = *a0;
    uint16_t members = vm5_word(a5);
    group->members = members;
    group->kills_left = members;
    group->reward = vm5_word(a5);
    *d0 = vm5_word(a5);
    if (*d0 != 0)
        *d0 += g_FrameCounter;
    group->deadline = *d0;
    group->spawned = 0;
    group->dead = 0;
    vm5_next(a0, a5);
}
