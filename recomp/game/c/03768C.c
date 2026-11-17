#include "game.h"

/* The toy ring's break-up natives ($03768C-$037707): the walk round the ring that the break-up
 * loops of Scr2_03926C spawn a figure at, one per step. */

/* Tbl_059B22: the sine table, a signed word per even heading (index & $3FE as a byte offset);
   cosine a quarter turn on (+$100 bytes) */
static constexpr uint32_t Tbl_059B22 = 0x059B22u;
static const s16 *sine_entry(uint16_t byte_index)
{
    PTR(const s16) e;
    e.v = Tbl_059B22 + byte_index;
    return e;
}

static uint32_t pos_x(const Vm2Obj *o) { return (uint32_t)(uint16_t)o->x << 16 | o->x_frac; }
static uint32_t pos_y(const Vm2Obj *o) { return (uint32_t)(uint16_t)o->y << 16 | o->y_frac; }

/* ToyRing_StartBreakUp - starts breaking a toy ring into its figures: releases the figures' sprite
 * frames and loads the walk round the ring into the script arguments: g_VM2Arg0 = the first
 * angle (spin - step / 2), g_VM2Arg1 = the angle step, g_VM2Arg2 / g_VM2Arg3 = the ring's x / y
 * radii now (radius - squash / 2, radius + squash, as ToyRing_Update draws it)
 * In:  vm = the ring's VM block (a4: +$24 the figure count); self = the ring (a6: +$E its ring
 *      block)
 * Out: d0 = the x radius, d1 = the y radius, d2 = squash / 2; d7/a1 as ToyRing_ReleaseFrames left
 *      them; g_VM2Arg0-3
 * Evidence: bsr ToyRing_ReleaseFrames / movea.w $E(a6),a0 / move.l $18(a0),d0 / move.l d0,g_VM2Arg1
 *           / asr.l #1 / neg.l / add.l $14(a0) / move.l d0,g_VM2Arg0 / the radii into g_VM2Arg2/3;
 *           Op2_CallNativeRel before the ToyRing_NextFigurePos loops in Scr2_03926C; TAS 25 frames
 */
void ToyRing_StartBreakUp(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint16_t *d7, uint32_t *a1, const Vm2Block *vm,
                          const Vm2Obj *self)
{
    uint16_t d0_low = (uint16_t)*d0, d2_low = (uint16_t)*d2;
    ToyRing_ReleaseFrames(&d0_low, &d2_low, d7, a1, vm, self);
    const ToyRingBlock *ring = toy_ring_block(self);
    uint32_t step = (uint32_t)ring->step;
    g_VM2Arg1 = step;
    g_VM2Arg0 = (uint32_t)ring->spin - (uint32_t)((int32_t)step >> 1);
    *d2 = (uint32_t)(ring->squash >> 1);
    *d0 = (uint32_t)ring->radius - *d2;
    *d1 = (uint32_t)ring->radius + (uint32_t)ring->squash;
    g_VM2Arg2 = *d0;
    g_VM2Arg3 = *d1;
}

/* ToyRing_NextFigurePos - switch native of the ring break-up loop: while figures are left (the
 * count - 1), puts the anchor (the saved position) at the next figure's place on the ring, x +
 * x radius * cos and y + y radius * sin of the angle, and steps the angle
 * In:  vm = the ring's VM block (a4: +$24 the figure count); self = the ring (a6); g_VM2Arg0-3
 *      from ToyRing_StartBreakUp (their high words: angle, x / y radius)
 * Out: d0 = 0: the next position in self's saved x/y (16.16), g_VM2Arg0 += g_VM2Arg1; d0 = 1: none
 *      left (the count cleared)
 * Evidence: subq.w #1,$24(a4) / bpl / clr.w $24(a4) / moveq #1; lea Tbl_059B22 / move.w g_VM2Arg0
 *           / andi.w #$3FE / muls.w $100(a0) / add.l $3E(a6) / move.l d0,$46(a6) / the same for y
 *           with (a0) into $4A(a6); Op2_CallNativeRelSwitch at $0395FC-$039668
 */
uint32_t ToyRing_NextFigurePos(Vm2Block *vm, Vm2Obj *self)
{
    uint16_t left = (uint16_t)(vm->var[1] - 1);   /* +$24: the figure count */
    vm->var[1] = left;
    if ((int16_t)left < 0) {
        vm->var[1] = 0;
        return 1;
    }
    const s16 *e = sine_entry((uint16_t)((g_VM2Arg0 >> 16) & 0x3FE));
    int32_t rx = (int16_t)(uint16_t)(g_VM2Arg2 >> 16);
    int32_t ry = (int16_t)(uint16_t)(g_VM2Arg3 >> 16);
    self->saved_x = (int32_t)((uint32_t)(rx * (int16_t)e[0x80]) + pos_x(self));
    self->saved_y = (int32_t)((uint32_t)(ry * (int16_t)e[0]) + pos_y(self));
    g_VM2Arg0 += g_VM2Arg1;
    return 0;
}
