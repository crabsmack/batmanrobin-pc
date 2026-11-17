#include "game.h"

/* The toy figure's draw natives ($036CD4-$036DA7): anim-script natives (Op2_CallNativeRel) that
 * pick one of the figure's 60 rotation steps and draw it through the shared tail
 * ToyFigure_DrawIndex. */

/* the toy figure's rotation frames: plain, and the outlined ("charged") copy (ROM data) */
static constexpr uint32_t Tbl_036DA8 = 0x036DA8u;
static constexpr uint32_t Tbl_ToyFigureChargedFrames = 0x036E98u;

enum {
    TOY_STEPS = 60,                 /* rotation steps of a full turn (6 degrees each) */
    TOY_TUMBLE_STEPS = 30,          /* the tumble divisor maps a speed of +-bias onto the 60 steps */
    TOY_MODE_OUTLINED = 0x0C,       /* VM variable 0 bits that ask for the outlined frames */
    TOY_MODE_CHARGED = 1 << 2,      /* charged: outlined on alternate 4-frame phases */
    TOY_CHARGED_PHASE = 1 << 2,     /* the g_FrameCounter bit the charged blink follows */
    TOY_WARNING_PHASE = 1 << 3,     /* warning (bit 3 alone): outlined on random frames of 8-frame phases */
};

static void set_low_word(uint32_t *r, uint16_t v) { *r = (*r & 0xFFFF0000u) | v; }
static void set_low_byte(uint32_t *r, uint8_t v) { *r = (*r & 0xFFFFFF00u) | v; }

/* ToyFigure_DrawIndex ($036D2E, reached only by the branches of the natives above it): stores the
 * rotation step d1 in sprite variable 0, looks it up in Tbl_036DA8 (or, while the VM variable 0
 * asks for it, in the outlined copy Tbl_ToyFigureChargedFrames: bit 2 = charged, on alternate
 * 4-frame phases; bit 3 alone = a warning, on random frames of the 8-frame phases), swaps the
 * slot's frame and adds the sprite at the object's position: Spr_AddBack when the sprite's depth
 * is 0, else Spr_AddSorted. Its jmp to Spr_Add* is a call and return here: they need a0 (the
 * frame-table entry Spr_ChangeFrame leaves), which the natives' contracts list as a clobber, so a
 * dk::jump would not hand it on. */
static void draw_index(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint16_t *d4, uint16_t *d5,
                       uint16_t *d6, uint16_t *d7, uint32_t *a1, uint32_t a2, const Vm2Block *vm, Vm2Sprite *spr,
                       const Vm2Obj *self)
{
    uint16_t step = (uint16_t)*d1;
    spr->var[0] = (int16_t)step;
    *d4 = (uint16_t)self->x;
    *d5 = (uint16_t)self->y;
    uint32_t table = Tbl_036DA8;
    uint16_t mode = vm->var[0];
    set_low_word(d0, (uint16_t)(mode & TOY_MODE_OUTLINED));
    if (mode & TOY_MODE_OUTLINED) {
        set_low_word(d0, (uint16_t)(mode & TOY_MODE_CHARGED));
        if (mode & TOY_MODE_CHARGED) {
            uint16_t phase = (uint16_t)(g_FrameCounter & TOY_CHARGED_PHASE);
            set_low_word(d0, phase);
            if (phase)
                table = Tbl_ToyFigureChargedFrames;
        } else {
            uint16_t phase = (uint16_t)(g_FrameCounter & TOY_WARNING_PHASE);
            set_low_word(d0, phase);
            if (phase) {
                *d0 = (uint32_t)Rand_Next();
                set_low_word(d0, (uint16_t)(*d0 & 1u));
                if (*d0 & 1u)
                    table = Tbl_ToyFigureChargedFrames;
            }
        }
    }
    uint16_t offset = (uint16_t)(step * 4);   /* add.w d1,d1 twice */
    PTR(const ToyFrameStep) entry;
    entry.v = table + (uint32_t)(int32_t)(int16_t)offset;
    set_low_word(d1, entry->frame);
    uint8_t attr = (uint8_t)(((uint16_t)spr->attr >> 8) & entry->attr_mask);   /* move.b $38(a5): the high byte */
    set_low_byte(d0, entry->flip);
    attr ^= entry->flip;
    *d6 = (uint16_t)(attr << 8);
    uint32_t a0 = entry.v + sizeof(ToyFrameStep);
    PTR(u16) slot;
    slot.v = *a1;
    set_low_word(d2, *slot);
    Spr_ChangeFrame(d0, d1, d2, d3, &a0, a1);
    slot.v = *a1;
    *slot = (uint16_t)*d1;
    *a1 += 2;
    *d7 = spr->depth;
    dk::Flags spr_ccr{};
    uint16_t node = (uint16_t)*d0;
    if (*d7 == 0)
        Spr_AddBack(&node, d1, *d4, *d5, *d6, a0, *a1, &spr_ccr);
    else
        Spr_AddSorted(&node, d1, d2, *d4, *d5, *d6, *d7, a0, *a1, a2, &spr_ccr);
    set_low_word(d0, node);
}

