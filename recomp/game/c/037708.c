#include "game.h"

/* The toy ring's natives ($037708-$0379BB): its set-up, the squash spring and the per-frame update
 * that rolls the hoop and draws its figures round the squashed ellipse. */

/* the toy figure's rotation frames: plain, and the outlined ("charged") copy (ROM data) */
static constexpr uint32_t Tbl_036DA8 = 0x036DA8u;
static constexpr uint32_t Tbl_ToyFigureChargedFrames = 0x036E98u;

/* Tbl_059B22: the sine table, a signed word per even heading (index & $3FE as a byte offset);
   cosine a quarter turn on (+$100 bytes) */
static constexpr uint32_t Tbl_059B22 = 0x059B22u;
static const s16 *sine_entry(uint16_t byte_index)
{
    PTR(const s16) e;
    e.v = Tbl_059B22 + byte_index;
    return e;
}

enum {
    RING_MODE_OUTLINED = 0x0C,     /* VM variable 0 bits that ask for the outlined frames */
    RING_MODE_CHARGED = 1 << 2,    /* charged: the blink pattern; also the attack box stays on */
    RING_WARNING_PHASE = 1 << 3,   /* warning (bit 3 alone): outlined on random frames of 8-frame phases */
    RING_ATTACK_OFF_RUNS = 2,      /* runs the VM block's +$01 count holds while not charged */
    RING_ON_TERRAIN = 0x0B,        /* the motion contact bits that mean "on the ground" */
    RING_AIR_BOX_DY = 0x0A,        /* airborne: terrain box offset = y radius / 2 + 10 */
    RING_FIGURE_FACING = 0x200,    /* a figure faces outward: the tangent's angle + a half turn */
};

static void set_low_word(uint32_t *r, uint16_t v) { *r = (*r & 0xFFFF0000u) | v; }
static void set_low_byte(uint32_t *r, uint8_t v) { *r = (*r & 0xFFFFFF00u) | v; }
static uint32_t swap(uint32_t v) { return v << 16 | v >> 16; }
static uint32_t muls(uint32_t d, int16_t m) { return (uint32_t)((int32_t)(int16_t)(uint16_t)d * m); }
/* rol.w #1 / ror.w #1 of a word in memory: the new word, and the bit rotated out (C) */
static bool rol_word(u16 *w) { uint16_t v = *w; *w = (uint16_t)(v << 1 | v >> 15); return (v & 0x8000u) != 0; }
static uint16_t ror_word(uint16_t v) { return (uint16_t)(v >> 1 | v << 15); }

/* ToyRing_ReleaseFrames - releases the sprite frames of all the figures of a toy ring (one word per
 * figure from +$24 of the ring block)
 * In:  vm = the ring's VM block (a4: +$24 the figure count); self = the ring (a6: +$E the ring block)
 * Out: d2 = the last frame released, d0 / a0 as Spr_ReleaseFrame left them; d7 = $FFFF (the dbra);
 *      a1 = past the frame list
 * Evidence: movea.w $E(a6),a1 / move.w $24(a4),d7 / lea $24(a1),a1 / bra dbra / move.w (a1)+,d2 /
 *           jsr Spr_ReleaseFrame / dbra d7; called by ToyRing_StartBreakUp and the death path
 *           $03762A; TAS f79748-f112915 (25 frames)
 */
void ToyRing_ReleaseFrames(uint16_t *d0, uint16_t *d2, uint16_t *d7, uint32_t *a1, const Vm2Block *vm,
                           const Vm2Obj *self)
{
    ToyRingBlock *ring = toy_ring_block(self);
    *a1 = dk::addr(ring->frame);
    *d7 = vm->var[1];   /* +$24: the figure count */
    while ((*d7)-- != 0) {
        PTR(const u16) frame;
        frame.v = *a1;
        *d2 = *frame;
        *a1 += 2;
        uint32_t a0 = 0;
        Spr_ReleaseFrame(d0, d2, &a0);
    }
}

/* the shared tail ($03772C): the ring block's squash speed = the word << 8, sign-extended */
static uint32_t set_squash_speed(const Vm2Obj *self, uint16_t speed)
{
    uint32_t d0 = (uint32_t)((int32_t)(int16_t)speed << 8);
    toy_ring_block(self)->squash_speed = (int32_t)d0;
    return d0;
}

