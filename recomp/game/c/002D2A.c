#include "game.h"

/* $002D2A-$002F35: Op2_Return, the native-call switches, and the player-target ops: an enemy's
 * VM block names the player record it targets (vm->tracked, g_Player1 or g_Player2), and the
 * targeting ops spread the enemies between the players. A record is a valid target while its
 * `targetable` byte is set; each frame every enemy targeting it adds 1 to its `attackers`, which
 * the player's handler latches into `attackers_last` and clears (NOTES "a2 script objects"). */

/* NEXT: the handler the script's next word names, jumped to */
static void next(uint32_t *handler, const u16 **script)
{
    *handler = dk::code((int16_t)(uint16_t)*(*script)++);
    dk::jump(*handler);
}

/* the branch taken: the script continues at the offset word's address plus its value
 * (adda.w (a2),a2), then NEXT */
static void branch(uint32_t *handler, const u16 **script)
{
    *script = (const u16 *)((const u8 *)*script + (int16_t)(uint16_t)**script);
    next(handler, script);
}

/* the branch not taken: past the offset word (addq.l #2,a2), then NEXT */
static void skip_offset(uint32_t *handler, const u16 **script)
{
    (*script)++;
    next(handler, script);
}

static PlayerRec *player1(void) { return (PlayerRec *)g_Player1; }
static PlayerRec *player2(void) { return (PlayerRec *)g_Player2; }

/* Op2_Return - a2 op: returns from a script subroutine or a coroutine step: pops the script that
 * Op2_Gosub* or Op2_StepCoroutineN pushed on the 68000 stack and continues there.
 * In:  (sp) = the return script
 * Out: script = popped; NEXT
 * Evidence: movea.l (a7)+,a2 / NEXT; Scr2_002D28 (the idle coroutine) is just this op; all 153 runs
 */
void Op2_Return(uint32_t *handler, const u16 **script)
{
    PTR(const u16) popped;
    popped.v = dk::pop_long();
    *script = popped;
    next(handler, script);
}

/* the switch after a native call: the routine's d0 (low word, doubled by add.w d0,d0) indexes a
 * table of word offsets in the script, each relative to its own address */
static void switch_on_d0(uint32_t *d0, uint32_t *handler, const u16 **script)
{
    *d0 = (*d0 & 0xFFFF0000u) | (uint16_t)(*d0 * 2);
    *script = (const u16 *)((const u8 *)*script + (int16_t)(uint16_t)*d0);
    branch(handler, script);
}

/* Op2_CallNativeRelSwitch - a2 op: calls native code at a relative address (the operand's address
 * plus its value), then branches through the table of relative offsets after the operand,
 * indexed by the d0 the routine returns.
 * In:  script = word routine offset, then one word offset per possible d0 (0, 1, ...); every
 *      register the routine reads
 * Out: every register as the routine leaves it, d0's low word doubled; script = table[d0]'s
 *      target; NEXT
 * Evidence: movea.l a2,a0 / adda.w (a2)+,a0 / jsr (a0) / add.w d0,d0 / adda.w d0,a2 /
 *           adda.w (a2),a2 / NEXT; 125 runs; $1E7A4 `2D30 01FE 0004 001C` calls $1E9A4 (d0 = 1
 *           while a countdown $30(a6) runs)
 */
void Op2_CallNativeRelSwitch(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5,
                             uint32_t *d6, uint32_t *d7, uint32_t *a0, uint32_t *a1, const u16 **script,
                             uint32_t *a3, uint32_t *a4, uint32_t *a5, uint32_t *a6)
{
    *a0 = dk::addr(*script) + (uint32_t)(int32_t)(int16_t)(uint16_t)**script;
    (*script)++;
    NativeFn(dk::code(*a0), d0, d1, d2, d3, d4, d5, d6, d7, a0, a1, script, a3, a4, a5, a6);
    switch_on_d0(d0, a0, script);
}

