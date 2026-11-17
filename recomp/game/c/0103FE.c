#include "game.h"

/* Player_HurtFixed - a2 native: takes $20 (1/8 of full) health from the player without a hit; at
 * 0 or below it plays sound request 1, clears the health and loses a life, and removes the player
 * when none are left (the jump into Player_DespawnNative).
 * In:  self = the player object (a6: +$1E record)
 * Out: a0 = the record; the flags: the subi.w's while health is left, else Player_LoseLife's
 * Evidence: movea.l $1E(a6),a0 / subi.w #$20,$4(a0) / bgt / move.w #1,d0 / jsr Snd_PlaySfx / clr.w
 *           $4(a0) / bsr Player_LoseLife / beq / bra.w Player_DespawnNative; dc.l after op $2CBC at
 *           $4AD4E (Batman) / $4D88A (Robin)
 */
void Player_HurtFixed(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6,
                      uint32_t *d7, uint32_t *a0, uint32_t *a1, uint32_t *a2, uint32_t *a3, uint32_t *a4, uint32_t *a5,
                      Vm2Obj *self, dk::Flags *ccr)
{
    (void)d1, (void)d2, (void)d3, (void)d4, (void)d5, (void)d6, (void)d7, (void)a2, (void)a3, (void)a4, (void)a5;
    PTR(PlayerRec) rec;
    rec.v = self->player;
    *a0 = rec.raw();
    uint16_t health = rec->health;
    *ccr = dk::Flags::compare(health, 0x20, 2);
    ccr->x = ccr->c;   /* subi sets X as C */
    rec->health = (uint16_t)(health - 0x20);
    if (!ccr->z && ccr->n == ccr->v)   /* bgt: health left */
        return;
    *d0 = (*d0 & 0xFFFF0000u) | 1;
    Snd_PlaySfx(d0);
    rec->health = 0;
    Player_LoseLife(d0, a1, self, ccr);
    if (ccr->z)
        return;
    dk::jump(at_Player_DespawnNative);
}

/* Player_TakeHit - a2 switch native: applies the hit in g_StruckBox to the player (unless it is
 * invulnerable: case 0): the reaction by hit type (6 flinch: 30 invulnerable frames, case 0; 7
 * knockback: 40, case 1; 8 knockback: 60, case 1; 9 knockdown: 60, case 2; types 7-9 copy the
 * box's knockback into the motion impulse), then health -= the damage taken; below 0 the health is
 * cleared, sound request 1 plays and the case is 3 (dead); otherwise the invulnerability starts
 * and sound request 0 plays.
 * In:  mot/spr = the player's motion and sprite blocks (a3/a5); self = the player object (a6)
 * Out: d0 = the case; d1 = the invulnerable frames; d2 low word = the damage; d7 low word = the
 *      case (saved across the sound call); a1 = the record
 * Evidence: moveq #0,d0 / tst.b $16(a5) / movea.l (g_StruckBox).w,a0 / move.w $1A(a0),d2 / moveq
 *           #$1E,d1 / cmpi.w #6 / move.l $22(a0),$2(a3) / moveq #$28 / moveq #1 / cmpi.w #7 / moveq
 *           #$3C / cmpi.w #8 / moveq #2 / move.w $20(a0),d2 / clr.w / sub.w d2,$4(a1) / bpl / the
 *           sounds 1 and 0 / moveq #3; dc.l after op $2D40 at 41 sites from $4AE50
 */
void Player_TakeHit(uint32_t *d0, uint32_t *d1, uint16_t *d2, uint16_t *d7, uint32_t *a1, Vm2Motion *mot,
                    Vm2Sprite *spr, const Vm2Obj *self)
{
    *d0 = 0;
    if (spr->invulnerable)
        return;
    PTR(HitBox) box;
    box.v = g_StruckBox;
    uint16_t type = box->hit_type;
    *d2 = type;
    *d1 = 0x1E;
    if (type != 6) {
        mot->impulse_x = box->knockback;
        *d1 = 0x28;
        *d0 = 1;
        if (type != 7) {
            *d1 = 0x3C;
            if (type != 8)
                *d0 = 2;
        }
    }
    PTR(PlayerRec) rec;
    rec.v = self->player;
    *a1 = rec.raw();
    uint16_t damage = box->damage;
    *d2 = damage;
    box->damage = 0;
    rec->health = (uint16_t)(rec->health - damage);
    if ((int16_t)(uint16_t)rec->health < 0) {
        rec->health = 0;
        *d0 = (*d0 & 0xFFFF0000u) | 1;
        Snd_PlaySfx(d0);
        *d0 = 3;
        return;
    }
    spr->invulnerable = (uint8_t)*d1;
    *d7 = (uint16_t)*d0;
    *d0 &= 0xFFFF0000u;
    Snd_PlaySfx(d0);
    *d0 = (*d0 & 0xFFFF0000u) | *d7;
}
