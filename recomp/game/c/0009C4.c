#include "game.h"

/* Obj_ReleaseFrames and the a2 random-branch, population, score and spawn-action ops
 * ($0009C4-$000ACB). */

/* NEXT: the handler the script's next word names, jumped to */
static void next(uint32_t *handler, const u16 **script)
{
    *handler = dk::code((int16_t)(uint16_t)*(*script)++);
    dk::jump(*handler);
}

/* Obj_ReleaseFrames - releases every sprite frame the sprite block holds (Spr_ReleaseFrame on
 * each slot, the slot cleared), then zeroes the count.
 * In:  spr = the sprite block (slots from +0, count +$14)
 * Out: slot = past the last slot released (the block itself when the count is 0); count = $FFFF
 *      (the dbra's end); d0 / d2 as Spr_ReleaseFrame leaves them (d2 = the last frame)
 * Evidence: lea (a5),a1 / move.w $14(a5),d7 / dbra / move.w (a1),d2 / bsr Spr_ReleaseFrame /
 *           clr.w (a1)+ / dbra / clr.w $14(a5); callers Obj_Destroy, $3B76, $102D6; 2.2k TAS frames
 */
void Obj_ReleaseFrames(uint16_t *d0, uint16_t *d2, uint16_t *count, u16 **slot, Vm2Sprite *spr)
{
    *slot = spr->frame;
    *count = spr->frame_count;
    if ((*count)-- == 0)
        return;   /* dbra at entry: no slot to release, the count stays (it is 0) */
    do {
        uint32_t a0 = 0;
        *d2 = **slot;
        Spr_ReleaseFrame(d0, d2, &a0);
        *(*slot)++ = 0;
    } while ((*count)-- != 0);
    spr->frame_count = 0;
}

/* Op2_BranchRandom - a2 op: a random branch: branches when a fresh random word (Rand_Next) is at
 * or above the threshold (unsigned), so with probability 1 - threshold / 65536.
 * In:  script = words threshold, offset
 * Out: rand = Rand_Next's long; the branch (Op2_Goto), else NEXT after the offset
 * Evidence: jsr Rand_Next / cmp.w (a2)+,d0 / bcc.w Op2_Goto / addq.l #2,a2 + NEXT; executed in the TAS
 */
void Op2_BranchRandom(uint32_t *rand, uint32_t *handler, const u16 **script)
{
    *rand = (uint32_t)Rand_Next();
    if ((uint16_t)*rand >= (uint16_t)*(*script)++) {
        dk::jump(at_Op2_Goto);
        return;
    }
    (*script)++;
    next(handler, script);
}

/* Op2_SwitchRandom - a2 op: a weighted random switch: branches by the offset of the first
 * (threshold, offset) pair whose threshold is at or below a fresh random word (unsigned). The
 * list has no end marker: its last threshold must be 0.
 * In:  script = (word threshold, word offset) pairs, thresholds descending
 * Out: rand = Rand_Next's long; the branch by the chosen offset (the first pair's via Op2_Goto)
 * Evidence: jsr Rand_Next / cmp.w (a2)+,d0 / bcc.w Op2_Goto / loop addq.l #2,a2 / cmp.w (a2)+,d0 /
 *           bcs / adda.w (a2),a2 + NEXT; executed in the TAS
 */
void Op2_SwitchRandom(uint32_t *rand, uint32_t *handler, const u16 **script)
{
    *rand = (uint32_t)Rand_Next();
    uint16_t r = (uint16_t)*rand;
    if (r >= (uint16_t)*(*script)++) {
        dk::jump(at_Op2_Goto);
        return;
    }
    do
        (*script)++;   /* the offset of the pair just rejected */
    while (r < (uint16_t)*(*script)++);
    *script = (const u16 *)((const u8 *)*script + (int16_t)(uint16_t)**script);
    next(handler, script);
}

/* Op2_ClearPopulation - a2 op: the object stops counting in the population: its weight comes off
 * g_ObjPopulation (not below 0) and becomes 0.
 * In:  self->population
 * Out: weight = the old weight (long); self->population = 0; NEXT (VM2_Next when no clamp)
 * Evidence: moveq #0,d0 / move.b $18(a6),d0 / clr.b $18(a6) / sub.w d0,(g_ObjPopulation) /
 *           bcc.w VM2_Next / clr.w; the same subtraction as Obj_Destroy; executed in the TAS
 */
void Op2_ClearPopulation(uint32_t *weight, uint32_t *handler, const u16 **script, Vm2Obj *self)
{
    *weight = self->population;
    self->population = 0;
    uint16_t pop = g_ObjPopulation;
    if (pop >= (uint16_t)*weight) {
        g_ObjPopulation = (uint16_t)(pop - *weight);
        dk::jump(at_VM2_Next);
        return;
    }
    g_ObjPopulation = 0;
    next(handler, script);
}

/* Op2_IncPopulation - a2 op: the object's population weight and g_ObjPopulation both + 1.
 * In:  self->population
 * Out: self->population + 1 (a byte), g_ObjPopulation + 1; NEXT
 * Evidence: addq.b #1,$18(a6) / addq.w #1,(g_ObjPopulation).w; executed in the TAS
 */
void Op2_IncPopulation(uint32_t *handler, const u16 **script, Vm2Obj *self)
{
    self->population = (uint8_t)(self->population + 1);
    g_ObjPopulation = (uint16_t)(g_ObjPopulation + 1);
    next(handler, script);
}