/* ToyRing_SetSquashSpeedArg - sets a toy ring's squash speed (+$C of the ring block, word << 8)
 * from the script argument g_VM2Arg0 (its high word)
 * In:  g_VM2Arg0; self = the ring (a6)
 * Out: d0 = the speed (sign-extended, << 8)
 * Evidence: move.w g_VM2Arg0,d0 / bra.s loc_03772C (movea.w $E(a6),a0 / ext.l d0 / asl.l #8,d0 /
 *           move.l d0,$C(a0)); Op2_CallNativeRel at $037670 (the landing coroutine of Scr2_037588)
 */
uint32_t ToyRing_SetSquashSpeedArg(const Vm2Obj *self)
{
    return set_squash_speed(self, (uint16_t)(g_VM2Arg0 >> 16));
}

/* ToyRing_SetSquashSpeed - sets a toy ring's squash speed from the next script word (word << 8),
 * e.g. $FA00 on landing so the ring flattens and springs back
 * In:  script = the native's argument (a2: word speed); self = the ring (a6)
 * Out: script past the word; d0 = the speed (sign-extended, << 8)
 * Evidence: move.w (a2)+,d0, then the shared tail; Op2_CallNativeRel $03764E with word $FA00
 */
void ToyRing_SetSquashSpeed(uint32_t *d0, const u16 **script, const Vm2Obj *self)
{
    *d0 = set_squash_speed(self, *(*script)++);
}

/* ToyRing_TestRebound - switch native: whether a landed toy ring has sprung back (the squash and
 * its speed both non-negative)
 * In:  self = the ring (a6)
 * Out: d0 = 1 rebounding, 0 still compressed
 * Evidence: movea.w $E(a6),a0 / tst.w $8(a0) / bmi / tst.w $C(a0) / bmi / moveq #1 / moveq #0;
 *           Op2_CallNativeRelSwitch $037680 in the landing coroutine
 */
uint32_t ToyRing_TestRebound(const Vm2Obj *self)
{
    const ToyRingBlock *ring = toy_ring_block(self);
    if (ring->squash < 0 || ring->squash_speed < 0)   /* tst.w of the high words: their signs */
        return 0;
    return 1;
}

/* ToyRing_Init - inline-argument native that sets up a toy ring: words figure count, radius, spring
 * stiffness, blink reload, blink pattern; the angle step between the figures = $4000 / count
 * In:  script = the native's arguments (a2: 5 words); vm = the ring's VM block (a4); self = the
 *      ring (a6: +$E the ring block)
 * Out: script past the words; the ring block cleared, then radius / last y radius (integer parts)
 *      = the radius, stiffness = word << 8, step = ($4000 / count) << 12, the blink timer / reload
 *      / pattern; vm +$24 = the count; d0 = the blink reload (low word), d1 = the step, d2 = the
 *      stiffness
 * Evidence: movea.w $E(a6),a0 / jsr ObjPool_Clear / move.w (a2)+ x3 / ext.l d2 / asl.l #8,d2 /
 *           move.w d0,$24(a4) / move.w d1,(a0) / move.w d1,4(a0) / move.l d2,$10(a0) / move.l
 *           #$4000,d1 / divu.w d0,d1 / swap / clr.w / lsr.l #4 / move.l d1,$18(a0); the first op of
 *           the ring spawner scripts (Scr2_03905E: 16 figures radius $6E; Scr2_0391F4: 6 figures)
 */
void ToyRing_Init(uint32_t *d0, uint32_t *d1, uint32_t *d2, const u16 **script, Vm2Block *vm, const Vm2Obj *self)
{
    ToyRingBlock *ring = toy_ring_block(self);
    uint32_t a0 = dk::addr(ring);
    ObjPool_Clear(d0, &a0);
    uint16_t count = *(*script)++;
    uint16_t radius = *(*script)++;
    set_low_word(d0, count);
    set_low_word(d1, radius);
    *d2 = (uint32_t)((int32_t)(int16_t)*(*script)++ << 8);
    vm->var[1] = count;   /* +$24: the figure count */
    /* move.w into the longs' high words: the integer parts */
    ring->radius = (int32_t)((uint32_t)radius << 16 | ((uint32_t)ring->radius & 0xFFFFu));
    ring->last_radius_y = (int32_t)((uint32_t)radius << 16 | ((uint32_t)ring->last_radius_y & 0xFFFFu));
    ring->stiffness = (int32_t)*d2;
    /* divu.w: a zero count would trap on the 68000; $4000 / count never overflows a word */
    uint32_t step = (uint32_t)(0x4000u / count);
    *d1 = (step << 16) >> 4;   /* swap / clr.w / lsr.l #4: the quotient << 12 */
    ring->step = (int32_t)*d1;
    uint16_t reload = *(*script)++;
    set_low_word(d0, reload);
    ring->blink_timer = (int16_t)reload;
    ring->blink_reload = reload;
    ring->blink_pattern = *(*script)++;
}

