#include "game.h"

/* the facade's segment width and the x splits of its four sign slots (facade pixels) */
static constexpr uint16_t FACADE_SEGMENT_W = 0x168;
static constexpr int16_t SIGN_SLOT_SPLIT[3] = { 0, 0xA8, 0x150 };
static constexpr uint8_t SIGN_BLASTED = 8;            /* sign_wanted: the blasted panel */
static constexpr uint16_t SIGN_DEBRIS_Y = 0x96;
static constexpr uint16_t SIGN_DEBRIS_ATTR = 0x2000;  /* palette line 1 */
static constexpr uint16_t SFX_SIGN_BLAST = 0x13;

/* the body both entries share; the registers are the ones Stage1_BlastSignChained hands on to the
 * trigger entry's second callback (it spawns at d4/d5) */
static void blast_sign(uint32_t *d0, uint32_t *d1, uint16_t x, uint16_t *d5, uint16_t *d6, uint32_t *d7, uint32_t *a0,
                       uint32_t *a1, uint32_t *a2, uint32_t *a3, uint32_t *a4, uint32_t *a5, uint32_t *a6)
{
    /* the facade x: x * 35/32 (x + x/8 - x/32, word arithmetic), less the segments scrolled past */
    int16_t eighth = (int16_t)x >> 3;
    uint16_t facade = (uint16_t)(x + eighth - (eighth >> 2));
    *d1 = (uint32_t)g_Stage1Segment * FACADE_SEGMENT_W;   /* mulu.w: the whole register */
    facade = (uint16_t)(facade - (uint16_t)*d1);
    *d0 = (*d0 & 0xFFFF0000u) | facade;
    int slot = 0;
    while (slot < 3 && (int16_t)facade > SIGN_SLOT_SPLIT[slot])   /* ble: this slot */
        slot++;
    *d1 = (uint32_t)slot;
    Stage1StreetVars *vars = (Stage1StreetVars *)g_SceneVars;
    vars->sign_wanted[slot] = SIGN_BLASTED;
    *a0 = ADDR68(vars->sign_wanted);
    *d5 = SIGN_DEBRIS_Y;
    *d6 = SIGN_DEBRIS_ATTR;
    Stage1_SpawnSignDebris(d1, x, *d5, *d6, d7, a0, a1, a2, a3, a4, a5, a6);
    *d0 = (*d0 & 0xFFFF0000u) | SFX_SIGN_BLAST;
    Snd_PlaySfx(d0);
    *d0 = 0;
}

/* Stage1_BlastSignChained - street trigger callback: blasts the bank sign nearest the trigger x
 * (Stage1_BlastSignAt), then returns into the trigger entry's second callback long
 * In:  d4 = the trigger x; a1 = the trigger entry after its callback long (Trigger_Fire)
 * Out: as Stage1_BlastSignAt, then the return goes to the pushed long (Enemy_Brute_SpawnFromSign,
 *      which spawns at d4/d5 = x/$96), whose rts returns to this callback's caller
 * Evidence: move.l (a1),-(a7) then falls into Stage1_BlastSignAt; entries $01622E/$016252/... in
 *           Tbl_015F5A are size $12 with $00020866 then $0001ACE0; TAS f2896-f4284 (9 frames)
 */
void Stage1_BlastSignChained(uint32_t *d0, uint32_t *d1, uint16_t d4, uint16_t *d5, uint16_t *d6, uint32_t *d7,
                             uint32_t *a0, uint32_t *a1, uint32_t *a2, uint32_t *a3, uint32_t *a4, uint32_t *a5,
                             uint32_t *a6)
{
    PTR(const u32) entry;
    entry.v = *a1;
    dk::push_long(*entry);            /* the second callback: this function's rts goes there */
    blast_sign(d0, d1, d4, d5, d6, d7, a0, a1, a2, a3, a4, a5, a6);
}

/* Stage1_BlastSignAt - blasts the bank sign slot nearest world x d4: sets its wanted panel to 8
 * (blasted) for Stage1_VIntFacade, throws debris and plays sound $13
 * In:  d4 = world x; a6 for Obj_NewMulti
 * Out: d0 = 0; d1 = the slot (0-3); d5 = $96, d6 = $2000 (the debris burst); d7 as the debris spawn
 *      left it; Stage1StreetVars.sign_wanted[slot] = 8
 * Evidence: facade x = d4*35/32 - g_Stage1Segment*$168 split at 0/$A8/$150 into slots 0-3 (as
 *           Stage1_UpdateStreetScroll); lea g_SceneVars+6,a0 / move.b #8,(a0,d1.w);
 *           Stage1_SpawnSignDebris at y $96; Snd_PlaySfx #$13; entered from
 *           Stage1_BombingBlastLeftSign/RightSign (bra)
 */
void Stage1_BlastSignAt(uint32_t *d0, uint32_t *d1, uint16_t d4, uint16_t *d5, uint16_t *d6, uint32_t *d7, uint32_t a6)
{
    uint32_t a0 = 0, a1 = 0, a2 = 0, a3 = 0, a4 = 0, a5 = 0, r6 = a6;   /* clobbered */
    blast_sign(d0, d1, d4, d5, d6, d7, &a0, &a1, &a2, &a3, &a4, &a5, &r6);
}
