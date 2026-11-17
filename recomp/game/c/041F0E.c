#include "game.h"

/* the sine table (a word per 2 units of angle, 1.15; the cosine $100 bytes on) */
static constexpr uint32_t Tbl_059B22 = 0x059B22u;
/* the puff animation: 9 word pairs (sprite frame, hits players) indexed by phase >> 8 */
static constexpr uint32_t Scr2_04204C = 0x04204Cu;
/* the hit spark */
static constexpr uint32_t Scr4_010964 = 0x010964u;

enum {
    JET_PUFFS = 5,
    PUFF_DELAY_STEP = 0x1E0,      /* each puff starts this much later than the one before */
    PUFF_PHASE_STEP = 0x80,       /* phase per frame */
    PUFF_PHASE_END = 0x900,       /* the puff starts again from the orb */
    PUFF_START_RADIUS = 0x280000, /* 40 px out, 16.16 */
    PUFF_HALF_SIZE = 3,           /* the damaging box: 7x7 around the puff */
    PUFF_DAMAGE = 0x10,
    PUFF_HIT_TYPE = 8,            /* knockback */
    PUFF_KNOCKBACK = 0x20000,     /* 2.0, away from the puff */
    SFX_GAS_JET = 0x3C,
};

static void set_low_word(uint32_t *r, uint16_t v) { *r = (*r & 0xFFFF0000u) | v; }
static uint32_t swap(uint32_t v) { return (v << 16) | (v >> 16); }
static uint32_t muls(uint32_t a, int16_t b) { return (uint32_t)((int32_t)(int16_t)a * b); }

/* Enemy_GasOrb_StepJet - one gas jet of a gas orb: when its bit is clear in script variable 3 the
 * puff block is freed; when set, it is allocated (5 puffs, each $1E0 phase later than the one
 * before) or each puff moves out along the jet's heading, is drawn, and on the damaging frames hits
 * the first player box it touches (+$10 damage, knockback 2.0 away); then sound $3C
 * In:  d0.w = the jet's bit; slot = the orb's field holding the puff block's handle (a2: +$12/$14/
 *      $16); jet = the jet's motion block (a3: heading +$C, y accel +$28, cruise y +$34); a4 = the
 *      orb's VM block (var 3); sprite = its sprite block (a5: attr, depth); a6 = the orb (x/y, the
 *      puff sprite slot +$2A)
 * Out: freed: d0.w = the handle, ObjPool_Free's; empty: d0.w = 0; allocated: d0 = $F6A0 (low word),
 *      d1 = 0, d7 = $FFFF, a0 = past the puffs; stepped: d7 = $FFFF, d0 = Snd_PlaySfx's, the rest
 *      as the last puff's calls left them
 * Evidence: and.w $28(a4),d0 / bne / move.w (a2),d0 / beq rts / clr.w (a2) / jmp ObjPool_Free; jsr
 *           ObjPool_AllocClear / the dbra of 5 (phase 0, -$1E0 ...); the step loop at $041F4C:
 *           addi.w #$80 / bcs / bpl / cmpi.w #$900 / restart moveq #0, #$280000, $34(a3) / else +4
 *           += $28(a3), +0 += +4 / swap / muls $100(a0) and (a0) of Tbl_059B22 / + x/y / lsr.w #6 /
 *           andi.w #$FFFC / Scr2_04204C pairs / Spr_ChangeFrame / Spr_AddFront / tst.w (a7)+ / the
 *           7x7 box Hitbox_FindPlayerBox / Hitbox_CanHitTarget / addi.w #$10,$20(a2) / move.w
 *           #8,$1A(a2) / the $20000 knockback / Hitbox_SpawnAtContact Scr4_010964 / move.w #$3C,d0 /
 *           Snd_PlaySfx; 3 bsr from Enemy_GasOrb_RunJets
 */
