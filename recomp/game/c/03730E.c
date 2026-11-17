#include "game.h"

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
    CHAIN_MODE_BEND = 1 << 0,       /* VM variable 0 bit: the second bend mode (the sway grows link by link) */
    CHAIN_MODE_OUTLINED = 0x0C,     /* bits that ask for the outlined frames */
    CHAIN_MODE_CHARGED = 1 << 2,    /* charged: the chain block's blink pattern */
    CHAIN_WARNING_PHASE = 1 << 3,   /* warning (bit 3 alone): outlined on random frames of 8-frame phases */
    CHAIN_LINK_SPACING = 0x14,      /* 20: the link spacing the extension counts in */
    CHAIN_SWAY_DIV = 0xA0,          /* the sway's bend divisor */
};

static void set_low_word(uint32_t *r, uint16_t v) { *r = (*r & 0xFFFF0000u) | v; }
static void set_low_byte(uint32_t *r, uint8_t v) { *r = (*r & 0xFFFFFF00u) | v; }
static uint32_t swap(uint32_t v) { return v << 16 | v >> 16; }
/* muls.w: the low words, signed */
static uint32_t muls(uint32_t a, uint32_t b) { return (uint32_t)((int32_t)(int16_t)(uint16_t)a * (int16_t)(uint16_t)b); }
static bool rol_word(u16 *w) { uint16_t v = *w; *w = (uint16_t)(v << 1 | v >> 15); return (v & 0x8000u) != 0; }

/* ToyChain_PlaceAndDrawLinks - places and draws a chain's links: from the drawn base each link sits
 * further along a heading that starts at the base heading + (sway >> 8) * the sway scale and bends
 * with the sway from link to link (variable 0 bit 0 picks one of two bend modes); each is drawn
 * rotated by its heading + the frame turn through Tbl_036DA8 or the outlined frames (charged /
 * blinking); the links' positions go into each link, the end into the tip
 * In:  chain = the chain block (a2); vm = the chain's VM block (a4: +$22 variable 0, +$24 the
 *      count); spr = its sprite block (a5: attributes +$38, depth +$3A); self = the chain object
 *      (a6: +$3E the sway value, 16.16, moved by the motion ops; +$4E/+$50 the drawn base)
 * Out: g_VM2Arg0 = the base heading; the blink mask; each link's x/y and frame, its sprite added;
 *      the tip. The registers as the 68000 leaves them: d0 = the last heading, d1 = Spr_Add*'s,
 *      d2/d3/d6 the bend accumulators, d4/d5 = the tip (integer parts in the low words), d7 =
 *      $FFFF, a1 = the last link, a3 = past the link list
 * Evidence: lea $34(a2),a3 / move.l $3E(a6),d3 / asr.l #8 / muls.w 8(a2) / add.l (a2),d3 / add.l
 *           4(a2),d2 / move.l d2,g_VM2Arg0 / the base $4E/$50 << 16 | $8000 / the blink (as
 *           ToyFigure_DrawIndex's) into $1A(a2) / addi.w #$14 / dbge d7 / blt / divs.w #$A0,d3 /
 *           muls.w / asl.l #2 / Tbl_059B22 cos +$100, sin / muls.w #$14 / mulu.w #$F00 / rol.w
 *           $1A(a2) / Spr_ChangeFrame / Spr_AddBack or Spr_AddSorted / dbra d7 / move.w d4,$30(a2)
 *           / move.w d5,$32(a2); called by ToyChain_Update; TAS f79928-f112311 (6555 frames). The
 *           stack pushes inside the loop only keep registers over the calls: the C keeps them in
 *           locals
 */
