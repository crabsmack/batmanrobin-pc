#include "game.h"

/* Taking a player out of play, and the handler of a player on screen ($0102D6-$0103CD). */

/* the weapon charge added per frame: 3 weapon types x 8 words (entry 0 while the rapid-charge
 * countdown runs, else entry level + 1) */
static constexpr uint32_t Tbl_WeaponChargeRate = 0x0103CEu;

/* Player_Despawn - takes a player out of play: the handler back to Player_JoinOrSpawn, the
 * record's in-play bit and latch fields cleared, the sprite frames and collision boxes released
 * and the 3 pool blocks freed (the last by the jump into ObjPool_Free).
 * In:  self = the player object (a6); a2 = the record
 * Out: a3/a4/a5 = the object's blocks, a0 = the sprite block; ObjPool_Free's Z
 * Evidence: move.l #$101CE,(a6) / bclr #0,$25(a2) / sf.b $3A(a2) / clr.w $36(a2) / clr.w $38(a2) /
 *           movem of the handles / jsr Obj_ReleaseFrames / moveq #0,d7 / jsr Obj_SetBodyBoxes /
 *           moveq #0,d7 / jsr Obj_SetAttackBoxes / ObjPool_Free x2 / bra.w ObjPool_Free; only from
 *           Player_DespawnNative
 */
void Player_Despawn(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6,
                    uint32_t *d7, uint32_t *a0, uint32_t *a1, uint32_t *a2, uint32_t *a3, uint32_t *a4, uint32_t *a5,
                    Vm2Obj *self)
{
    self->handler = at_Player_JoinOrSpawn;
    PTR(PlayerRec) rec;
    rec.v = *a2;
    rec->flags = (uint8_t)(rec->flags & ~PLAYER_IN_PLAY);
    rec->targetable = 0;
    rec->attackers_last = 0;
    rec->attackers = 0;
    *a3 = dk::addr(dk::handle(self->motion));
    *a4 = dk::addr(dk::handle(self->vm));
    *a5 = dk::addr(dk::handle(self->sprite));
    uint16_t w0 = (uint16_t)*d0, w2 = (uint16_t)*d2, w7 = (uint16_t)*d7;
    PTR(u16) slot;
    slot.v = *a1;
    u16 *slot_p = slot;
    Obj_ReleaseFrames(&w0, &w2, &w7, &slot_p, dk::handle<Vm2Sprite>(self->sprite));
    *d0 = (*d0 & 0xFFFF0000u) | w0;
    *d2 = (*d2 & 0xFFFF0000u) | w2;
    *a1 = dk::addr(slot_p);
    uint16_t none = 0;
    Obj_SetBodyBoxes(d0, d1, d2, d3, d4, d5, d6, &none, a1, a2, *a4, dk::addr(self));
    none = 0;
    dk::Flags boxes_ccr{};   /* its condition codes stay in the CPU too */
    Obj_SetAttackBoxes(d0, d1, d2, d3, d4, d5, d6, &none, a1, a2, *a4, dk::addr(self), &boxes_ccr);
    *d7 = none;
    ObjPool_Free(*a3);
    ObjPool_Free(*a4);
    *a0 = *a5;
    dk::jump(at_ObjPool_Free);
}

/* Player_Run - handler of a player on screen: unless script variable 1 has bit $400 (no control),
 * feeds the pad into variable 0 (pressed << 8 | held) and the button releases into variable 1
 * bit $20 (A/B/C while the flying bits 1-2 are set, else B/C), then Player_RunScript.
 * In:  self = the player object (a6: +$58 pad)
 * Out: a4 = its VM block; a0 = its pad (when read); d0 = the release test; then Player_RunScript's
 * Evidence: movea.w $A(a6),a4 / andi.w #$400 of $24(a4) / lea g_Pad1 or g_Pad2,a0 / move.b $2(a0),d0
 *           / lsl.w #8 / move.b (a0),d0 / move.w d0,$22(a4) / move.b $3(a0),d0 / andi.w #6 of $24(a4)
 *           / andi.w #$70 or #$60 / ori.w #$20,$24(a4); handler long $10322 (Player_JoinOrSpawn)
 */
