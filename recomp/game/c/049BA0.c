#include "game.h"

/* The two halves of the player-select screen ($049BA0-$049C3B): plane A's top half is Batman,
   scrolled by the chooser's x; the bottom half is Robin, scrolled by its y word. Each native
   slides its half off screen when the other hero is chosen and back in when its own is. */

enum {
    BATMAN_OFF = 0x118,    /* x at or past this: the top half is off screen to the right */
    BATMAN_START = -255,   /* x the top half restarts from, left of the screen ($FF01) */
    ROBIN_OFF = -280,      /* y at or below this: the bottom half is off screen ($FEE8) */
    ROBIN_START = 0xFF,    /* y the bottom half restarts from */
};

/* PlayerSelect_SlideBatmanHalf - player-select native for the top (Batman) half, position = the
 * chooser's x: when Robin is chosen it cruises right until x >= $118 (off screen); when Batman is
 * chosen and the half is off screen it restarts at x -255 at cruise speed, braking to rest at 0.
 * In:  motion = the chooser's motion block, self = the chooser
 * Out: motion->vel_x / target_x, self->x
 * Evidence: tst.l $24(a3) / cmpi.w #$118,$3E(a6) blt / clr.l $18 / clr.l $24 / tst.b g_P1IsRobin /
 *           move.l $30(a3),$24(a3), or move.w #$FF01,$3E(a6) / move.l $30(a3),$18(a3) / clr.l
 *           $24(a3); Op2_CallNativeRel from $49B46 every frame; TAS f431-f476
 */
void PlayerSelect_SlideBatmanHalf(Vm2Motion *motion, Vm2Obj *self)
{
    if (motion->target_x != 0) {   /* sliding out: stop once off screen */
        if (self->x < BATMAN_OFF)
            return;
        motion->vel_x = 0;
        motion->target_x = 0;
    }
    if (g_P1IsRobin) {             /* Robin chosen: slide out */
        if (self->x >= BATMAN_OFF)
            return;
        motion->target_x = motion->cruise_x;
        return;
    }
    /* Batman chosen: from off screen (either side), restart sliding in */
    if (self->x > BATMAN_START && self->x < BATMAN_OFF)
        return;
    self->x = BATMAN_START;
    motion->vel_x = motion->cruise_x;
    motion->target_x = 0;
}

/* PlayerSelect_SlideRobinHalf - player-select native for the bottom (Robin) half, position = the
 * chooser's y word: when Batman is chosen it cruises left until y <= -280 (off screen); when Robin
 * is chosen and the half is off screen it restarts at y $FF at minus the cruise speed, braking to
 * rest at 0.
 * In:  motion = the chooser's motion block, self = the chooser
 * Out: motion->vel_y / target_y, self->y
 * Evidence: tst.l $2C(a3) / cmpi.w #$FEE8,$42(a6) bgt / clr.l $1C / clr.l $2C / tst.b g_P1IsRobin
 *           bne / move.l $34(a3),$2C(a3) / neg.l $2C(a3), or move.w #$FF,$42(a6) / $34 -> $1C neg /
 *           clr.l $2C; Op2_CallNativeRel from $49B4A every frame; runtime entry in the TAS
 */
void PlayerSelect_SlideRobinHalf(Vm2Motion *motion, Vm2Obj *self)
{
    if (motion->target_y != 0) {   /* sliding out: stop once off screen */
        if (self->y > ROBIN_OFF)
            return;
        motion->vel_y = 0;
        motion->target_y = 0;
    }
    if (!g_P1IsRobin) {            /* Batman chosen: slide out */
        if (self->y <= ROBIN_OFF)
            return;
        motion->target_y = -(int32_t)motion->cruise_y;
        return;
    }
    /* Robin chosen: only from off screen, restart sliding in */
    if (self->y > ROBIN_OFF)
        return;
    self->y = ROBIN_START;
    motion->vel_y = -(int32_t)motion->cruise_y;
    motion->target_y = 0;
}