/* Op2_CallNativeSwitch - a2 op: calls a native routine at an absolute address, then branches
 * through the table of relative offsets after the operand, indexed by the returned d0.
 * In:  script = long routine, then one word offset per d0 value; every register the routine reads
 * Out: every register as the routine leaves it, d0's low word doubled; script = table[d0]'s
 *      target; NEXT
 * Evidence: movea.l (a2)+,a0 / jsr (a0) / add.w d0,d0 / adda.w d0,a2 / adda.w (a2),a2 / NEXT;
 *           151 runs; $1E94C `2D40 000093A0 FFE8 0002` (Camera_IsLocked: d0 = 0 or 1)
 */
void Op2_CallNativeSwitch(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5,
                          uint32_t *d6, uint32_t *d7, uint32_t *a0, uint32_t *a1, const u16 **script,
                          uint32_t *a3, uint32_t *a4, uint32_t *a5, uint32_t *a6)
{
    const u32 *operand = (const u32 *)*script;
    *a0 = *operand;
    *script = (const u16 *)(operand + 1);
    NativeFn(dk::code(*a0), d0, d1, d2, d3, d4, d5, d6, d7, a0, a1, script, a3, a4, a5, a6);
    switch_on_d0(d0, a0, script);
}

/* Op2_ClearPlayerTarget - a2 op: drops the object's targeted player.
 * In:  script (no operand); vm->tracked
 * Out: vm->tracked = 0; NEXT
 * Evidence: clr.w $10(a4) / NEXT; the other ops read $10(a4) as g_Player1 / g_Player2 ($F650 /
 *           $F690); 8 runs
 */
void Op2_ClearPlayerTarget(const u16 **script, Vm2Block *vm)
{
    vm->tracked = 0;
    uint32_t handler;
    next(&handler, script);
}

/* target `rec`: counted as one of its attackers in both counts (so the next pick this frame sees
 * it too), then the branch */
static void take_target(uint32_t *handler, const u16 **script, Vm2Block *vm, PlayerRec *rec)
{
    vm->tracked = dk::handle_of(rec);
    rec->attackers_last++;
    rec->attackers++;
    branch(handler, script);
}

/* the picker ($2DA2, shared by the target ops): the targetable player; both targetable: the one
 * attacked by fewer enemies last frame, a tie alternating by g_TargetTieFlip. None: no target and
 * no branch. d0's low word = player 1's attackers_last when both were compared. */
static void pick_target(uint16_t *d0, uint32_t *handler, const u16 **script, Vm2Block *vm)
{
    PlayerRec *p1 = player1();
    PlayerRec *p2 = player2();
    if (p1->targetable == 0) {
        if (p2->targetable != 0) {
            take_target(handler, script, vm, p2);
            return;
        }
        vm->tracked = 0;
        skip_offset(handler, script);
        return;
    }
    if (p2->targetable == 0) {
        take_target(handler, script, vm, p1);
        return;
    }
    *d0 = p1->attackers_last;
    if ((int16_t)*d0 < (int16_t)p2->attackers_last) {
        take_target(handler, script, vm, p1);
        return;
    }
    if ((int16_t)*d0 > (int16_t)p2->attackers_last) {
        take_target(handler, script, vm, p2);
        return;
    }
    g_TargetTieFlip = (uint8_t)~g_TargetTieFlip;
    take_target(handler, script, vm, g_TargetTieFlip == 0 ? p1 : p2);
}

/* Op2_BalancePlayerTarget - a2 op: makes sure the object targets a player, spreading the enemies
 * between the two: keeps a targetable target unless the other player is targetable and was
 * attacked by fewer enemies last frame, then switches to it; with no valid target, the picker.
 * In:  script = word offset; vm->tracked; the records' targetable / attackers_last
 * Out: a player targeted (counted): the branch; none: vm->tracked = 0, NEXT after the offset;
 *      d0's low word = the old target handle (then its attackers_last when compared); a1 = the
 *      other record when the target was valid
 * Evidence: move.w $10(a4),d0 / tst.b $3A(a0) / cmpa.l #$FFFFF650,a0 picks the other record /
 *           cmp.w $36(a1),d0 / ble / addq.w #1,$36(a1) / addq.w #1,$38(a1) / adda.w (a2),a2;
 *           122 runs; $27D54 `2D56 0002 2D22 ...`
 */