/* the step + n, mod 60: the andi #$7F first, then 60 off until below (unsigned word compare) */
static void turn(uint32_t *d1, const Vm2Sprite *spr, uint16_t n)
{
    uint16_t step = (uint16_t)(((uint16_t)spr->var[0] + n) & 0x7F);
    while (step >= TOY_STEPS)
        step = (uint16_t)(step - TOY_STEPS);
    set_low_word(d1, step);
}

/* ToyFigure_DrawSpin1 - anim-script native: turns a toy figure one step further round (sprite
 * variable 0 + 1, mod 60) and draws that rotation frame
 * In:  spr = the sprite block (a5: variable 0 = the step); vm/self = the object (a4/a6); a1 = the
 *      next frame slot (Op2_AnimFrame); a2 passed to Spr_AddSorted
 * Out: d1 = the step, then ToyFigure_DrawIndex's: the frame swapped, the sprite added, a1 + 2
 * Evidence: move.w $22(a5),d1 / addq.w #1,d1 / andi.w #$7F,d1 / cmpi.w #$3C,d1 / bcs
 *           ToyFigure_DrawIndex / subi.w #$3C,d1; Op2_CallNativeRel in the figure animation script
 *           $036CBC; TAS f91205-f93246 (191 frames)
 */
void ToyFigure_DrawSpin1(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint16_t *d4, uint16_t *d5,
                         uint16_t *d6, uint16_t *d7, uint32_t *a1, uint32_t a2, const Vm2Block *vm, Vm2Sprite *spr,
                         const Vm2Obj *self)
{
    turn(d1, spr, 1);
    draw_index(d0, d1, d2, d3, d4, d5, d6, d7, a1, a2, vm, spr, self);
}

/* ToyFigure_DrawSpin3 - anim-script native: turns a toy figure three steps (18 degrees) further
 * round, mod 60, and draws that rotation frame
 * In:  as ToyFigure_DrawSpin1
 * Out: as ToyFigure_DrawSpin1
 * Evidence: move.w $22(a5),d1 / addq.w #3,d1 / andi.w #$7F,d1 / the mod-60 loop; the figure
 *           animation script $036CC8 (ToyFigure_SpawnFlyer's) and Scr2_035D74+$76; a computed-call
 *           target in the corpus
 */
void ToyFigure_DrawSpin3(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint16_t *d4, uint16_t *d5,
                         uint16_t *d6, uint16_t *d7, uint32_t *a1, uint32_t a2, const Vm2Block *vm, Vm2Sprite *spr,
                         const Vm2Obj *self)
{
    turn(d1, spr, 3);
    draw_index(d0, d1, d2, d3, d4, d5, d6, d7, a1, a2, vm, spr, self);
}

