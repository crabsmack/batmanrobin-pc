#include "game.h"

/* VM5_CloseGroup - closes a wave's kill group at the wave's end: its member target becomes the
 * spawned count, and the block is freed when all of those are already gone.
 * In:  a6 = the wave (group = the group block's handle or 0)
 * Out: d0's low word = the group handle (0) or its spawned count
 * Evidence: move.w $32(a6),d0 / beq / move.w 6(a0),d0 / move.w d0,(a0) / cmp.w 8(a0),d0 / bgt /
 *           bsr ObjPool_Free; called by VM5_EndWave
 */
uint16_t VM5_CloseGroup(uint32_t a6)
{
    PTR(Vm5Wave) wave;
    wave.v = a6;
    uint16_t d0 = (uint16_t)wave->group;
    if (d0 == 0)
        return d0;
    PTR(Vm5Group) group;
    group.v = (uint32_t)(int32_t)wave->group;
    d0 = group->spawned;
    group->members = d0;
    if ((int16_t)d0 <= (int16_t)group->dead)
        ObjPool_Free(group.raw());
    return d0;
}

/* SpawnAction_GroupMemberReward - spawn action 1 (a kill-group member died): counts the group's
 * kills left down and, at 0 before the deadline (0 = none), runs the group's reward action at the
 * dead enemy's position; then counts the member gone (SpawnAction_GroupCountGone).
 * In:  d1 = the group block (the dead object's spawn_arg, linked by VM5_SpawnEntry) or 0; a6 =
 *      the dead object, d4 / d5 its position; the registers go on to the reward action
 * Out: the reward's registers (SpawnAction_Run); d0's low word = the group's member target
 * Evidence: tst.w d1 / subq.w #1,2(a0) / bne / move.l $A(a0),d0 / beq / cmp.l (g_FrameCounter).w,d0
 *           / bcs / move.w 4(a0),d0 / jsr SpawnAction_Run with a0 saved / bra $6FA2; Tbl_012EF0
 *           entry 1; 22 corpus runs. The bra into the next file is a call then return here: the
 *           group pointer goes in a0, which this function's contract lists as a clobber
 */
void SpawnAction_GroupMemberReward(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4,
                                   uint32_t *d5, uint32_t *d6, uint32_t *d7, uint32_t *a1, uint32_t a2, uint32_t a3,
                                   uint32_t a4, uint32_t a5, uint32_t a6)
{
    if ((uint16_t)*d1 == 0)
        return;
    PTR(Vm5Group) group;
    group.v = (uint32_t)(int32_t)(int16_t)*d1;
    group->kills_left = (uint16_t)(group->kills_left - 1);
    if (group->kills_left == 0) {
        *d0 = group->deadline;
        if (*d0 == 0 || *d0 >= g_FrameCounter) {   /* bcs: past the deadline, no reward */
            *d0 = (*d0 & 0xFFFF0000u) | group->reward;
            SpawnAction_Run(d0, d1, d2, d3, d4, d5, d6, d7, a1, a2, a3, a4, a5, a6);
        }
    }
    dk::Flags gone{};
    *d0 = (*d0 & 0xFFFF0000u) | SpawnAction_GroupCountGone(group.raw(), &gone);
}