void Op2_BalancePlayerTarget(uint16_t *d0, uint32_t *a1, const u16 **script, Vm2Block *vm)
{
    uint32_t handler;
    *d0 = (uint16_t)vm->tracked;
    if (*d0 == 0) {
        pick_target(d0, &handler, script, vm);
        return;
    }
    PlayerRec *rec = dk::handle<PlayerRec>(vm->tracked);
    if (rec->targetable == 0) {
        pick_target(d0, &handler, script, vm);
        return;
    }
    PlayerRec *other = (u8 *)rec == g_Player1 ? player2() : player1();
    *a1 = dk::addr(other);
    if (other->targetable != 0) {
        *d0 = rec->attackers_last;
        if ((int16_t)*d0 > (int16_t)other->attackers_last) {
            vm->tracked = dk::handle_of(other);
            other->attackers_last++;
            other->attackers++;
            branch(&handler, script);
            return;
        }
    }
    rec->attackers++;
    branch(&handler, script);
}

/* Op2_SwapPlayerTarget - a2 op: retargets the other player when that one is targetable;
 * otherwise keeps a still-targetable current target; with no target at all, the picker.
 * In:  script = word offset; vm->tracked
 * Out: a player targeted (counted in its attackers): the branch; else vm->tracked = 0, NEXT after
 *      the offset; d0's low word = the old handle; a0 = the handler; a1 = the other record
 * Evidence: lea g_Player2,a1 / cmpi.w #$F650,d0 / beq / lea g_Player1,a1 / tst.b $3A(a1) /
 *           move.w a1,$10(a4); 45 runs
 */
void Op2_SwapPlayerTarget(uint16_t *d0, uint32_t *handler, uint32_t *a1, const u16 **script, Vm2Block *vm)
{
    *d0 = (uint16_t)vm->tracked;
    if (*d0 == 0) {
        pick_target(d0, handler, script, vm);
        return;
    }
    PlayerRec *rec = dk::handle<PlayerRec>(vm->tracked);
    PlayerRec *other = *d0 == 0xF650 ? player2() : player1();
    *a1 = dk::addr(other);
    if (other->targetable != 0) {
        vm->tracked = dk::handle_of(other);
        other->attackers++;
        branch(handler, script);
        return;
    }
    if (rec->targetable != 0) {
        rec->attackers++;
        branch(handler, script);
        return;
    }
    vm->tracked = 0;
    skip_offset(handler, script);
}

/* Op2_KeepPlayerTarget - a2 op: confirms the current player target for this frame: counts the
 * object in its attackers while it is targetable, drops it when it is not.
 * In:  script (no operand); vm->tracked
 * Out: d0's low word = the handle; a0 = the handler; NEXT (VM2_Next with no target)
 * Evidence: move.w $10(a4),d0 / beq.w VM2_Next / tst.b $3A(a0) / addq.w #1,$38(a0) or
 *           clr.w $10(a4); 85 runs
 */
void Op2_KeepPlayerTarget(uint16_t *d0, uint32_t *handler, const u16 **script, Vm2Block *vm)
{
    *d0 = (uint16_t)vm->tracked;
    if (*d0 == 0) {
        dk::jump(at_VM2_Next);
        return;
    }
    PlayerRec *rec = dk::handle<PlayerRec>(vm->tracked);
    if (rec->targetable != 0)
        rec->attackers++;
    else
        vm->tracked = 0;
    next(handler, script);
}

