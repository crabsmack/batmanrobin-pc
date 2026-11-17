#include "game.h"

/* Tbl_059B22: the sine table, a signed word per even heading (index & $3FE as a byte offset);
   cosine a quarter turn on (+$100 bytes) */
static constexpr uint32_t Tbl_059B22 = 0x059B22u;
/* the ladder segment sprites: (frame, attributes) word pairs, 20 bands of the segment's angle
   (ROM $026A46, labelled Scr4_026A46: the movem.w's pc-relative base; the second half H-flipped) */
static constexpr uint32_t Tbl_LadderSegFrames = 0x026A46u;

static constexpr int32_t LADDER_PULL = 0x180;            /* angular pull back toward angle 0 per frame */
static constexpr uint32_t LADDER_ANGLE_BASE = 0x1008000;  /* hangs straight down ($100 = +y) + half a step */
static constexpr int LADDER_SEGMENTS = 10;
static constexpr int LADDER_HANG_SEGMENT = 5;            /* the thug hangs from the 5th segment (dbra count 4) */
static constexpr int32_t LADDER_CLIP_Y = (int32_t)0xFFF00000;   /* segments above y -16 aren't drawn */
static constexpr uint16_t LADDER_DEPTH = 0x100;
static constexpr int16_t LADDER_HANG_DX = -13;           /* the thug's drawn point off the segment */
static constexpr int16_t LADDER_HANG_DY = 0x1C;

static inline uint32_t swap32(uint32_t v) { return (v << 16) | (v >> 16); }
static inline uint32_t set_w(uint32_t reg, uint16_t w) { return (reg & 0xFFFF0000u) | w; }

static int16_t sine_at(uint16_t byte_index, bool cosine)
{
    PTR(const s16) e;
    e.v = Tbl_059B22 + byte_index + (cosine ? 0x100u : 0u);
    return *e;
}

/* Enemy_LadderThug_Throw - ladder-thug native: runs the a4 throw script kept in its coroutine slot
 * 3 at its drawn point plus the two inline words (a random one of 3-6 bombs arcing to fixed floor
 * points: Arc_PlanToArgs)
 * In:  script = words dx, dy (a2); vm = its VM block (a4: coroutine 3 = the throw script); self
 *      (a6: the drawn point)
 * Out: d2/d3 = 0; d4/d5 = the throw point as 16.16 (integer in the high word); a0 = the throw
 *      script; script past the words; then as Fx_RunScript
 * Evidence: moveq #0,d2-d5 / move.w $4E(a6),d4 / move.w $50(a6),d5 / add.w (a2)+,d4 / add.w
 *           (a2)+,d5 / swap d4 / swap d5 / movea.l $44(a4),a0 / jmp Fx_RunScript; op $2CB2 at
 *           $026800 (dx -$C, dy -5) and $0268E6 in the throw animations; TAS f25249-f33867 (7). The
 *           prototype gains &a0 (measured a clobber) so the jump hands the script on
 */
void Enemy_LadderThug_Throw(uint32_t d0, uint32_t d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t d6,
                            uint32_t d7, uint32_t *a0, uint32_t a1, const s16 **script, uint32_t a3,
                            const Vm2Block *vm, uint32_t a5, const Vm2Obj *self)
{
    *d2 = 0;
    *d3 = 0;
    uint16_t x = (uint16_t)((uint16_t)self->draw_x + (uint16_t)*(*script)++);
    uint16_t y = (uint16_t)((uint16_t)self->draw_y + (uint16_t)*(*script)++);
    *d4 = (uint32_t)x << 16;
    *d5 = (uint32_t)y << 16;
    *a0 = vm->coroutine[3].v;
    dk::jump(at_Fx_RunScript);
}

/* Enemy_LadderThug_ReleaseLadder - ladder-thug native before Op2_Kill: releases the art of the 10
 * ladder-segment sprite frames kept in its swing block (+$8..)
 * In:  self = the ladder thug (a6: +$10 its swing block)
 * Out: d2.w = the last frame handle; d7.w = $FFFF (dbra); a1 = past the 10 handles; d0 as
 *      Spr_ReleaseFrame left it
 * Evidence: movea.w $10(a6),a1 / move.w #9,d7 / lea $8(a1),a1 / move.w (a1)+,d2 / jsr
 *           Spr_ReleaseFrame / dbra; op $2CB2 at $02669A, then Op2_Kill
 */