/* ToyFigure_SetTumbleDivisor - anim-script native: sets the velocity-to-frame divisor of a tumbling
 * toy figure: frame_div = frame_bias / 30, so a y speed in [-bias, +bias] maps onto the 60 steps
 * In:  spr = the sprite block (a5: +$32 the bias, a long)
 * Out: d1 = divs.w's result (remainder high, quotient low; the bias itself on an overflow);
 *      frame_div = its low word
 * Evidence: move.l $32(a5),d1 / divs.w #$1E,d1 / move.w d1,$36(a5) / rts; the figure animation
 *           script $036CAC calls it before ToyFigure_DrawByVelY
 */
uint32_t ToyFigure_SetTumbleDivisor(Vm2Sprite *spr)
{
    int32_t bias = spr->frame_bias;
    int32_t q = bias / TOY_TUMBLE_STEPS;   /* the 68000 truncates toward zero, as C does */
    int32_t r = bias % TOY_TUMBLE_STEPS;   /* and the remainder takes the dividend's sign */
    uint32_t d1 = (uint32_t)bias;
    if (q >= -0x8000 && q <= 0x7FFF)       /* else divs overflows: V set, d1 unchanged */
        d1 = ((uint32_t)(uint16_t)r << 16) | (uint16_t)q;
    spr->frame_div = (uint16_t)d1;
    return d1;
}

/* ToyFigure_DrawByVelY - anim-script native: picks a toy figure's rotation step from its vertical
 * motion, (y velocity + y accel + bias) / divisor, 0 when negative or past 59, and draws it (the
 * tumble through a jump)
 * In:  motion = the motion block (a3: +$1C, +$28); spr = the sprite block (a5: +$32 bias, +$36
 *      divisor); the rest as ToyFigure_DrawSpin1
 * Out: d1 = divu.w's result (step 0 in the low word when out of range), then ToyFigure_DrawIndex's
 * Evidence: move.l $1C(a3),d1 / add.l $28(a3),d1 / add.l $32(a5),d1 / bmi / divu.w $36(a5),d1 /
 *           cmpi.w #$3C,d1 / bcs ToyFigure_DrawIndex / move.w #0,d1; the figure animation script
 *           $036CB4 (after ToyFigure_SetTumbleDivisor) while the figure jumps
 */
void ToyFigure_DrawByVelY(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint16_t *d4, uint16_t *d5,
                          uint16_t *d6, uint16_t *d7, uint32_t *a1, uint32_t a2, const Vm2Motion *motion,
                          const Vm2Block *vm, Vm2Sprite *spr, const Vm2Obj *self)
{
    *d1 = (uint32_t)motion->vel_y + (uint32_t)motion->accel_y + (uint32_t)spr->frame_bias;
    bool in_range = false;
    if ((int32_t)*d1 >= 0) {
        uint16_t div = spr->frame_div;
        /* a zero divisor traps on the 68000 (never: SetTumbleDivisor runs first); divu overflows
           (quotient past $FFFF) leave d1 unchanged */
        if (div != 0 && *d1 / div <= 0xFFFFu)
            *d1 = ((*d1 % div) << 16) | (*d1 / div);
        in_range = (uint16_t)*d1 < TOY_STEPS;
    }
    if (!in_range)
        set_low_word(d1, 0);
    draw_index(d0, d1, d2, d3, d4, d5, d6, d7, a1, a2, vm, spr, self);
}

/* ToyFigure_DrawAngle - anim-script native: draws a toy figure at the rotation step the next
 * script word gives (0-59, 6 degrees each)
 * In:  script = the native's argument (a2: word step); the rest as ToyFigure_DrawSpin1
 * Out: script past the word; d1 = the step, then ToyFigure_DrawIndex's
 * Evidence: move.w (a2)+,d1, falling into ToyFigure_DrawIndex; the figure animation script
 *           $036C9E passes 0 (upright)
 */
void ToyFigure_DrawAngle(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint16_t *d4, uint16_t *d5,
                         uint16_t *d6, uint16_t *d7, uint32_t *a1, const u16 **script, const Vm2Block *vm,
                         Vm2Sprite *spr, const Vm2Obj *self)
{
    set_low_word(d1, *(*script)++);
    draw_index(d0, d1, d2, d3, d4, d5, d6, d7, a1, dk::addr(*script), vm, spr, self);
}
