#include "game.h"

/* Mot_StorePlayerWindow and Op2_HoldPlayerWindow ($0034A6-$003571): a window around the targeted
 * player, kept as four signed bytes (half the pixel distances) in the words of the motion block's
 * target point (+$14: x near / far, +$16: y near / far), and the op that holds the object in it.
 * (The inline-words twin of Op2_SetPlayerWindowFromArgs at $34A2 is raw bytes, never run.) */

/* NEXT: the handler the script's next word names, jumped to */
static void next(const u16 **script)
{
    dk::jump(dk::code((int16_t)(uint16_t)*(*script)++));
}

/* Mot_StorePlayerWindow - packs a player-relative window into the motion block: the halves of
 * x_lo / x_hi into the bytes of +$14, of y_lo / y_hi into +$16 (Op2_HoldPlayerWindow doubles them
 * back), then continues the script.
 * In:  x_lo / x_hi, y_lo / y_hi (d0-d3) = the window (pixels, signed, even)
 * Out: d0 = x_lo << 7 with x_hi / 2 in its low byte, d1 = x_hi / 2 (d2 / d3 the same for y);
 *      mot +$14 = d0, +$16 = d2; NEXT
 * Evidence: asl.w #7,d0 / asr.w #1,d1 / move.b d1,d0 (d2/d3 the same) / move.w d0,$14(a3) /
 *           move.w d2,$16(a3) / NEXT; $34BE reads the bytes back with ext.w / add.w; 51 runs
 */
void Mot_StorePlayerWindow(uint16_t *x_lo, uint16_t *x_hi, uint16_t *y_lo, uint16_t *y_hi, const u16 **script,
                           Vm2Motion *mot)
{
    *x_lo = (uint16_t)(*x_lo << 7);
    *x_hi = (uint16_t)((int16_t)*x_hi >> 1);
    *x_lo = (uint16_t)((*x_lo & 0xFF00u) | (*x_hi & 0xFFu));
    *y_lo = (uint16_t)(*y_lo << 7);
    *y_hi = (uint16_t)((int16_t)*y_hi >> 1);
    *y_lo = (uint16_t)((*y_lo & 0xFF00u) | (*y_hi & 0xFFu));
    mot->point_x = (int16_t)*x_lo;   /* the window shares the target point's words */
    mot->point_y = (int16_t)*y_lo;
    next(script);
}

/* the window's bytes doubled back: the high byte (near) or the low byte (far), ext.w / add.w */
static uint16_t window_near(int16_t packed) { return (uint16_t)((int8_t)((uint16_t)packed >> 8) * 2); }
static uint16_t window_far(int16_t packed) { return (uint16_t)((int8_t)(uint8_t)packed * 2); }

/* Op2_HoldPlayerWindow - a2 op: steers both axes to keep the object inside the window around the
 * targeted player (+ the object's jitter), first pulling it back on screen (screen x $1E-$122,
 * y $1E-$C2): each target speed becomes +cruise (below the window or the screen), -cruise
 * (beyond) or 0 (inside). No target: Op2_ClearTargetSpeeds.
 * In:  vm->tracked = the targeted player record; mot +$14/+$16 = the window; self->x/y, jitter
 * Out: mot->target_x / target_y; d0-d4 low words as the asm leaves them; NEXT
 * Evidence: move.w $10(a4),d0 / beq.w Op2_ClearTargetSpeeds / sub.w (g_CameraX).w,d2 /
 *           cmpi.w #$122,d2 / cmpi.w #$1E,d2 / move.b $14(a3),d3 / ext.w / add.w ... / move.w
 *           $1C(a6),d0 / add.w $2E(a0),d0 / cmp.w d3,d1 / blt ...; the y half the same with
 *           #$C2 and $16/$17, $32(a0); 53 runs
 */
void Op2_HoldPlayerWindow(uint16_t *d0, uint16_t *d1, uint16_t *d2, uint16_t *d3, uint16_t *d4, const u16 **script,
                          Vm2Motion *mot, const Vm2Block *vm, const Vm2Obj *self)
{
    *d0 = (uint16_t)vm->tracked;
    if (*d0 == 0) {
        dk::jump(at_Op2_ClearTargetSpeeds);
        return;
    }
    const PlayerRec *target = dk::handle<PlayerRec>(vm->tracked);

    /* x: off screen to the right, or past the window's far edge: back; left of the screen or
       short of the near edge: forward; inside: stop */
    *d1 = (uint16_t)self->x;
    *d2 = (uint16_t)(*d1 - (uint16_t)(g_CameraX >> 16));
    int32_t speed_sign;
    if ((int16_t)*d2 > 0x122)
        speed_sign = -1;
    else if ((int16_t)*d2 < 0x1E)
        speed_sign = 1;
    else {
        *d3 = window_near(mot->point_x);
        *d4 = window_far(mot->point_x);
        *d0 = (uint16_t)(self->jitter + target->x);
        *d3 = (uint16_t)(*d3 + *d0);
        *d4 = (uint16_t)(*d4 + *d0);
        if ((int16_t)*d1 < (int16_t)*d3)
            speed_sign = 1;
        else if ((int16_t)*d1 < (int16_t)*d4)
            speed_sign = 0;
        else
            speed_sign = -1;
    }
    mot->target_x = speed_sign == 0 ? 0 : speed_sign > 0 ? (int32_t)mot->cruise_x
                                                         : (int32_t)(0u - (uint32_t)(int32_t)mot->cruise_x);

    /* y: the same against $1E-$C2 and the y window, the far edge inclusive */
    *d1 = (uint16_t)self->y;
    *d2 = (uint16_t)(*d1 - (uint16_t)(g_CameraY >> 16));
    if ((int16_t)*d2 > 0xC2)
        speed_sign = -1;
    else if ((int16_t)*d2 < 0x1E)
        speed_sign = 1;
    else {
        *d3 = window_near(mot->point_y);
        *d4 = window_far(mot->point_y);
        *d0 = (uint16_t)(self->jitter + target->y);
        *d3 = (uint16_t)(*d3 + *d0);
        *d4 = (uint16_t)(*d4 + *d0);
        if ((int16_t)*d1 < (int16_t)*d3)
            speed_sign = 1;
        else if ((int16_t)*d1 > (int16_t)*d4)
            speed_sign = -1;
        else
            speed_sign = 0;
    }
    mot->target_y = speed_sign == 0 ? 0 : speed_sign > 0 ? (int32_t)mot->cruise_y
                                                         : (int32_t)(0u - (uint32_t)(int32_t)mot->cruise_y);
    next(script);
}