void Enemy_GasOrb_StepJet(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5,
                          uint32_t *d6, uint32_t *d7, uint32_t *a0, uint32_t *a1, s16 *slot, const Vm2Motion *jet,
                          uint32_t *a4, const Vm2Sprite *sprite, uint32_t *a6)
{
    PTR(const Vm2Block) vm;
    vm.v = *a4;
    set_low_word(d0, (uint16_t)(*d0 & vm->var[3]));
    if ((uint16_t)*d0 == 0) {
        set_low_word(d0, (uint16_t)*slot);
        if (*slot == 0)
            return;
        *slot = 0;
        *a0 = (uint32_t)(int32_t)(int16_t)(uint16_t)*d0;
        dk::jump(at_ObjPool_Free);
        return;
    }
    set_low_word(d0, (uint16_t)*slot);
    if (*slot == 0) {
        ObjPool_AllocClear(a0);
        *slot = (int16_t)(uint16_t)*a0;
        *d0 = 0;
        *d1 = 0;
        *d7 = JET_PUFFS - 1;
        GasPuff *puff = (GasPuff *)dk::handle(*slot);
        for (int i = 0; i < JET_PUFFS; i++) {
            puff[i].phase = (uint16_t)*d0;
            set_low_word(d0, (uint16_t)(*d0 - PUFF_DELAY_STEP));
            set_low_word(d7, (uint16_t)(*d7 - 1));   /* dbra */
        }
        *a0 = dk::addr(puff + JET_PUFFS);
        return;
    }

    GasPuff *puffs = (GasPuff *)dk::handle(*slot);
    PTR(const s16) sine;
    sine.v = Tbl_059B22;
    PTR(const u16) anim;
    anim.v = Scr2_04204C;
    *d7 = JET_PUFFS - 1;
    for (int i = 0; i < JET_PUFFS; i++) {
        uint32_t counter = *d7;   /* movem.l d7/a2 around the puff */
        GasPuff *puff = &puffs[i];
        uint16_t phase = puff->phase;
        bool restart = (uint32_t)phase + PUFF_PHASE_STEP > 0xFFFFu;   /* bcs */
        phase = (uint16_t)(phase + PUFF_PHASE_STEP);
        set_low_word(d0, phase);
        if (!restart && (int16_t)phase < 0) {   /* still in its start delay */
            puff->phase = phase;
            *d7 = counter;
            set_low_word(d7, (uint16_t)(*d7 - 1));
            continue;
        }
        if (restart || phase >= PUFF_PHASE_END) {
            *d0 = 0;
            *d4 = PUFF_START_RADIUS;
            *d5 = (uint32_t)(int32_t)jet->cruise_y;
        } else {
            *d4 = (uint32_t)(int32_t)puff->radius;
            *d5 = (uint32_t)(int32_t)puff->speed + (uint32_t)(int32_t)jet->accel_y;
            *d4 += *d5;
        }
        puff->phase = (uint16_t)*d0;
        puff->radius = (int32_t)*d4;
        puff->speed = (int32_t)*d5;

        /* the puff's position: the orb + radius * (cos, sin) of the jet's heading */
        *d4 = swap(*d4);
        set_low_word(d5, (uint16_t)*d4);
        set_low_word(d1, (uint16_t)((uint32_t)(int32_t)jet->heading >> 16));
        set_low_word(d1, (uint16_t)(*d1 & 0x3FE));
        const s16 *angle = sine.get() + ((uint16_t)*d1 >> 1);
        *a0 = dk::addr(angle);
        *d4 = swap(muls(*d4, angle[0x100 / 2]));
        *d5 = swap(muls(*d5, angle[0]));
        PTR(S4Obj) self;
        self.v = *a6;
        set_low_word(d4, (uint16_t)(*d4 + self->x));
        set_low_word(d5, (uint16_t)(*d5 + self->y));

        /* its frame and hit flag */
        set_low_word(d0, (uint16_t)(((uint16_t)*d0 >> 6) & 0xFFFC));
        const u16 *pair = anim.get() + ((uint16_t)*d0 >> 1);
        *a0 = dk::addr(anim.get());
        set_low_word(d1, pair[0]);
        uint16_t hits = pair[1];
        set_low_word(d2, self->sprite_frame);
        Spr_ChangeFrame(d0, d1, d2, d3, a0, a1);
        self->sprite_frame = (uint16_t)*d1;
        set_low_word(d6, sprite->attr);
        set_low_word(d7, sprite->depth);
        uint16_t w1 = (uint16_t)*d1;
        dk::Flags spr_ccr{};   /* its &ccr (Spr_* hand their condition codes back) */
        Spr_AddFront(d0, &w1, (uint16_t)*d4, (uint16_t)*d5, (uint16_t)*d6, a0, &spr_ccr);
        set_low_word(d1, w1);

        if (hits != 0) {
            set_low_word(d6, (uint16_t)*d4);
            set_low_word(d7, (uint16_t)*d5);
            set_low_word(d4, (uint16_t)(*d4 - PUFF_HALF_SIZE));
            set_low_word(d5, (uint16_t)(*d5 - PUFF_HALF_SIZE));
            set_low_word(d6, (uint16_t)(*d6 + PUFF_HALF_SIZE));
            set_low_word(d7, (uint16_t)(*d7 + PUFF_HALF_SIZE));
            uint32_t found = dk::addr(puff);
            Hitbox_FindPlayerBox(d0, (uint16_t)*d4, (uint16_t)*d5, (uint16_t)*d6, (uint16_t)*d7, &found);
            /* Z clear (a box overlaps) leaves a2 on a linked box; Z set on the list's sentinel */
            PTR(const u16) link;
            link.v = found;
            PTR(HitBox) box;
            box.v = found;
            if (*link.get() != 0) {
                dk::Flags f{};
                w1 = (uint16_t)*d1;
                Hitbox_CanHitTarget(d0, &w1, a1, box.get(), &f);
                set_low_word(d1, w1);
                if (f.z) {
                    box->damage += PUFF_DAMAGE;
                    box->hit_type = PUFF_HIT_TYPE;
                    *d2 = PUFF_KNOCKBACK;
                    uint16_t side = (uint16_t)(box->left + box->right - (uint16_t)*d4 - (uint16_t)*d6);
                    set_low_word(d1, side);
                    if ((int16_t)side < 0)
                        *d2 = 0u - *d2;
                    box->knockback = (int32_t)*d2;
                    *a1 = Scr4_010964;
                    Hitbox_SpawnAtContact(d0, d1, d2, *d3, *d4, *d5, *d6, *d7, *a0, *a1, box.get(), dk::addr(jet),
                                          a4, dk::addr(sprite), a6);
                }
            }
        }
        *d7 = counter;
        set_low_word(d7, (uint16_t)(*d7 - 1));   /* dbra */
    }
    set_low_word(d0, SFX_GAS_JET);
    Snd_PlaySfx(d0);
}
