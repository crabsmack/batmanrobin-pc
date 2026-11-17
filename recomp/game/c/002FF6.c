#include "game.h"

/* $002FF6-$0031C5: Op2_HoldRangeOfPlayerX and its turn-branch variant: an enemy keeps a
 * horizontal distance [near, far] (+ its jitter) from the targeted player, in screen space, on
 * the side it is on. A window pushed past the screen edge is clamped, and when the clamp leaves
 * the object within `margin` of the player's far side it walks round to the other side. A
 * stopped object widens its window by 6 on each end, so it doesn't start walking at once
 * (hysteresis). */

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

/* a .w operation on a data register: the upper word kept */
static int16_t lo(uint32_t r) { return (int16_t)(uint16_t)r; }
static void set_lo(uint32_t *r, int v) { *r = (*r & 0xFFFF0000u) | (uint16_t)v; }

/* the H flip bit (byte +$38 bit 3 = attr bit 11): set = facing left */
static const uint16_t FACE_LEFT = 0x0800;

enum HoldMove { HOLD_IN_RANGE, HOLD_WALK_RIGHT, HOLD_WALK_LEFT };

/* The window test shared by both ops ($3026 / $30F0, the same code twice). Reads the words near,
 * far, margin (movem.w: each sign-extended into d2-d4), puts the player and the object in screen
 * space (d5, d6), and says which way the object must go. `*on_right` = the object is at or right
 * of the player. */
static HoldMove hold_window(uint32_t *d2, uint32_t *d3, uint32_t *d4, uint16_t *d5, uint16_t *d6, const u16 **script,
                            const Vm2Motion *mot, const Vm2Obj *self, bool *on_right)
{
    *d2 = (uint32_t)(int32_t)(int16_t)(uint16_t)*(*script)++;   /* near */
    *d3 = (uint32_t)(int32_t)(int16_t)(uint16_t)*(*script)++;   /* far */
    *d4 = (uint32_t)(int32_t)(int16_t)(uint16_t)*(*script)++;   /* margin */
    set_lo(d2, lo(*d2) + self->jitter);
    set_lo(d3, lo(*d3) + self->jitter);
    *d6 = (uint16_t)self->x;
    uint16_t camera = (uint16_t)(g_CameraX >> 16);
    *d5 = (uint16_t)(*d5 - camera);
    *d6 = (uint16_t)(*d6 - camera);
    int16_t player = (int16_t)*d5;
    int16_t me = (int16_t)*d6;
    bool stopped = ((uint32_t)mot->target_x >> 16) == 0;   /* tst.w $24(a3): the high word */

    /* native: the screen's edges widened to the wide view (the gameplay mode, R20: the players'
       area is the wide view, so the windows near its edges keep the 4:3 run's relation to them) */
    int16_t right_far = (int16_t)(0x136 + WS_AREA_R()), right_near = (int16_t)(0x12C + WS_AREA_R());
    int16_t left_far = (int16_t)(0xA - WS_AREA_L()), left_near = (int16_t)(0x14 - WS_AREA_L());

    *on_right = player <= me;
    if (*on_right) {
        /* the window right of the player: [player + near, player + far] */
        set_lo(d2, lo(*d2) + player);
        set_lo(d3, lo(*d3) + player);
        if (lo(*d3) > right_far) {
            set_lo(d3, right_far);
            if (lo(*d2) > right_near) {
                set_lo(d2, right_near);
                /* add.w d5,d4 / sub.w d6,d4 / bgt: the clamped window leaves too little room */
                int16_t reach = (int16_t)(lo(*d4) + player);
                set_lo(d4, reach - me);
                if (reach > me)
                    return HOLD_WALK_LEFT;   /* round to the player's other side */
            }
        }
        if (stopped) {
            set_lo(d2, lo(*d2) - 6);
            set_lo(d3, lo(*d3) + 6);
        }
        if (lo(*d3) < me)
            return HOLD_WALK_LEFT;
        if (lo(*d2) > me)
            return HOLD_WALK_RIGHT;
        return HOLD_IN_RANGE;
    }
    /* the window left of the player: [player - far, player - near] */
    set_lo(d2, -lo(*d2) + player);
    set_lo(d3, -lo(*d3) + player);
    if (lo(*d3) < left_far) {
        set_lo(d3, left_far);
        if (lo(*d2) < left_near) {
            set_lo(d2, left_near);
            int16_t reach = (int16_t)(lo(*d4) + me);
            set_lo(d4, reach - player);
            if (reach > player)
                return HOLD_WALK_RIGHT;
        }
    }
    if (stopped) {
        set_lo(d3, lo(*d3) - 6);
        set_lo(d2, lo(*d2) + 6);
    }
    if (lo(*d2) < me)
        return HOLD_WALK_LEFT;
    if (lo(*d3) > me)
        return HOLD_WALK_RIGHT;
    return HOLD_IN_RANGE;
}

