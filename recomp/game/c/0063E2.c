#include "game.h"

/* $0063E2-$00645F: a random bounce, and the animation, frame and depth set-up ops of the a4
 * object (self = a6) and of the last spawned child (a5). The ticker word at +$1C is the
 * animation accumulator byte and its rate byte: Op4_Draw adds the rate each frame and steps the
 * animation script on the carry. */

static void next(uint32_t *handler, const u16 **script)
{
    *handler = dk::code((int16_t)(uint16_t)*(*script)++);
    dk::jump(*handler);
}

/* the ticker word: accumulator (high byte) and rate (low byte) */
static void set_ticker(Vm4Obj *obj, uint16_t w)
{
    obj->anim_tick = (uint8_t)(w >> 8);
    obj->anim_rate = (uint8_t)w;
}

/* the long operand (an anim-script address) */
static uint32_t read_long(const u16 **script)
{
    uint32_t v = (uint32_t)(uint16_t)(*script)[0] << 16 | (uint16_t)(*script)[1];
    *script += 2;
    return v;
}

/* Op4_BounceYRandom - a4 op: bounces the object: y velocity = -(v/2 + a random [0, 1/4) of v/2's
 * whole part): a reversed, roughly halved, slightly randomised rebound.
 * In:  script (no operands); self = a6
 * Out: d1 = v/2 (asr.l); d0 = the new y velocity; self->vel_y; NEXT
 * Evidence: bsr Rand_Next / andi.w #$3FFF,d0 / move.l $42(a6),d1 / asr.l #1,d1 / swap d1 /
 *           mulu.w d1,d0 / swap d1 / add.l d1,d0 / neg.l d0 / move.l d0,$42(a6); script uses $1A9BA
 *           (`63E2 53D4` after Op4_TerrainMoveDefault), $358B8; 6 corpus runs
 */
void Op4_BounceYRandom(uint32_t *d0, uint32_t *d1, const u16 **script, Vm4Obj *self)
{
    uint16_t r = (uint16_t)((uint32_t)Rand_Next() & 0x3FFF);
    *d1 = (uint32_t)((int32_t)self->vel_y >> 1);
    /* mulu.w with d1 swapped: the half's whole (high) word, unsigned, times r */
    *d0 = (uint32_t)r * (uint16_t)(*d1 >> 16);
    *d0 += *d1;
    *d0 = 0u - *d0;
    self->vel_y = (int32_t)*d0;
    uint32_t handler;
    next(&handler, script);
}

/* Op4_SetAnimRateRandom - a4 op: sets the animation ticker's accumulator and rate both to base +
 * a random number below range (a randomised animation speed).
 * In:  script = byte range, byte base; self = a6
 * Out: d0 = the value (low byte; Rand_Below's upper bytes); self's ticker; NEXT
 * Evidence: moveq #0,d0 / move.b (a4)+,d0 / bsr Rand_Below / add.b (a4)+,d0 / move.b d0,$1C(a6) /
 *           move.b d0,$1D(a6); script use $306CE (`6402 4040`); 13 corpus runs
 */
void Op4_SetAnimRateRandom(uint32_t *d0, const u16 **script, Vm4Obj *self)
{
    const u8 *bytes = (const u8 *)*script;   /* the two operand bytes */
    *d0 = bytes[0];
    Rand_Below(d0);
    *d0 = (*d0 & 0xFFFFFF00u) | (uint8_t)(*d0 + bytes[1]);
    *script += 1;
    self->anim_tick = (uint8_t)*d0;
    self->anim_rate = (uint8_t)*d0;
    uint32_t handler;
    next(&handler, script);
}

/* Op4_SetAnim - a4 op: starts an animation: the ticker word, the anim-script pointer, and the
 * resume pc set to the next op so later frames restart there.
 * In:  script = word ticker ($FF40 = step now, then every 4th frame), long anim-script address;
 *      self = a6
 * Out: self's ticker / anim / resume; NEXT
 * Evidence: move.w (a4)+,$1C(a6) / move.l (a4)+,$C(a6) / move.l a4,$8(a6); 161 script uses, e.g.
 *           $108D8 (`6418 FF40 0001 08E4` then `634E 67EE`); 151 corpus runs
 */
