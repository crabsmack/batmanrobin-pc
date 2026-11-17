#include "game.h"

/* the hazard's 24 perspective frames by distance (RAM frame words $FC40 near .. $FDA8 far; the
   table proper starts at $03DF14, read from $03DF04 + 2 x ((distance + $B8) >> 4)) */
static constexpr uint32_t Tbl_RoadHazardFrames = 0x03DF04u;

enum {
    HAZARD_HIT_PAUSE = 0x0F,         /* frames the hazard's attacks pause after a hit */
    HAZARD_PUSH_BACK = 0x80,         /* a hit pushes it back one checker row */
    HAZARD_FAR = 0x8C0,              /* passed (unsigned): the script kills it */
    HAZARD_ROW_BIAS = 0xC0,          /* its distance + this, + the checker phase, / 16 = its row x 8 */
    HAZARD_FRONT = 0x8000,           /* sprite priority unless its row is hidden */
    HAZARD_PLAYER_DEPTH_FIRST = 0x1E,  /* frame indexes $1E-$26 are at the players' depth */
    HAZARD_PLAYER_DEPTH_LAST = 0x26,
    HAZARD_DRAWN = 0, HAZARD_AT_PLAYERS = 1, HAZARD_PASSED = 2,   /* the switch's results */
};

/* Boss_Hatter_RoadHazardHit - hit callback of the road hazard's attack box: when
 * Hitbox_CanHitTarget accepts the struck player box, pauses the hazard's attacks (15 frames),
 * pushes it back one checker row and knocks the player back (EnemyAtk_ApplyKnockSpeed)
 * In:  a0 = the attack box, struck = the struck box (a2), a4/a6 = the hazard's VM block / object
 * Out: refused: Hitbox_CanHitTarget's d0 (nonzero) and d1; accepted: EnemyAtk_ApplyKnockSpeed's
 * Evidence: jsr Hitbox_CanHitTarget / beq.w / rts / move.b #$F,$1(a4) / addi.w #$80,$42(a6) /
 *           jmp EnemyAtk_ApplyKnockSpeed; callback long $0003DE62 in the Op2_SetAttackBox spec at
 *           $03DE38; fuzz-113400-s2
 */
void Boss_Hatter_RoadHazardHit(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5,
                               uint32_t *d6, uint32_t *d7, uint32_t *a0, HitBox *struck, uint32_t a3, uint32_t *a4,
                               uint32_t a5, uint32_t *a6)
{
    uint16_t d1w = (uint16_t)*d1;
    uint32_t a1 = 0;
    dk::Flags ccr{};
    Hitbox_CanHitTarget(d0, &d1w, &a1, struck, &ccr);
    *d1 = (*d1 & 0xFFFF0000u) | d1w;
    if (!ccr.z)
        return;
    PTR(Vm2Block) vm;
    vm.v = *a4;
    PTR(Vm2Obj) self;
    self.v = *a6;
    vm->delay = HAZARD_HIT_PAUSE;
    self->y = (int16_t)(self->y + HAZARD_PUSH_BACK);
    dk::jump(at_EnemyAtk_ApplyKnockSpeed);
}

/* Boss_Hatter_RoadHazardPlace - the road hazard's per-frame placement (switch): moves it toward
 * the players by the road speed (its y is its road distance), finds its checker row, draws it
 * there in one of 24 perspective sizes (in front unless its row is hidden behind a crest) and
 * says what the script does next
 * In:  sprite = its sprite block (a5: frame slot 0); self = the hazard (a6: x = lane x, y =
 *      distance)
 * Out: d0 = 0 drawn, 1 drawn at the players' depth (the script tests the attack box), 2 passed the
 *      far end (killed; distance not stored); self->y / draw_x / draw_y; d4/d5 = the drawn
 *      position, d6 = the priority, d7 = the frame index, a1 = its row's scale word (as Spr_ChangeFrame
 *      left it; Spr_AddBack copies it to d1), d1-d3 as the
 *      sprite calls leave them
 * Evidence: move.w $42(a6),d7 / sub.w g_SceneVars2+$C / cmpi.w #$8C0 / bcs / moveq #2; addi.w #$C0 /
 *           add.w g_SceneVars2+$E / lsr.w #4 / andi.w #$FFF8 / lea g_SceneVars+2 / #$8000 / cmp.w
 *           (a1)+,d5 / ble; muls / asl.l #4 / swap / + g_SceneVars2+$3A/$3C; subq.w #8 / lsr.w #4 /
 *           add.w / move.w $03DF04(pc,d7.w),d1 / Spr_ChangeFrame / cmpi.w #$26 / #$1E / Spr_AddBack
 *           or Spr_AddFront; Op2_CallNativeRelSwitch $03DE80 at $03DE28/$03DE58; TAS f114753-
 */