void ToyChain_PlaceAndDrawLinks(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5,
                                uint32_t *d6, uint16_t *d7, uint32_t *a1, ToyChainBlock *chain, uint32_t *a3,
                                const Vm2Block *vm, const Vm2Sprite *spr, const Vm2Obj *self)
{
    *a3 = dk::addr(chain->link);
    uint32_t sway = (uint32_t)(uint16_t)self->x << 16 | self->x_frac;
    uint32_t r2 = muls((uint32_t)((int32_t)sway >> 8), (uint16_t)chain->sway_scale);
    uint32_t r3 = sway + (uint32_t)chain->sway_base;
    r2 += (uint32_t)chain->heading;
    g_VM2Arg0 = r2;
    uint32_t r6 = r3;
    uint32_t r4 = (uint32_t)(uint16_t)self->draw_x << 16 | 0x8000u;
    uint32_t r5 = (uint32_t)(uint16_t)self->draw_y << 16 | 0x8000u;
    uint16_t r7 = (uint16_t)(vm->var[1] - 1);   /* +$24: the link count */
    uint32_t r0 = *d0, r1 = *d1;
    auto leave = [&]() {
        *d0 = r0; *d1 = r1; *d2 = r2; *d3 = r3; *d4 = r4; *d5 = r5; *d6 = r6; *d7 = r7;
    };
    if ((int16_t)r7 < 0) {
        leave();
        return;
    }

    /* the blink: this frame's mask (bit 15 first: outlined) */
    uint16_t mode = vm->var[0];
    set_low_word(&r0, (uint16_t)(mode & CHAIN_MODE_OUTLINED));
    if (mode & CHAIN_MODE_OUTLINED) {
        set_low_word(&r0, (uint16_t)(mode & CHAIN_MODE_CHARGED));
        if (mode & CHAIN_MODE_CHARGED) {
            set_low_word(&r0, chain->blink_pattern);
        } else {
            set_low_word(&r0, (uint16_t)(g_FrameCounter & CHAIN_WARNING_PHASE));
            if ((uint16_t)r0 != 0) {
                r0 = (uint32_t)Rand_Next();
                set_low_word(&r0, (uint16_t)(r0 & 1u));
                if ((uint16_t)r0 != 0)
                    set_low_word(&r0, 0xFFFF);
            }
        }
    }
    chain->blink_mask = (uint16_t)r0;

    /* the links the extension still hides: 20 more each until it reaches 0 (dbge) */
    set_low_word(&r0, (uint16_t)((uint32_t)chain->extension >> 16));
    bool ge;
    for (;;) {
        uint16_t a = (uint16_t)r0;
        uint16_t sum = (uint16_t)(a + CHAIN_LINK_SPACING);
        set_low_word(&r0, sum);
        bool n = (sum & 0x8000u) != 0;
        bool v = !(a & 0x8000u) && (sum & 0x8000u);   /* a positive operand overflowing */
        ge = n == v;
        if (ge)
            break;
        r7 = (uint16_t)(r7 - 1);
        if (r7 == 0xFFFFu)
            break;
    }
    if (!ge) {   /* blt: every link still hidden */
        leave();
        return;
    }
    set_low_word(&r0, (uint16_t)((uint16_t)r0 - CHAIN_LINK_SPACING));
    set_low_word(&r0, (uint16_t)((uint16_t)r0 * 2));

    /* divs.w #$A0,d3: quotient low, remainder high; an overflow leaves d3 */
    int32_t q = (int32_t)r3 / CHAIN_SWAY_DIV, rem = (int32_t)r3 % CHAIN_SWAY_DIV;
    if (q >= -0x8000 && q <= 0x7FFF)
        r3 = (uint32_t)(uint16_t)rem << 16 | (uint16_t)q;
    r3 = muls(r3, r0) << 2;
    r3 += r6;
    uint32_t r1i = (r2 >> 16) & 0x3FE;   /* swap / andi.l #$3FE */
    const s16 *e = sine_entry((uint16_t)r1i);
    r1 = r1i;
    set_low_word(&r1, (uint16_t)(int16_t)e[0x80]);
    r1 = muls(r1, r0);
    r4 += r1;
    set_low_word(&r1, (uint16_t)(int16_t)e[0]);
    r1 = muls(r1, r0);
    r5 += r1;
    r0 = r2;
    set_low_word(&r1, (uint16_t)(mode & CHAIN_MODE_BEND));
    if (!(mode & CHAIN_MODE_BEND)) {
        r3 -= r6;
        r2 += r3;
        r3 = r6;
        r6 = 0;
    }

    /* the links, base first */
    do {
        r0 = swap(r0);
        set_low_word(&r0, (uint16_t)(r0 & 0x3FE));
        e = sine_entry((uint16_t)r0);
        set_low_word(&r1, (uint16_t)(int16_t)e[0x80]);
        r1 = muls(r1, CHAIN_LINK_SPACING);
        r4 += r1;
        uint32_t next_x = r1 + r4;
        set_low_word(&r1, (uint16_t)(int16_t)e[0]);
        r1 = muls(r1, CHAIN_LINK_SPACING);
        r5 += r1;
        uint32_t next_y = r1 + r5;
        PTR(const s16) slot;
        slot.v = *a3;
        *a3 += 2;
        *a1 = (uint32_t)(int32_t)*slot;   /* movea.w: the link */
        uint32_t keep2 = r2, keep3 = r3, keep6 = r6;
        uint16_t keep7 = r7;
        r4 = swap(r4);
        r5 = swap(r5);
        PTR(ToyChainLink) link;
        link.v = *a1;
        link->x = (int16_t)(uint16_t)r4;
        link->y = (int16_t)(uint16_t)r5;
        set_low_word(&r0, (uint16_t)(((uint16_t)r0 + chain->frame_turn) & 0x3FE));
        r0 = (uint32_t)(uint16_t)r0 * 0xF00u;   /* mulu.w: 0-59 in the high word */
        r0 = swap(r0);
        set_low_word(&r0, (uint16_t)((uint16_t)r0 * 4));
        uint32_t table = rol_word(&chain->blink_mask) ? Tbl_ToyFigureChargedFrames : Tbl_036DA8;
        PTR(const ToyFrameStep) fs;
        fs.v = table + (uint32_t)(int32_t)(int16_t)(uint16_t)r0;
        set_low_word(&r1, fs->frame);
        uint8_t attr = (uint8_t)(((uint16_t)spr->attr >> 8) & fs->attr_mask);
        set_low_byte(&r0, fs->flip);
        attr ^= fs->flip;
        set_low_word(&r6, (uint16_t)(attr << 8));
        uint32_t a0 = fs.v + sizeof(ToyFrameStep);
        set_low_word(&r2, link->frame);
        Spr_ChangeFrame(&r0, &r1, &r2, &r3, &a0, a1);
        link.v = *a1;
        link->frame = (uint16_t)r1;
        r7 = spr->depth;
        dk::Flags spr_ccr{};
        uint16_t node = (uint16_t)r0;
        if (r7 != 0)
            Spr_AddSorted(&node, &r1, &r2, (uint16_t)r4, (uint16_t)r5, (uint16_t)r6, r7, a0, *a1, dk::addr(chain),
                          &spr_ccr);
        else
            Spr_AddBack(&node, &r1, (uint16_t)r4, (uint16_t)r5, (uint16_t)r6, a0, *a1, &spr_ccr);
        set_low_word(&r0, node);
        r2 = keep2;
        r3 = keep3;
        r6 = keep6;
        r7 = keep7;
        r5 = next_y;
        r4 = next_x;
        r2 += r3;
        r3 += r6;
        r0 = r2;
    } while (r7-- != 0);
    r4 = swap(r4);
    r5 = swap(r5);
    chain->tip_x = (int16_t)(uint16_t)r4;
    chain->tip_y = (int16_t)(uint16_t)r5;
    leave();
}