void Enemy_LadderThug_ReleaseLadder(uint16_t *d0, uint16_t *d2, uint16_t *d7, uint32_t *a1, const LadderThugObj *self)
{
    *a1 = (uint32_t)(int32_t)self->ladder + 8;   /* movea.w: sign-extended; the frames at +$8 */
    uint32_t a0 = 0;
    *d7 = LADDER_SEGMENTS - 1;
    do {
        PTR(const u16) frame;
        frame.v = *a1;
        *d2 = *frame;
        *a1 += 2;
        Spr_ReleaseFrame(d0, d2, &a0);
    } while ((*d7)-- != 0);                      /* dbra */
}

/* Enemy_LadderThug_SwingLadder - ladder-thug native every frame: swings the rope ladder as a
 * pendulum (the swing block's angle and speed, pulled back toward 0 by $180 a frame) and draws its
 * 10 segments down the curving chain, each one's frame by its angle (Tbl_LadderSegFrames); the 5th
 * segment sets the drawn point where the thug hangs, and doubles the bend of the segments below it
 * In:  self = the ladder thug (a6: +$10 its swing block, x/y the ladder's top); a2-a5 saved
 * Out: the swing block's angle/speed and segment frames; self draw_x/draw_y; d0-d7/a1 as the last
 *      segment left them (d7.w = $FFFF); a2-a6 kept (on the stack)
 * Evidence: movem.l a2-a6,-(a7) / movea.w $10(a6),a1 / move.l #$180,d0 / bmi / neg.l / the
 *           segment loop (dbra 9) with divs.w #$D, movem.w $26A46(pc,d1.w),d1/d6, Spr_ChangeFrame +
 *           Spr_AddSorted under movem.l d2-d5/d7, the d7 = 4 drawn point (-13, +$1C) and add.l
 *           d3,d3; op $2CB2 at $02663A and $02668E in both ladder loops; png f025250. A segment
 *           above y -16 is skipped without stepping a1, so the frames below it use the next slots
 *           (the original's): the prototype gains a2-a5 (pushed by the movem; a2 Spr_AddSorted's)
 */