/* Op2_AddPopulation - a2 op: adds the operand to the object's population weight (its low byte)
 * and to g_ObjPopulation; spawners wait on the total (Op2_WaitPopBelow).
 * In:  script = word n
 * Out: n (the operand); self->population + n, g_ObjPopulation + n; NEXT
 * Evidence: move.w (a2)+,d0 / add.b d0,$18(a6) / add.w d0,(g_ObjPopulation).w; executed in the TAS
 */
void Op2_AddPopulation(uint16_t *n, uint32_t *handler, const u16 **script, Vm2Obj *self)
{
    *n = *(*script)++;
    self->population = (uint8_t)(self->population + *n);
    g_ObjPopulation = (uint16_t)(g_ObjPopulation + *n);
    next(handler, script);
}

/* Op2_SetScoreValue - a2 op: the points the object is worth (awarded by Op2_AwardScoreTo*).
 * In:  script = word points
 * Out: vm->points; NEXT
 * Evidence: move.w (a2)+,$36(a4); read by Op2_AwardScoreToHitter / ToPlayers; executed in the TAS
 */
void Op2_SetScoreValue(const u16 **script, Vm2Block *vm)
{
    uint32_t handler;
    vm->points = (int16_t)(uint16_t)*(*script)++;
    next(&handler, script);
}

/* Op2_AwardScoreToHitter - a2 op: adds the object's points to the score of the player whose box
 * struck it (attacker id 1 or 3: player 1, 2 or 4: player 2), then runs its spawn action. Any
 * other attacker: nothing, NEXT.
 * In:  g_StruckBox = the box that was hit (its struck_by id); vm->points
 * Out: d0 = the points (long) and a0 = the record, then Op2_RunSpawnAction; else d0's low word =
 *      the id, a0 = g_Player2, VM2_Next
 * Evidence: movea.l (g_StruckBox),a0 / move.w $1A(a0),d0 / cmpi.w #1,#3 (g_Player1), #2,#4
 *           (g_Player2) / bne.w VM2_Next / ext.l / add.l d0,(a0) / bra.w Op2_RunSpawnAction
 */
void Op2_AwardScoreToHitter(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5,
                            uint32_t *d6, uint32_t *d7, uint32_t *a0, uint32_t *a1, uint32_t *a2, uint32_t *a3,
                            Vm2Block **vm, uint32_t *a5, uint32_t *a6)
{
    PTR(const HitBox) box;
    box.v = g_StruckBox;
    uint16_t id = box->struck_by;
    *d0 = (*d0 & 0xFFFF0000u) | id;
    PlayerRec *p;
    if (id == 1 || id == 3) {
        p = (PlayerRec *)g_Player1;
    } else {
        p = (PlayerRec *)g_Player2;
        *a0 = dk::addr(p);
        if (id != 2 && id != 4) {
            dk::jump(at_VM2_Next);
            return;
        }
    }
    *a0 = dk::addr(p);
    *d0 = (uint32_t)(int32_t)(*vm)->points;
    p->score += *d0;
    dk::jump(at_Op2_RunSpawnAction);
}

/* Op2_AwardScoreToPlayers - a2 op: adds the object's points to the score of every player in
 * play (bit 0 of the record's flags +$25), then runs its spawn action.
 * In:  vm->points; g_P1Flags / g_P2Flags
 * Out: d0 = the points (long), a0 = g_Player1; the scores; then Op2_RunSpawnAction
 * Evidence: move.w $36(a4),d0 / ext.l / lea g_Player1,a0 / btst #0,$25(a0) / add.l d0,(a0) /
 *           btst #0,$65(a0) / add.l d0,$40(a0) / bra.s Op2_RunSpawnAction; executed in the TAS
 */
void Op2_AwardScoreToPlayers(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5,
                             uint32_t *d6, uint32_t *d7, uint32_t *a0, uint32_t *a1, uint32_t *a2, uint32_t *a3,
                             Vm2Block **vm, uint32_t *a5, uint32_t *a6)
{
    *d0 = (uint32_t)(int32_t)(*vm)->points;
    *a0 = dk::addr(g_Player1);
    if (g_P1Flags & 1)
        ((PlayerRec *)g_Player1)->score += *d0;
    if (g_P2Flags & 1)
        ((PlayerRec *)g_Player2)->score += *d0;
    dk::jump(at_Op2_RunSpawnAction);
}

/* Op2_SetSpawnAction - a2 op: sets the object's spawn action (a Tbl_012EF0 index, run by
 * Op2_RunSpawnAction) and clears its argument. An object that is already a spawn-group member
 * (action 1) keeps it, and the op then does not skip its operand: NEXT reads the operand word as
 * the next handler (the original's code, kept).
 * In:  script = word type; self->spawn_action
 * Out: type = the operand; self->spawn_action / spawn_arg = 0; NEXT (VM2_Next for a member)
 * Evidence: cmpi.b #1,$19(a6) / beq.w VM2_Next / move.w (a2)+,d0 / move.b d0,$19(a6) /
 *           clr.w $1A(a6); Obj_Destroy treats action 1 as a group member; executed in the TAS
 */
void Op2_SetSpawnAction(uint16_t *type, uint32_t *handler, const u16 **script, Vm2Obj *self)
{
    if (self->spawn_action == 1) {
        dk::jump(at_VM2_Next);
        return;
    }
    *type = *(*script)++;
    self->spawn_action = (uint8_t)*type;
    self->spawn_arg = 0;
    next(handler, script);
}