/* Op2_HoldRangeOfPlayerX - a2 op: walks to stay [near, far] (+ jitter) from the targeted player
 * on the side the object is on, facing the player once there (see the file comment).
 * In:  script = words near, far, margin, word offset; vm->tracked; self->x / jitter;
 *      mot->cruise_x / target_x; g_CameraX
 * Out: in range: x target speed 0, facing the player, the branch; moving: x target speed +-cruise,
 *      facing the way it walks, NEXT after the offset; no target: the operands skipped,
 *      Op2_ClearTargetSpeedX. d0 = the handle (low word), or -cruise walking left; d2-d4 the
 *      window; d5 / d6 the player / object screen x
 * Evidence: movem.w (a2)+,d2-d4 / add.w $1C(a6); positions minus g_CameraX; cmpi #$136 / #$12C,
 *           #$A / #$14 clamps; subq / addq #6 when $24(a3) = 0; 31 runs; $1B0B2
 *           `2FF6 006E 0078 0008 FF6C`
 */
void Op2_HoldRangeOfPlayerX(uint32_t *d0, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint16_t *d5, uint16_t *d6,
                            const u16 **script, Vm2Motion *mot, const Vm2Block *vm, Vm2Sprite *spr,
                            const Vm2Obj *self)
{
    set_lo(d0, vm->tracked);
    if (vm->tracked == 0) {
        *script += 4;
        dk::jump(at_Op2_ClearTargetSpeedX);
        return;
    }
    *d5 = (uint16_t)dk::handle<PlayerRec>(vm->tracked)->x;
    bool on_right;
    HoldMove move = hold_window(d2, d3, d4, d5, d6, script, mot, self, &on_right);
    uint32_t handler;
    switch (move) {
    case HOLD_IN_RANGE:
        if (on_right)
            spr->attr |= FACE_LEFT;
        else
            spr->attr &= (uint16_t)~FACE_LEFT;
        mot->target_x = 0;
        branch(&handler, script);
        return;
    case HOLD_WALK_RIGHT:
        mot->target_x = mot->cruise_x;
        spr->attr &= (uint16_t)~FACE_LEFT;
        break;
    case HOLD_WALK_LEFT:
        *d0 = (uint32_t)-(int32_t)mot->cruise_x;
        mot->target_x = (int32_t)*d0;
        spr->attr |= FACE_LEFT;
        break;
    }
    (*script)++;   /* past the offset */
    next(&handler, script);
}

/* Op2_HoldRangeOfPlayerXTurnBranch - a2 op: as Op2_HoldRangeOfPlayerX but leaves the facing alone
 * and branches on it instead: in range and facing the player: offset 0 (Op2_Goto); needing to
 * face left: offset 1; needing to face right: offset 2; walking the way it faces continues.
 * In:  script = words near, far, margin, words offset0, offset1, offset2; as
 *      Op2_HoldRangeOfPlayerX, plus spr->attr's H flip read
 * Out: a branch, or NEXT after the operands (also with no target, x braked); the registers as
 *      Op2_HoldRangeOfPlayerX; a0 = the handler
 * Evidence: the window code of $2FF6 again ($30F0) with btst #3,$38(a5) choosing Op2_Goto / $314C
 *           (skip 1) / $3196 (skip 2) / $31AC (skip 3); 5 runs incl. the TAS
 */
void Op2_HoldRangeOfPlayerXTurnBranch(uint32_t *d0, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint16_t *d5,
                                      uint16_t *d6, uint32_t *handler, const u16 **script, Vm2Motion *mot,
                                      const Vm2Block *vm, const Vm2Sprite *spr, const Vm2Obj *self)
{
    set_lo(d0, vm->tracked);
    if (vm->tracked == 0) {
        *script += 6;
        dk::jump(at_Op2_ClearTargetSpeedX);
        return;
    }
    *d5 = (uint16_t)dk::handle<PlayerRec>(vm->tracked)->x;
    bool on_right;
    HoldMove move = hold_window(d2, d3, d4, d5, d6, script, mot, self, &on_right);
    bool facing_left = (spr->attr & FACE_LEFT) != 0;
    switch (move) {
    case HOLD_IN_RANGE:
        mot->target_x = 0;
        if (on_right == facing_left) {   /* already facing the player */
            dk::jump(at_Op2_Goto);
            return;
        }
        *script += on_right ? 1 : 2;     /* turn left: offset 1; turn right: offset 2 */
        branch(handler, script);
        return;
    case HOLD_WALK_RIGHT:
        mot->target_x = mot->cruise_x;
        if (facing_left) {
            *script += 2;                /* offset 2: turn right */
            branch(handler, script);
            return;
        }
        break;
    case HOLD_WALK_LEFT:
        *d0 = (uint32_t)-(int32_t)mot->cruise_x;
        mot->target_x = (int32_t)*d0;
        if (!facing_left) {
            *script += 1;                /* offset 1: turn left */
            branch(handler, script);
            return;
        }
        break;
    }
    *script += 3;   /* past the three offsets */
    next(handler, script);
}