void Enemy_LadderThug_SwingLadder(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5,
                                  uint32_t *d6, uint32_t *d7, uint32_t *a1, uint32_t a2, uint32_t a3, uint32_t a4,
                                  uint32_t a5, Vm2Obj *self)
{
    /* movem.l a2-a6,-(a7): a2 lowest */
    dk::push_long(dk::addr(self));
    dk::push_long(a5);
    dk::push_long(a4);
    dk::push_long(a3);
    dk::push_long(a2);
    const LadderThugObj *thug = (const LadderThugObj *)self;
    LadderSwing *swing = dk::handle<LadderSwing>(thug->ladder);

    /* the pendulum: pulled toward angle 0 */
    *d0 = (uint32_t)LADDER_PULL;
    *d1 = (uint32_t)(int32_t)swing->speed;
    *d2 = (uint32_t)(int32_t)swing->angle;
    if ((int32_t)*d2 >= 0)
        *d0 = (uint32_t)-(int32_t)*d0;
    *d1 += *d0;
    swing->speed = (int32_t)*d1;
    *d2 += *d1;
    swing->angle = (int32_t)*d2;
    *d3 = *d2;                                   /* the bend: each segment turns by the angle */
    *d2 = *d2 * 4 + LADDER_ANGLE_BASE;           /* the first segment's heading, 16.16 */

    /* the start: the ladder's top plus (cos, sin) << 7 of the first heading */
    *d0 = swap32(*d2);
    *d0 = set_w(*d0, (uint16_t)(*d0 & 0x3FE));
    uint32_t a0 = Tbl_059B22 + (uint16_t)*d0;
    *d4 = ((uint32_t)(int32_t)sine_at((uint16_t)*d0, true) << 7);
    *d5 = ((uint32_t)(int32_t)sine_at((uint16_t)*d0, false) << 7);
    *d4 += ((uint32_t)(uint16_t)self->x << 16) | self->x_frac;
    *d5 += ((uint32_t)(uint16_t)self->y << 16) | self->y_frac;

    *d7 = set_w(*d7, LADDER_SEGMENTS - 1);
    *a1 = (uint32_t)(int32_t)thug->ladder + 8;   /* the segment frame handles */
    do {
        /* one segment down the chain: (cos, sin) << 5 of its heading */
        *d0 = swap32(*d2);
        *d0 = set_w(*d0, (uint16_t)(*d0 & 0x3FE));
        *d1 = set_w(*d1, (uint16_t)*d0);
        a0 = Tbl_059B22 + (uint16_t)*d0;
        *d0 = ((uint32_t)(int32_t)sine_at((uint16_t)*d1, true) << 5);
        *d4 += *d0;
        *d0 = ((uint32_t)(int32_t)sine_at((uint16_t)*d1, false) << 5);
        *d5 += *d0;

        /* the segment's frame band: (heading - $80) / 13, clamped to 0..$13 */
        int32_t num = (int32_t)(int16_t)(uint16_t)((uint16_t)*d1 - 0x80);   /* subi.w / ext.l */
        int16_t quot = (int16_t)(num / 13);
        int16_t rem = (int16_t)(num % 13);
        *d1 = ((uint32_t)(uint16_t)rem << 16) | (uint16_t)quot;               /* divs.w */
        if (quot < 0)
            *d1 = 0;                                                         /* moveq */
        if ((int16_t)(uint16_t)*d1 > 0x13)
            *d1 = set_w(*d1, 0x13);
        *d1 = set_w(*d1, (uint16_t)((uint16_t)*d1 * 4));
        PTR(const s16) pair;
        pair.v = Tbl_LadderSegFrames + (uint32_t)(int32_t)(int16_t)(uint16_t)*d1;
        const s16 *frame_attr = pair;
        *d1 = (uint32_t)(int32_t)frame_attr[0];  /* movem.w: sign-extended */
        *d6 = (uint32_t)(int32_t)frame_attr[1];

        if ((int32_t)*d5 >= LADDER_CLIP_Y) {
            /* movem.l d2-d5/d7,-(a7): d2 lowest; the draw calls run with them on the stack */
            const uint32_t s2 = *d2, s3 = *d3, s4 = *d4, s5 = *d5, s7 = *d7;
            dk::push_long(s7);
            dk::push_long(s5);
            dk::push_long(s4);
            dk::push_long(s3);
            dk::push_long(s2);
            PTR(u16) slot;
            slot.v = *a1;
            *d2 = set_w(*d2, *slot);              /* the segment's current frame */
            Spr_ChangeFrame(d0, d1, d2, d3, &a0, a1);
            slot.v = *a1;
            *slot = (u16)*d1;                    /* move.w d1,(a1)+ */
            *a1 += 2;
            uint16_t w0 = (uint16_t)*d0;
            dk::Flags sorted_ccr = dk::Flags::test((s5 << 16) | (s5 >> 16), 4);   /* the swap d5's */
            Spr_AddSorted(&w0, d1, d2, (uint16_t)(s4 >> 16), (uint16_t)(s5 >> 16), (uint16_t)*d6, LADDER_DEPTH,
                          a0, *a1, a2, &sorted_ccr);   /* swap d4 / swap d5: the integer point */
            *d0 = set_w(*d0, w0);
            /* movem.l (a7)+,d2-d5/d7 */
            *d2 = dk::pop_long();
            *d3 = dk::pop_long();
            *d4 = dk::pop_long();
            *d5 = dk::pop_long();
            *d7 = dk::pop_long();
        }
        if ((uint16_t)*d7 == LADDER_SEGMENTS - LADDER_HANG_SEGMENT - 1) {
            /* the thug's drawn point: a half step on from this segment, turned back $80 */
            *d0 = swap32(*d2);
            *d0 = set_w(*d0, (uint16_t)(((uint16_t)*d0 - 0x80) & 0x3FE));
            uint16_t at = (uint16_t)*d0;
            a0 = Tbl_059B22 + at;
            *d0 = swap32(((uint32_t)(int32_t)sine_at(at, true) << 4) + *d4);
            *d0 = set_w(*d0, (uint16_t)((uint16_t)*d0 + (uint16_t)LADDER_HANG_DX));
            self->draw_x = (s16)(uint16_t)*d0;
            *d0 = swap32(((uint32_t)(int32_t)sine_at(at, false) << 4) + *d5);
            *d0 = set_w(*d0, (uint16_t)((uint16_t)*d0 + LADDER_HANG_DY));
            self->draw_y = (s16)(uint16_t)*d0;
            *d3 += *d3;                          /* the segments below bend twice as much */
        }
        *d2 += *d3;
        *d7 = set_w(*d7, (uint16_t)((uint16_t)*d7 - 1));   /* dbra */
    } while ((uint16_t)*d7 != 0xFFFF);
    (void)a0;
    /* movem.l (a7)+,a2-a6: the registers are the entry's (the stub restores them) */
    for (int i = 0; i < 5; i++)
        dk::pop_long();
}