/* the squash spring ($0377A0-$0377EF): the speed pulls the squash back toward 0 by the stiffness;
   past the rest point it is damped by a quarter and the spring stops when slow; a squash pushed
   beyond 7/8 of the radius loses its speed */
static void ring_spring(ToyRingBlock *ring)
{
    uint32_t speed = (uint32_t)ring->squash_speed;
    uint32_t squash = (uint32_t)ring->squash;
    bool damp;
    if ((int32_t)squash < 0) {
        speed += (uint32_t)ring->stiffness;
        squash += speed;
        damp = (int32_t)squash >= 0;
        if (!damp) {
            uint32_t r = (uint32_t)ring->radius;
            uint32_t floor = r - (uint32_t)((int32_t)r >> 3) + squash;
            if ((int32_t)floor < 0)
                speed = 0;
        }
    } else {
        speed -= (uint32_t)ring->stiffness;
        squash += speed;
        damp = (int32_t)squash < 0;
    }
    if (damp) {
        speed -= (uint32_t)((int32_t)speed >> 2);
        bool settle = (int32_t)speed < 0 ? (int32_t)speed >= (int32_t)0xFFFF0000u : (int32_t)speed <= 0x10000;
        if (settle) {
            speed = 0;
            squash = 0;
        }
    }
    ring->squash_speed = (int32_t)speed;
    ring->squash = (int32_t)squash;
}

/* ToyRing_Update - per-frame toy-ring update: runs the squash spring, rolls the ring with its x
 * speed (spin += x speed / radius), sizes its body and attack boxes to the squashed radii (the
 * attack held off unless charged), keeps its bottom on the floor while it squashes, and draws
 * every figure round the ellipse facing outward (Spr_AddFront), outlined by the blink
 * In:  self = the ring (a6: +$E the ring block); motion/vm/spr = its blocks (a3/a4/a5)
 * Out: the ring block's spring, spin, blink and last y radius; the box rectangles; the motion
 *      block's terrain box offsets (+$42/+$4A), self y; the figures' frames and sprites. d0 = x
 *      radius << 16 | y radius (integer parts), d1 = the step, d2/d3 = the last figure's offset
 *      (x radius * cos, y radius * sin), d4/d5 = self x/y (16.16), d6 = spin + count * step, d7 =
 *      the drawn angle (low word of spin - step) << 16 | $FFFF; a1 = past the frame list
 * Evidence: the spring at $0377A0-$0377EC; the blink $0377EE-$037834; divu.w (a2) of #$10000 /
 *           muls.w with $18(a3) >> 8 / add.l d6,$14(a2); movem.w d2-d5,$8(box) for 4(a4) and $C(a4)
 *           / move.b #2,1(a4) unless var 0 bit 2; the floor tracking $0378A2-$0378F4; per figure
 *           Mot_Atan2 + $200 / mulu.w #$F00 / rol.w $1E(a2) / Spr_ChangeFrame / Spr_AddFront;
 *           Op2_CallNativeRel $037618 in the ring loop Scr2_037604. The movem pairs only keep
 *           registers over the calls: the C keeps them in locals
 */
