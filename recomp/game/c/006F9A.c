#include "game.h"

/* SpawnAction_GroupCountGone - counts one kill-group member gone and frees the group block once
 * that reaches the member target.
 * In:  a0 = the group block
 * Out: d0 = the member target; ObjPool_Free when every member is gone (the condition codes of its
 *      addq.b to g_ObjPoolFree), else those of the cmp.w
 * Evidence: addq.w #1,8(a0) / move.w (a0),d0 / cmp.w 8(a0),d0 / ble.w ObjPool_Free / rts; the tail
 *           of SpawnAction_GroupMemberReward and SpawnAction_GroupMemberGone; 22 corpus runs. The
 *           ble into ObjPool_Free is a call then return here
 */
uint16_t SpawnAction_GroupCountGone(uint32_t a0, dk::Flags *ccr)
{
    PTR(Vm5Group) group;
    group.v = a0;
    group->dead = (uint16_t)(group->dead + 1);
    uint16_t members = group->members;
    *ccr = dk::Flags::compare(members, group->dead, 2, ccr->x);
    if ((int16_t)members > (int16_t)group->dead)
        return members;
    ObjPool_Free(a0);
    uint8_t free_blocks = g_ObjPoolFree;   /* addq.b #1: its flags */
    *ccr = dk::Flags{ free_blocks == 0, (free_blocks & 0x80) != 0, free_blocks == 0, free_blocks == 0x80,
                      free_blocks == 0 };
    return members;
}

/* SpawnAction_GroupMemberGone - spawn action 2, and Obj_Destroy's call for an object still
 * carrying action 1 (removed, e.g. off screen, without its kill counted): counts the member gone;
 * no reward.
 * In:  d1 = the group block or 0 (nothing)
 * Out: as SpawnAction_GroupCountGone
 * Evidence: tst.w d1 / bne / rts / movea.w d1,a0 falling into $6FA2; Tbl_012EF0 entry 2; bsr from
 *           Obj_Destroy when $19(a6) == 1 with d1 = $1A(a6)
 */
void SpawnAction_GroupMemberGone(uint16_t d1)
{
    if (d1 == 0)
        return;
    dk::Flags flags{};
    SpawnAction_GroupCountGone((uint32_t)(int32_t)(int16_t)d1, &flags);
}

/* Op5_Spawn - a5 op: spawns one enemy from a spawn entry (VM5_SpawnEntry), then VM5_AfterSpawn
 * reloads the timer, switches to the after-quota script when the quota is met, and yields.
 * In:  a5 = script: word offset to a spawn entry; a6 = the wave
 * Out: the wave's pc = the next op; as VM5_SpawnEntry and VM5_AfterSpawn (the next file) leave
 *      the registers
 * Evidence: bsr VM5_SpawnEntry / move.l a5,$1E(a6) / bra VM5_AfterSpawn; 694 uses, e.g. $16942
 *           (`6FB2 0052 6AC4 00F0`); 87 corpus runs
 */
void Op5_Spawn(uint32_t *d0, uint32_t *d1, uint32_t d2, uint32_t d3, uint32_t *d4, uint32_t *d5, uint32_t *d6,
               uint32_t *d7, uint32_t *a0, uint32_t *a1, uint32_t *a2, uint32_t *a3, uint32_t *a4, uint32_t *a5,
               uint32_t *a6)
{
    VM5_SpawnEntry(d0, d1, d2, d3, d4, d5, d6, d7, a0, a1, a2, a3, a4, a5, *a6);
    PTR(Vm5Wave) wave;
    wave.v = *a6;
    wave->pc.v = *a5;
    dk::jump(at_VM5_AfterSpawn);
}

/* Op5_SpawnMulti - a5 op: spawns n+1 enemies at once from a list of spawn-entry offsets, then as
 * Op5_Spawn.
 * In:  a5 = script: word n (count - 1), n+1 word entry offsets; a6 = the wave
 * Out: d0's low word = $FFFF (dbra; the upper word VM5_SpawnEntry's); as Op5_Spawn
 * Evidence: move.w (a5)+,d0 / move.w d0,-(a7) / bsr VM5_SpawnEntry / move.w (a7)+,d0 / dbra /
 *           move.l a5,$1E(a6) / bra VM5_AfterSpawn; 156 uses, e.g. $1695A (`6FBC 0002 0038 003E
 *           004C`: 3 enemies); 36 corpus runs
 */
void Op5_SpawnMulti(uint32_t *d0, uint32_t *d1, uint32_t d2, uint32_t d3, uint32_t *d4, uint32_t *d5,
                    uint32_t *d6, uint32_t *d7, uint32_t *a0, uint32_t *a1, uint32_t *a2, uint32_t *a3, uint32_t *a4,
                    uint32_t *a5, uint32_t *a6)
{
    *d0 = (*d0 & 0xFFFF0000u) | vm5_word(a5);
    do {
        uint16_t left = (uint16_t)*d0;   /* kept on the stack across the spawn in the original */
        VM5_SpawnEntry(d0, d1, d2, d3, d4, d5, d6, d7, a0, a1, a2, a3, a4, a5, *a6);
        *d0 = (*d0 & 0xFFFF0000u) | left;
        *d0 = (*d0 & 0xFFFF0000u) | (uint16_t)(*d0 - 1);
    } while ((uint16_t)*d0 != 0xFFFF);
    PTR(Vm5Wave) wave;
    wave.v = *a6;
    wave->pc.v = *a5;
    dk::jump(at_VM5_AfterSpawn);
}

/* Op5_SpawnRandom - a5 op: spawns one enemy from a spawn entry picked at random from n, then as
 * Op5_Spawn; the script goes on after the table.
 * In:  a5 = script: word n, n word entry offsets; a6 = the wave
 * Out: the wave's pc = past the table; as Op5_Spawn
 * Evidence: move.w (a5)+,d0 / lea (a5,d0.w),a0 / adda.w d0,a0 / move.l a0,$1E(a6) / bsr Rand_Below /
 *           add.w d0,d0 / adda.w d0,a5 / bsr VM5_SpawnEntry / bra VM5_AfterSpawn; 295 uses, e.g.
 *           $169C4 (`6FD2 0003 0016 001C 0022`); 74 corpus runs
 */
void Op5_SpawnRandom(uint32_t *d0, uint32_t *d1, uint32_t d2, uint32_t d3, uint32_t *d4, uint32_t *d5,
                     uint32_t *d6, uint32_t *d7, uint32_t *a0, uint32_t *a1, uint32_t *a2, uint32_t *a3, uint32_t *a4,
                     uint32_t *a5, uint32_t *a6)
{
    uint16_t n = vm5_word(a5);
    *d0 = (*d0 & 0xFFFF0000u) | n;
    PTR(Vm5Wave) wave;
    wave.v = *a6;
    wave->pc.v = *a5 + 2 * (uint32_t)(int32_t)(int16_t)n;   /* past the n offsets */
    Rand_Below(d0);
    *d0 = (*d0 & 0xFFFF0000u) | (uint16_t)(*d0 + *d0);
    *a5 += (uint32_t)(int32_t)(int16_t)(uint16_t)*d0;
    VM5_SpawnEntry(d0, d1, d2, d3, d4, d5, d6, d7, a0, a1, a2, a3, a4, a5, *a6);
    dk::jump(at_VM5_AfterSpawn);
}
