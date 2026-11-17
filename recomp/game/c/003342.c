#include "game.h"

/* $003342-$003401: the facing ops toward the targeted player (vm->tracked): face and cruise with
 * a branch when that turned the object round, face only (with or without that branch), and the
 * y cruise. The H flip bit of the sprite attributes (byte +$38 bit 3) set = facing left. */

/* NEXT: the handler the script's next word names, jumped to */
static void next(uint32_t *handler, const u16 **script)
{
    *handler = dk::code((int16_t)(uint16_t)*(*script)++);
    dk::jump(*handler);
}

/* the branch at the script's current word (adda.w (a2),a2), then NEXT */
static void branch(uint32_t *handler, const u16 **script)
{
    *script = (const u16 *)((const u8 *)*script + (int16_t)(uint16_t)**script);
    next(handler, script);
}

static const uint16_t FACE_LEFT = 0x0800;

/* bset / bclr #3,$38(a5): the facing set, and whether it was already so (the Z the branches read) */
static bool face(Vm2Sprite *spr, bool left)
{
    bool was_left = (spr->attr & FACE_LEFT) != 0;
    if (left)
        spr->attr |= FACE_LEFT;
    else
        spr->attr &= (uint16_t)~FACE_LEFT;
    return was_left != left;   /* turned */
}

/* the targeted player's record: d0 = its handle, a0 = the record, then d0 = its x (low words) */
static const PlayerRec *target_x(uint32_t *d0, uint32_t *a0, const Vm2Block *vm)
{
    *a0 = (uint32_t)(int32_t)vm->tracked;
    const PlayerRec *rec = dk::handle<PlayerRec>(vm->tracked);
    *d0 = (*d0 & 0xFFFF0000u) | (uint16_t)rec->x;
    return rec;
}

/* Op2_FaceCruiseToPlayerXTurnBranch - a2 op: as Op2_FaceCruiseToPlayerX (face and cruise toward
 * the targeted player's x) and branches when that turned the object round.
 * In:  script = word offset; vm->tracked; mot->cruise_x; self->x; spr->attr
 * Out: mot->target_x = +-cruise, the facing; the branch when the H flip bit changed, else NEXT
 *      after the offset (Op2_SkipWord); no target: Op2_SkipWord, the speed unchanged. d0 = the
 *      handle, the player's x, or -cruise (long) going left; a0 = the record, then the handler
 * Evidence: move.l $30(a3),$24(a3) / bclr #3,$38(a5) / beq.w Op2_SkipWord, or neg.l + bset /
 *           bne.w Op2_SkipWord; 19 runs
 */
void Op2_FaceCruiseToPlayerXTurnBranch(uint32_t *d0, uint32_t *a0, const u16 **script, Vm2Motion *mot,
                                       const Vm2Block *vm, Vm2Sprite *spr, const Vm2Obj *self)
{
    *d0 = (*d0 & 0xFFFF0000u) | (uint16_t)vm->tracked;
    if (vm->tracked == 0) {
        dk::jump(at_Op2_SkipWord);
        return;
    }
    const PlayerRec *rec = target_x(d0, a0, vm);
    bool turned;
    if (rec->x < self->x) {
        *d0 = (uint32_t)-(int32_t)mot->cruise_x;
        mot->target_x = (int32_t)*d0;
        turned = face(spr, true);
    } else {
        mot->target_x = mot->cruise_x;
        turned = face(spr, false);
    }
    if (!turned) {
        dk::jump(at_Op2_SkipWord);
        return;
    }
    branch(a0, script);
}

/* Op2_FacePlayer - a2 op: turns the object toward the targeted player (facing left when the
 * player is left of it), no movement change.
 * In:  script (no operand); vm->tracked; self->x
 * Out: spr->attr's H flip; d0's low word = the handle, then the player's x; a0 = the record, then
 *      the handler; NEXT (VM2_Next with no target)
 * Evidence: move.w $2E(a0),d0 / cmp.w $3E(a6),d0 / blt / bclr or bset #3,$38(a5); 25 runs
 */
void Op2_FacePlayer(uint16_t *d0, uint32_t *a0, const u16 **script, const Vm2Block *vm, Vm2Sprite *spr,
                    const Vm2Obj *self)
{
    *d0 = (uint16_t)vm->tracked;
    if (*d0 == 0) {
        dk::jump(at_VM2_Next);
        return;
    }
    uint32_t d0l = *d0;
    const PlayerRec *rec = target_x(&d0l, a0, vm);
    *d0 = (uint16_t)d0l;
    face(spr, rec->x < self->x);
    next(a0, script);
}

/* Op2_FacePlayerTurnBranch - a2 op: turns toward the targeted player and branches when that
 * changed the facing.
 * In:  script = word offset; vm->tracked; self->x
 * Out: spr->attr's H flip; the branch when it turned, else NEXT after the offset (Op2_SkipWord,
 *      also with no target); d0 / a0 as Op2_FacePlayer
 * Evidence: bclr / bset #3,$38(a5) then beq / bne Op2_SkipWord; 19 runs
 */
void Op2_FacePlayerTurnBranch(uint16_t *d0, uint32_t *a0, const u16 **script, const Vm2Block *vm, Vm2Sprite *spr,
                              const Vm2Obj *self)
{
    *d0 = (uint16_t)vm->tracked;
    if (*d0 == 0) {
        dk::jump(at_Op2_SkipWord);
        return;
    }
    uint32_t d0l = *d0;
    const PlayerRec *rec = target_x(&d0l, a0, vm);
    *d0 = (uint16_t)d0l;
    if (!face(spr, rec->x < self->x)) {
        dk::jump(at_Op2_SkipWord);
        return;
    }
    branch(a0, script);
}

/* Op2_CruiseToPlayerY - a2 op: cruises toward the targeted player's y: y target speed = +cruise
 * when the player is at or below the object, else Op2_CruiseUp; no target brakes y
 * (Op2_ClearTargetSpeedY).
 * In:  script (no operand); vm->tracked; mot->cruise_y; self->y
 * Out: mot->target_y; d0's low word = the handle, then the player's y; a0 = the record, then the
 *      handler; NEXT (or the two tails)
 * Evidence: move.w $32(a0),d0 / cmp.w $42(a6),d0 / blt.w Op2_CruiseUp / move.l $34(a3),$2C(a3);
 *           54 runs
 */
void Op2_CruiseToPlayerY(uint16_t *d0, uint32_t *a0, const u16 **script, Vm2Motion *mot, const Vm2Block *vm,
                         const Vm2Obj *self)
{
    *d0 = (uint16_t)vm->tracked;
    if (*d0 == 0) {
        dk::jump(at_Op2_ClearTargetSpeedY);
        return;
    }
    *a0 = (uint32_t)(int32_t)vm->tracked;
    const PlayerRec *rec = dk::handle<PlayerRec>(vm->tracked);
    *d0 = (uint16_t)rec->y;
    if ((int16_t)*d0 < self->y) {
        dk::jump(at_Op2_CruiseUp);
        return;
    }
    mot->target_y = mot->cruise_y;
    next(a0, script);
}