void Op4_SetAnim(const u16 **script, Vm4Obj *self)
{
    set_ticker(self, *(*script)++);
    self->anim.v = read_long(script);
    self->resume = *script;
    uint32_t handler;
    next(&handler, script);
}

/* Op4_SetFrame - a4 op: shows one fixed sprite frame: stops the animation ticker, sets the wanted
 * frame and the resume pc to the next op.
 * In:  script = word frame (a byte offset into the frame table); self = a6
 * Out: self's ticker = 0, frame, resume; NEXT
 * Evidence: clr.w $1C(a6) / move.w (a4)+,$1E(a6) / move.l a4,$8(a6); script uses $1A5A6
 *           (`6428 3232`), $1B5F4; 43 corpus runs
 */
void Op4_SetFrame(const u16 **script, Vm4Obj *self)
{
    set_ticker(self, 0);
    self->frame = *(*script)++;
    self->resume = *script;
    uint32_t handler;
    next(&handler, script);
}

/* Op4_ChildSetAnim - a4 op: starts an animation on the last spawned child: its ticker word and
 * anim-script pointer.
 * In:  script = word ticker, long anim-script address; child = a5
 * Out: the child's ticker / anim; the move.l's condition codes; NEXT
 * Evidence: move.w (a4)+,$1C(a5) / move.l (a4)+,$C(a5) / NEXT; script uses $1E270
 *           (`6438 FF40 0001 E3FE`); 47 corpus runs
 */
void Op4_ChildSetAnim(uint32_t *handler, const u16 **script, Vm4Obj *child, dk::Flags *ccr)
{
    set_ticker(child, *(*script)++);
    uint32_t anim = read_long(script);
    child->anim.v = anim;
    *ccr = dk::Flags::test(anim, 4);
    next(handler, script);
}

/* Op4_ChildSetFrame - a4 op: gives the last spawned child one fixed frame: clears its ticker and
 * sets its wanted frame.
 * In:  script = word frame; child = a5
 * Out: the child's ticker = 0, frame; the move.w's condition codes; NEXT
 * Evidence: clr.w $1C(a5) / move.w (a4)+,$1E(a5) / NEXT; script uses $1ED76 (`6444 FD62` then
 *           Op4_Spawn); 14 corpus runs
 */
void Op4_ChildSetFrame(uint32_t *handler, const u16 **script, Vm4Obj *child, dk::Flags *ccr)
{
    set_ticker(child, 0);
    uint16_t frame = *(*script)++;
    child->frame = frame;
    *ccr = dk::Flags::test(frame, 2);
    next(handler, script);
}

/* Op4_SetDepth - a4 op: sets the object's display depth (0 = in front of all sprites, 1 = behind
 * all, else depth-sorted with that key).
 * In:  script = word depth; self = a6
 * Out: self->depth; NEXT
 * Evidence: move.w (a4)+,$24(a6) / NEXT; the draw tail picks Spr_AddFront / AddBack / AddSorted by
 *           it; script uses $1465A (`6450 0001`), $1466A (`6450 0000`); 42 corpus runs
 */
void Op4_SetDepth(uint32_t *handler, const u16 **script, Vm4Obj *self)
{
    self->depth = *(*script)++;
    next(handler, script);
}

/* Op4_ChildSetDepth - a4 op: sets the last spawned child's display depth.
 * In:  script = word depth; child = a5
 * Out: child->depth; NEXT
 * Evidence: move.w (a4)+,$24(a5) / NEXT; script uses $4718C, $471AA (`6458 0002`); 2 corpus runs
 */
void Op4_ChildSetDepth(const u16 **script, Vm4Obj *child)
{
    child->depth = *(*script)++;
    uint32_t handler;
    next(&handler, script);
}