void Boss_Hatter_RoadHazardPlace(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint16_t *d5,
                                 uint16_t *d6, uint16_t *d7, uint32_t *a1, Vm2Sprite *sprite, Vm2Obj *self)
{
    const HatterRoadVars *road = hatter_road();
    uint16_t distance = (uint16_t)(self->y - road->step);
    *d7 = distance;
    if (distance >= HAZARD_FAR) {
        *d0 = HAZARD_PASSED;
        return;
    }
    self->y = (int16_t)distance;
    uint16_t depth = (uint16_t)(distance + HAZARD_ROW_BIAS);
    uint16_t offset = (uint16_t)(((uint16_t)(depth + road->phase) >> 4) & 0xFFF8u);
    *d0 = (*d0 & 0xFFFF0000u) | offset;
    /* g_SceneVars+2 + 8 x (row + 2): rows[] start at +$12 (a hazard right at the players reads
       the 8 bytes before rows[0]) */
    const RoadRow *row = (const RoadRow *)(g_SceneVars + 2 + offset);
    *a1 = dk::addr(&row->scale);   /* (a1)+ three times: a1 is left on the scale (Spr_AddBack's d1) */
    *d6 = HAZARD_FRONT;
    *d5 = (uint16_t)row->line;
    if (row->line > row->clip_line)
        *d6 = 0;
    uint32_t x = (uint32_t)((int32_t)(int16_t)(self->x + row->x_offset) * (int32_t)row->scale) << 4;
    x = (x << 16) | (x >> 16);                    /* swap: the product / 4096 in the low word */
    x = (x & 0xFFFF0000u) | (uint16_t)(x + road->origin_x);
    *d4 = x;
    *d5 = (uint16_t)(*d5 + road->origin_y);
    self->draw_x = (int16_t)(uint16_t)x;
    self->draw_y = (int16_t)*d5;
    uint16_t index = (uint16_t)(((uint16_t)(depth - 8) >> 4) * 2);
    *d7 = index;
    PTR(const u16) frames;
    frames.v = Tbl_RoadHazardFrames;
    *d1 = (*d1 & 0xFFFF0000u) | frames[index / 2];
    *d2 = (*d2 & 0xFFFF0000u) | sprite->frame[0];
    uint32_t a0 = 0;
    Spr_ChangeFrame(d0, d1, d2, d3, &a0, a1);
    sprite->frame[0] = (uint16_t)*d1;
    uint16_t d0w = (uint16_t)*d0;
    if ((int16_t)index > HAZARD_PLAYER_DEPTH_LAST) {
        dk::Flags spr_ccr{};   /* its &ccr (Spr_* hand their condition codes back) */
        Spr_AddBack(&d0w, d1, (uint16_t)*d4, *d5, *d6, a0, *a1, &spr_ccr);
        *d0 = HAZARD_DRAWN;
        return;
    }
    if ((int16_t)index >= HAZARD_PLAYER_DEPTH_FIRST) {
        dk::Flags spr_ccr{};   /* its &ccr (Spr_* hand their condition codes back) */
        Spr_AddBack(&d0w, d1, (uint16_t)*d4, *d5, *d6, a0, *a1, &spr_ccr);
        *d0 = HAZARD_AT_PLAYERS;
        return;
    }
    uint16_t d1w = (uint16_t)*d1;
    dk::Flags spr_ccr{};   /* its &ccr (Spr_* hand their condition codes back) */
    Spr_AddFront(d0, &d1w, (uint16_t)*d4, *d5, *d6, &a0, &spr_ccr);
    *d1 = (*d1 & 0xFFFF0000u) | d1w;
    *d0 = HAZARD_DRAWN;
}
