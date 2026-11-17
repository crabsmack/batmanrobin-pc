#include "game.h"

enum { LIFT_PROBE_DOWN = 0x20000 };   /* 2 px (16.16): the floor probe when not moving vertically */

/* swap: the 68000's halves exchanged */
static uint32_t Lift_Swap(uint32_t v)
{
    return (v << 16) | (v >> 16);
}

/* a register's low word replaced (move.w / add.w / sub.w into a data register) */
static uint32_t Lift_SetLow(uint32_t r, uint16_t w)
{
    return (r & 0xFFFF0000u) | w;
}

/* Stage2_MoveOnScrollingMap - a2 native move step for the objects on the lift shaft's girders:
 * moves by d1/d2 through Coll_MoveOnBlockMap in map space (y + the map's scroll), probing 2 px down
 * when not moving vertically, and stores the terrain results the way the move op does
 * In:  d1/d2 = the move (16.16); mot = the motion block (terrain box A offset and masks), self =
 *      the object (16.16 position); g_MapScrollY
 * Out: self's x/y moved (the fractions from the move, the integers from the collision); mot
 *      terrain_code / wall_hit / contact = d3 / d6 / d7; a wall stops vel_x, a floor vel_y and
 *      target_y; d0-d7 as the collision leaves them (d1/d2 = the integer move, swapped)
 * Evidence: tst.l d2 / bne / addi.l #$20000,d2 / movem.l $3E(a6),d4-d5 / add.l / move.w d1,$40(a6) /
 *           move.w d2,$44(a6) / swap x4 / sub.w / add.w $40(a3) / add.w $42(a3) / add.w g_MapScrollY /
 *           movem.w $44(a3),d6-d7 / jsr Coll_MoveOnBlockMap / back out of map space / move.w to
 *           $3E/$42(a6), $50/$52/$54(a3) / clr.l $18 or $1C+$2C(a3); Op2_CallNativeRel at $024D2A
 *           (barrel thug coroutine 1); TAS f22331-f31862 (1590 frames)
 */
void Stage2_MoveOnScrollingMap(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5,
                               uint32_t *d6, uint32_t *d7, uint32_t a0, uint32_t *a1, uint32_t *a2, Vm2Motion **motion,
                               uint32_t *a4, uint32_t *a5, Vm2Obj **object)
{
    Vm2Motion *mot = *motion;
    Vm2Obj *self = *object;
    if (*d2 == 0)
        *d2 += LIFT_PROBE_DOWN;

    /* the new position (16.16): its fractions stored at once */
    *d4 = ((uint32_t)(uint16_t)self->x << 16) | self->x_frac;   /* movem.l $3E(a6),d4-d5 */
    *d5 = ((uint32_t)(uint16_t)self->y << 16) | self->y_frac;
    *d1 += *d4;
    *d2 += *d5;
    self->x_frac = (uint16_t)*d1;
    self->y_frac = (uint16_t)*d2;
    *d1 = Lift_Swap(*d1);
    *d2 = Lift_Swap(*d2);
    *d4 = Lift_Swap(*d4);
    *d5 = Lift_Swap(*d5);

    /* the integer move, and the box's position in map space */
    uint16_t map_y = (uint16_t)(g_MapScrollY >> 16);   /* the long's integer word */
    *d1 = Lift_SetLow(*d1, (uint16_t)(*d1 - *d4));
    *d2 = Lift_SetLow(*d2, (uint16_t)(*d2 - *d5));
    *d4 = Lift_SetLow(*d4, (uint16_t)(*d4 + mot->box_a_dx));
    *d5 = Lift_SetLow(*d5, (uint16_t)(*d5 + mot->box_a_dy + map_y));
    *d6 = (uint32_t)(int32_t)(int16_t)mot->box_a_wall_mask;    /* movem.w: sign-extended */
    *d7 = (uint32_t)(int32_t)(int16_t)mot->box_a_floor_mask;

    Coll_MoveOnBlockMap(d0, *d1, *d2, d3, d4, d5, d6, d7, a0, *a1, *a2, mot, *a4, *a5, dk::addr(self));

    /* back out of map space */
    map_y = (uint16_t)(g_MapScrollY >> 16);
    *d4 = Lift_SetLow(*d4, (uint16_t)(*d4 - mot->box_a_dx));
    *d5 = Lift_SetLow(*d5, (uint16_t)(*d5 - mot->box_a_dy - map_y));
    self->x = (int16_t)(uint16_t)*d4;
    self->y = (int16_t)(uint16_t)*d5;
    mot->terrain_code = (uint16_t)*d3;
    mot->wall_hit = (uint16_t)*d6;
    if ((uint16_t)*d6 != 0)
        mot->vel_x = 0;
    mot->contact = (uint16_t)*d7;
    if ((uint16_t)*d7 != 0) {
        mot->vel_y = 0;
        mot->target_y = 0;
    }
}
