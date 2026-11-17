#include "game.h"

/* the fire's two 16-colour palettes for line 1, $20 bytes apart (ROM data) */
static constexpr uint32_t Pal_FireFlicker = 0x020E1Cu;
static constexpr uint32_t FIRE_PALETTE_SIZE = 0x20;
static constexpr uint16_t FIRE_FLICKER_DELAYS = 8;     /* a random 0-7 frames between swaps */

/* Stage1_StartFireFlicker - bombing native: starts a run-list object that flickers palette line 1
 * between two 16-colour palettes at random intervals (the fire's glow, inferred)
 * In:  a6 (Obj_New replaces it)
 * Out: d0 = 0, the flicker's delay and toggle 0; d0 = -1 when the pool is full
 * Evidence: lea Stage1_FireFlickerRun,a0 / jsr Obj_New / bne / clr.l $18(a6) / moveq #0,d0; called
 *           at $020D68 in Scr2_020CF6 right after Stage1_BombingStartFire; TAS f2112
 */
uint32_t Stage1_StartFireFlicker(uint32_t a6)
{
    uint32_t d0 = 0, a0 = at_Stage1_FireFlickerRun, a1 = 0, obj = a6;
    Obj_New(&d0, &a0, &a1, &obj);
    if (d0 != 0)
        return d0;
    PTR(FireFlickerObj) at;
    at.v = obj;
    at->delay = 0;                    /* clr.l $18(a6): delay and toggle */
    at->toggle = 0;
    return 0;
}

/* Stage1_FireFlickerRun - run-list handler of the fire flicker: when its random 0-7 frame delay runs
 * out, flips the toggle and loads palette line 1 from the first or the second fire palette
 * In:  self = the flicker (a6)
 * Out: the delay counted down; at its end a new delay (d0 = Rand_Below(8)), the toggle flipped and
 *      a0 = the palette, then as Pal_LoadLine1
 * Evidence: subq.w #1,$18(a6) / bpl / move.w #8,d0 / jsr Rand_Below / move.w d0,$18(a6) / lea
 *           Pal_FireFlicker,a0 / not.w $1A(a6) / beq / lea $20(a0),a0 / jmp Pal_LoadLine1; the TAS
 *           and fuzz-01800..05400
 */
void Stage1_FireFlickerRun(uint32_t *d0, uint32_t *a0, uint32_t *a1, FireFlickerObj *self)
{
    int16_t delay = (int16_t)(self->delay - 1);
    self->delay = delay;
    if (delay >= 0)                   /* bpl */
        return;
    *d0 = (*d0 & 0xFFFF0000u) | FIRE_FLICKER_DELAYS;
    Rand_Below(d0);
    self->delay = (int16_t)*d0;
    *a0 = Pal_FireFlicker;
    self->toggle = (uint16_t)~self->toggle;
    if (self->toggle != 0)            /* not.w / beq: the first palette when it became 0 */
        *a0 += FIRE_PALETTE_SIZE;
    dk::jump(at_Pal_LoadLine1);
}