void ToyRing_Update(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5,
                    uint32_t *d6, uint32_t *d7, uint32_t *a1, Vm2Motion *motion, Vm2Block *vm, const Vm2Sprite *spr,
                    Vm2Obj *self)
{
    ToyRingBlock *ring = toy_ring_block(self);
    ring_spring(ring);

    /* the blink: this frame's mask (bit 15 first: outlined) */
    uint16_t mode = vm->var[0];
    uint32_t r0 = (uint32_t)ring->squash_speed;   /* d0 as the spring left it */
    set_low_word(&r0, (uint16_t)(mode & RING_MODE_OUTLINED));
    if (mode & RING_MODE_OUTLINED) {
        set_low_word(&r0, (uint16_t)(mode & RING_MODE_CHARGED));
        if (mode & RING_MODE_CHARGED) {
            set_low_word(&r0, ring->blink_pattern);
            ring->blink_timer = (int16_t)(ring->blink_timer - 1);
            if (ring->blink_timer < 0) {
                ring->blink_timer = (int16_t)ring->blink_reload;
                set_low_word(&r0, ror_word((uint16_t)r0));
                ring->blink_pattern = (uint16_t)r0;
            }
        } else {
            set_low_word(&r0, (uint16_t)(g_FrameCounter & RING_WARNING_PHASE));
            if ((uint16_t)r0 != 0) {
                r0 = (uint32_t)Rand_Next();
                set_low_word(&r0, (uint16_t)(r0 & 1u));
                if ((uint16_t)r0 != 0)
                    set_low_word(&r0, 0xFFFF);
            }
        }
    }
    ring->blink_mask = (uint16_t)r0;

    /* the roll: spin += (x velocity >> 8) * ($10000 / radius) */
    uint32_t inv = 0x10000u;
    uint16_t r_int = (uint16_t)((uint32_t)ring->radius >> 16);
    if (r_int != 0 && inv / r_int <= 0xFFFFu)   /* divu overflow (radius 1) leaves it; 0 would trap */
        inv = (inv % r_int) << 16 | (inv / r_int);
    uint32_t roll = muls((uint32_t)(motion->vel_x >> 8), (int16_t)(uint16_t)inv);
    ring->spin = (int32_t)((uint32_t)ring->spin + roll);
    uint32_t spin = (uint32_t)ring->spin;

    /* the squashed radii: x = r - squash / 2, y = r + squash; the boxes are +-half of them */
    uint32_t rx = (uint32_t)ring->radius - (uint32_t)(ring->squash >> 1);
    uint32_t ry = (uint32_t)ring->radius + (uint32_t)ring->squash;
    uint16_t half_x = (uint16_t)((int16_t)(rx >> 16) >> 1);
    uint16_t half_y = (uint16_t)((int16_t)(ry >> 16) >> 1);
    r0 = rx;
    int16_t body = vm->body[0];
    set_low_word(&r0, (uint16_t)body);
    if (body != 0) {
        HitBox *box = dk::handle<HitBox>(body);
        box->spec_left = (int16_t)-half_x;
        box->spec_top = (int16_t)-half_y;
        box->spec_right = (int16_t)half_x;
        box->spec_bottom = (int16_t)half_y;
    }
    int16_t attack = vm->attack[0];
    set_low_word(&r0, (uint16_t)attack);
    if (attack != 0) {
        HitBox *box = dk::handle<HitBox>(attack);
        box->spec_left = (int16_t)-half_x;
        box->spec_top = (int16_t)-half_y;
        box->spec_right = (int16_t)half_x;
        box->spec_bottom = (int16_t)half_y;
        set_low_word(&r0, (uint16_t)(mode & RING_MODE_CHARGED));
        if (!(mode & RING_MODE_CHARGED))
            vm->delay = RING_ATTACK_OFF_RUNS;
    }

    /* the floor: on the ground the ring's y follows half the change of its y radius, so its bottom
       stays put, and the terrain boxes move the other way; airborne the box offsets follow */
    uint32_t x = (uint32_t)(uint16_t)self->x << 16 | self->x_frac;
    uint32_t y = (uint32_t)(uint16_t)self->y << 16 | self->y_frac;
    uint32_t last_ry = (uint32_t)ring->last_radius_y;
    ring->last_radius_y = (int32_t)ry;
    uint32_t r2 = (uint32_t)((int32_t)(last_ry - ry) >> 1);
    uint16_t ry_int = (uint16_t)(ry >> 16);
    r0 = (r0 & 0xFFFF0000u) | ry_int;   /* move.w d1,d0 after the swap: x radius << 16 | y radius */
    if (motion->contact & RING_ON_TERRAIN) {
        uint16_t box_y = (uint16_t)((uint16_t)(y >> 16) + (uint16_t)motion->box_b_dy);
        y += r2;
        self->y = (int16_t)(y >> 16);
        self->y_frac = (uint16_t)y;
        box_y = (uint16_t)(box_y - (uint16_t)(y >> 16));
        motion->box_a_dy = (int16_t)box_y;
        motion->box_b_dy = (int16_t)box_y;
    } else {
        uint16_t box_y = (uint16_t)((int16_t)ry_int >> 1);
        box_y = (uint16_t)(box_y + RING_AIR_BOX_DY);
        uint16_t old = (uint16_t)motion->box_a_dy;
        motion->box_a_dy = (int16_t)box_y;
        if ((int16_t)box_y > (int16_t)old)
            box_y = old;
        motion->box_b_dy = (int16_t)box_y;
    }

    /* the figures: each at the midpoint of its point on the ellipse and the one before, turned to
       face outward along the chord between them */
    uint32_t step = (uint32_t)ring->step;
    uint32_t packed = r0;   /* x radius << 16 | y radius */
    uint32_t angle = swap(spin - step);
    set_low_word(&angle, (uint16_t)(angle & 0x3FE));
    const s16 *e = sine_entry((uint16_t)angle);
    uint32_t prev_x = muls(packed >> 16, (int16_t)e[0x80]);
    uint32_t prev_y = muls(packed, (int16_t)e[0]);
    uint16_t count = vm->var[1];   /* +$24: the figure count */
    *a1 = dk::addr(ring->frame);
    while (count-- != 0) {
        uint16_t at = (uint16_t)((spin >> 16) & 0x3FE);
        e = sine_entry(at);
        uint32_t cur_x = muls(packed >> 16, (int16_t)e[0x80]);
        uint32_t cur_y = muls(packed, (int16_t)e[0]);
        uint32_t fx = (uint32_t)((int32_t)(x * 2u + cur_x + prev_x) >> 1);
        uint32_t fy = (uint32_t)((int32_t)(y * 2u + cur_y + prev_y) >> 1);
        uint32_t dx = (uint32_t)((int32_t)(cur_x - prev_x) >> 8);
        uint32_t dy = (uint32_t)((int32_t)(cur_y - prev_y) >> 8);
        uint32_t heading = prev_x;
        Mot_Atan2(&dx, &dy, &heading);
        uint16_t facing = (uint16_t)(((uint16_t)heading + RING_FIGURE_FACING) & 0x3FE);
        uint32_t product = (uint32_t)facing * 0xF00u;   /* mulu.w; the swap brings 0-59 down */
        uint16_t offset = (uint16_t)((product >> 16) * 4);
        uint32_t table = rol_word(&ring->blink_mask) ? Tbl_ToyFigureChargedFrames : Tbl_036DA8;
        PTR(const ToyFrameStep) fs;
        fs.v = table + (uint32_t)(int32_t)(int16_t)offset;
        uint32_t frame = (dy & 0xFFFF0000u) | fs->frame;   /* move.w into d1 (dy as Mot_Atan2 left it) */
        uint8_t attr = (uint8_t)(((uint16_t)spr->attr >> 8) & fs->attr_mask);
        uint32_t r0f = dx;
        set_low_byte(&r0f, fs->flip);
        attr ^= fs->flip;
        uint16_t attr_word = (uint16_t)(attr << 8);
        uint32_t a0 = fs.v + sizeof(ToyFrameStep);
        PTR(u16) slot;
        slot.v = *a1;
        uint32_t old = (product << 16) | *slot;   /* d2 after the swap, its low word the slot's frame */
        uint32_t r3f = prev_y;
        Spr_ChangeFrame(&r0f, &frame, &old, &r3f, &a0, a1);
        slot.v = *a1;
        *slot = (uint16_t)frame;
        *a1 += 2;
        uint16_t frame_word = (uint16_t)frame;
        dk::Flags spr_ccr{};
        Spr_AddFront(&r0f, &frame_word, (uint16_t)(fx >> 16), (uint16_t)(fy >> 16), attr_word, &a0, &spr_ccr);
        prev_x = cur_x;
        prev_y = cur_y;
        spin += step;
    }
    *d0 = packed;
    *d1 = step;
    *d2 = prev_x;
    *d3 = prev_y;
    *d4 = x;
    *d5 = y;
    *d6 = spin;
    *d7 = (angle & 0xFFFF0000u) | 0xFFFFu;
}