/* Op2_BranchNoPlayerTarget - a2 op: branches when the object has no targetable player target
 * (dropping a stale one); otherwise counts itself in the target's attackers and continues.
 * In:  script = word offset; vm->tracked
 * Out: none: the branch (Op2_Goto with no target at all); else NEXT after the offset; d0's low
 *      word = the handle; a0 = the handler
 * Evidence: move.w $10(a4),d0 / beq.w Op2_Goto / tst.b $3A(a0) / addq.w #1,$38(a0) / addq.l #2,a2,
 *           or clr.w $10(a4) / adda.w (a2),a2; 123 runs
 */
void Op2_BranchNoPlayerTarget(uint16_t *d0, uint32_t *handler, const u16 **script, Vm2Block *vm)
{
    *d0 = (uint16_t)vm->tracked;
    if (*d0 == 0) {
        dk::jump(at_Op2_Goto);
        return;
    }
    PlayerRec *rec = dk::handle<PlayerRec>(vm->tracked);
    if (rec->targetable != 0) {
        rec->attackers++;
        skip_offset(handler, script);
        return;
    }
    vm->tracked = 0;
    branch(handler, script);
}

/* Op2_KeepOrPickPlayerTarget - a2 op: keeps a still-targetable player target (counted, no
 * branch); otherwise the picker, which branches when it finds one.
 * In:  script = word offset; vm->tracked
 * Out: kept: NEXT after the offset; picked: the branch; none: vm->tracked = 0, NEXT after the
 *      offset; d0's low word = the handle (or the picker's)
 * Evidence: beq.w $2DA2 on $10(a4) = 0 and on tst.b $3A(a0) = 0, else addq.w #1,$38(a0) /
 *           addq.l #2,a2; 57 runs
 */
void Op2_KeepOrPickPlayerTarget(uint16_t *d0, const u16 **script, Vm2Block *vm)
{
    uint32_t handler;
    *d0 = (uint16_t)vm->tracked;
    if (*d0 == 0) {
        pick_target(d0, &handler, script, vm);
        return;
    }
    PlayerRec *rec = dk::handle<PlayerRec>(vm->tracked);
    if (rec->targetable == 0) {
        pick_target(d0, &handler, script, vm);
        return;
    }
    rec->attackers++;
    skip_offset(&handler, script);
}

/* the four position branches: the targeted player's coordinate against the object's, branching
 * when `taken` holds for (player - object); no target: Op2_SkipWord */
enum { PT_RIGHT, PT_LEFT, PT_BELOW, PT_ABOVE };

static void branch_on_player(uint16_t *d0, uint32_t *a0, const u16 **script, const Vm2Block *vm,
                             const Vm2Obj *self, int test)
{
    *d0 = (uint16_t)vm->tracked;
    if (*d0 == 0) {
        dk::jump(at_Op2_SkipWord);
        return;
    }
    *a0 = (uint32_t)(int32_t)vm->tracked;
    const PlayerRec *rec = dk::handle<PlayerRec>(vm->tracked);
    bool horizontal = test == PT_RIGHT || test == PT_LEFT;
    *d0 = (uint16_t)(horizontal ? rec->x : rec->y);
    int16_t mine = horizontal ? self->x : self->y;
    /* blt (right / below) or bgt (left / above) to Op2_SkipWord */
    bool skip = (test == PT_RIGHT || test == PT_BELOW) ? (int16_t)*d0 < mine : (int16_t)*d0 > mine;
    if (skip) {
        dk::jump(at_Op2_SkipWord);
        return;
    }
    branch(a0, script);
}

/* Op2_BranchPlayerRight - a2 op: branches when the targeted player's x is at or right of the
 * object's x; no target skips.
 * In:  script = word offset; vm->tracked; self->x
 * Out: the branch, else NEXT after the offset (Op2_SkipWord); d0's low word = the handle, then
 *      the player's x; a0 = the record, then the handler
 * Evidence: move.w $2E(a0),d0 / cmp.w $3E(a6),d0 / blt.w Op2_SkipWord / adda.w (a2),a2; the player
 *           handler $102D6 copies its x / y to record +$2E / +$32; 11 runs
 */