void Player_Run(uint32_t *d0, uint32_t *d1, uint32_t d2, uint32_t d3, uint32_t d4, uint32_t d5, uint32_t d6,
                uint32_t d7, uint32_t *a0, uint32_t a1, uint32_t *a2, uint32_t *a3, uint32_t *a4, const Vm2Obj *self)
{
    (void)d2, (void)d3, (void)d4, (void)d5, (void)d6, (void)d7, (void)a1, (void)a2, (void)a3;
    Vm2Block *vm = dk::handle<Vm2Block>(self->vm);
    *a4 = dk::addr(vm);
    uint16_t no_control = (uint16_t)vm->var[1] & 0x400;
    *d0 = (*d0 & 0xFFFF0000u) | no_control;
    if (no_control) {
        dk::jump(at_Player_RunScript);
        return;
    }
    const u8 *pad = (self->owner_id == 1) ? g_Pad1 : g_Pad2;
    *a0 = dk::addr(pad);
    uint16_t w = (uint16_t)(((uint16_t)pad[2] << 8) | pad[0]);
    vm->var[0] = w;
    w = (uint16_t)((w & 0xFF00u) | pad[3]);
    uint16_t flying = (uint16_t)vm->var[1] & 6;
    *d1 = (*d1 & 0xFFFF0000u) | flying;
    w &= flying ? 0x70 : 0x60;
    *d0 = (*d0 & 0xFFFF0000u) | w;
    if (w)
        vm->var[1] |= 0x20;
    dk::jump(at_Player_RunScript);
}

/* Player_RunScript - the player's frame after the pad feed: runs its a2 script (VM2_RunObject),
 * copies the position to the record, latches +$38 into +$36 (+$3A = $FF) while script variable
 * 1 has bit $40 (else clears them), clears +$38, and adds the weapon charge for its type and level
 * (entry 0, $280, while the rapid-charge countdown runs, counting it down).
 * In:  self = the player object (a6)
 * Out: a0 = the record; a2-a5 as the script left them; d0 = the charge added, d1 = type << 4;
 *      the flags of the charge's add.w
 * Evidence: jsr VM2_RunObject / movea.l $1E(a6),a0 / move.l $3E(a6),$2E(a0) / move.l $42(a6),$32(a0)
 *           / andi.w #$40 / sf.b or st.b $3A(a0) / clr.w $38(a0) / tst.w $26(a0) / subq.w / moveq #0,d0
 *           or (level + 1) * 2 / asl.w #4 of the type / move.w Tbl_WeaponChargeRate(pc,d0.w),d0 /
 *           add.w d0,$8(a0); branched to by Player_JoinOrSpawn and Player_Run
 */
void Player_RunScript(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6,
                      uint32_t *d7, uint32_t *a0, uint32_t *a1, uint32_t *a2, uint32_t *a3, uint32_t *a4, uint32_t *a5,
                      Vm2Obj *self, dk::Flags *ccr)
{

    const u16 *script = 0;
    void *motion = 0, *sprite = 0;
    Vm2Block *vm = 0;
    VM2_RunObject(d0, d1, d2, d3, d4, d5, d6, d7, a0, a1, &script, &motion, &vm, &sprite, self);
    *a2 = dk::addr(script);
    *a3 = dk::addr(motion);
    *a4 = dk::addr(vm);
    *a5 = dk::addr(sprite);
    PTR(PlayerRec) rec;
    rec.v = self->player;
    *a0 = rec.raw();
    rec->x = self->x;
    rec->x_frac = self->x_frac;
    rec->y = self->y;
    rec->y_frac = self->y_frac;
    uint16_t latch = (uint16_t)vm->var[1] & 0x40;
    *d0 = (*d0 & 0xFFFF0000u) | latch;
    if (!latch) {
        rec->targetable = 0;
        rec->attackers_last = 0;
    } else {
        rec->targetable = 0xFF;
        rec->attackers_last = rec->attackers;
    }
    rec->attackers = 0;
    if (rec->rapid_charge) {
        rec->rapid_charge -= 1;
        *d0 = 0;
    } else {
        *d0 = (*d0 & 0xFFFF0000u) | (uint16_t)(((uint16_t)rec->weapon_level + 1) * 2);
    }
    uint16_t type16 = (uint16_t)((uint16_t)rec->weapon_type << 4);
    *d1 = (*d1 & 0xFFFF0000u) | type16;
    uint16_t index = (uint16_t)((uint16_t)*d0 + type16);
    PTR(const u16) rate;
    rate.v = Tbl_WeaponChargeRate + (uint32_t)(int32_t)(int16_t)index;
    uint16_t add = *rate;
    *d0 = (*d0 & 0xFFFF0000u) | add;
    uint16_t before = rec->weapon_charge;
    uint16_t after = (uint16_t)(before + add);
    rec->weapon_charge = after;
    bool c = (uint32_t)before + add > 0xFFFF;
    bool v = ((~(before ^ add) & (before ^ after)) & 0x8000) != 0;
    *ccr = dk::Flags{ c, (after & 0x8000) != 0, after == 0, v, c };
}