void Op2_BranchPlayerRight(uint16_t *d0, uint32_t *a0, const u16 **script, const Vm2Block *vm, const Vm2Obj *self)
{
    branch_on_player(d0, a0, script, vm, self, PT_RIGHT);
}

/* Op2_BranchPlayerLeft - a2 op: branches when the targeted player's x is at or left of the
 * object's x; no target skips.
 * In:  script = word offset; vm->tracked; self->x
 * Out: as Op2_BranchPlayerRight
 * Evidence: cmp.w $3E(a6),d0 / bgt.w Op2_SkipWord; 8 runs
 */
void Op2_BranchPlayerLeft(uint16_t *d0, uint32_t *a0, const u16 **script, const Vm2Block *vm, const Vm2Obj *self)
{
    branch_on_player(d0, a0, script, vm, self, PT_LEFT);
}

/* Op2_BranchPlayerBelow - a2 op: branches when the targeted player's y is at or below the
 * object's y; no target skips.
 * In:  script = word offset; vm->tracked; self->y
 * Out: as Op2_BranchPlayerRight with the y (record +$32)
 * Evidence: move.w $32(a0),d0 / cmp.w $42(a6),d0 / blt.w Op2_SkipWord; 7 runs (not the TAS)
 */
void Op2_BranchPlayerBelow(uint16_t *d0, uint32_t *a0, const u16 **script, const Vm2Block *vm, const Vm2Obj *self)
{
    branch_on_player(d0, a0, script, vm, self, PT_BELOW);
}

/* Op2_BranchPlayerAbove - a2 op: branches when the targeted player's y is at or above the
 * object's y; no target skips.
 * In:  script = word offset; vm->tracked; self->y
 * Out: as Op2_BranchPlayerBelow
 * Evidence: cmp.w $42(a6),d0 / bgt.w Op2_SkipWord; 7 runs
 */
void Op2_BranchPlayerAbove(uint16_t *d0, uint32_t *a0, const u16 **script, const Vm2Block *vm, const Vm2Obj *self)
{
    branch_on_player(d0, a0, script, vm, self, PT_ABOVE);
}

/* the H flip bit (byte +$38 bit 3 = attr bit 11): set = facing left */
static const uint16_t FACE_LEFT = 0x0800;

/* Op2_FaceCruiseToPlayerX - a2 op: turns toward the targeted player and cruises toward its x:
 * x target speed = +cruise facing right (player at or right of the object), -cruise facing left;
 * no target brakes x (Op2_ClearTargetSpeedX).
 * In:  script (no operand); vm->tracked; mot->cruise_x; self->x
 * Out: mot->target_x; spr->attr's H flip; d0 = the handle, then the player's x (low word), or
 *      -cruise (long) going left; NEXT
 * Evidence: move.l $30(a3),$24(a3) / bclr #3,$38(a5), or neg.l + bset; beq.w
 *           Op2_ClearTargetSpeedX; 32 runs
 */
void Op2_FaceCruiseToPlayerX(uint32_t *d0, const u16 **script, Vm2Motion *mot, const Vm2Block *vm,
                             Vm2Sprite *spr, const Vm2Obj *self)
{
    *d0 = (*d0 & 0xFFFF0000u) | (uint16_t)vm->tracked;
    if ((uint16_t)*d0 == 0) {
        dk::jump(at_Op2_ClearTargetSpeedX);
        return;
    }
    const PlayerRec *rec = dk::handle<PlayerRec>(vm->tracked);
    *d0 = (*d0 & 0xFFFF0000u) | (uint16_t)rec->x;
    if ((int16_t)(uint16_t)*d0 < self->x) {
        *d0 = (uint32_t)-(int32_t)mot->cruise_x;
        mot->target_x = (int32_t)*d0;
        spr->attr |= FACE_LEFT;
    } else {
        mot->target_x = mot->cruise_x;
        spr->attr &= (uint16_t)~FACE_LEFT;
    }
    uint32_t handler;
    next(&handler, script);
}
