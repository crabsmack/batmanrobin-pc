#include "game.h"

/* the shot scripts: [type 0-2][level 0-7][plain, charged] a4 script longs; the flying section's
 * set from +$C0 */
static constexpr uint32_t Tbl_010AF8 = 0x010AF8u;

/* the sprite attribute bit for a mirrored (facing left) object: bit 3 of the attribute's high byte */
static constexpr uint16_t ATTR_FLIP_H = 0x0800;

/* The body of both natives ($010A86): reads the operands (dx, dy, a long whose high word is the
 * heading facing right and low word facing left), places the shot at the player plus (dx, dy)
 * (dx mirrored facing left), picks the script for the weapon type and level (the charged one
 * when the charge word reached $2B00, which it clears), and runs it on g_SpawnCtx with the
 * player's sprite attributes and owner id pad + 2. The registers the spawn ran with go through
 * VM4_Next (every register: its own contract ends at its computed NEXT); the movem around it
 * keeps a2-a6. */
static void fire(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6,
                 uint32_t *d7, uint32_t *a1, uint32_t *a2, uint32_t a3, uint32_t a4, const Vm2Sprite *spr,
                 const Vm2Obj *self)
{
    (void)a4;
    *d2 = 0;
    PTR(const u16) ops;
    ops.v = *a2;
    uint16_t dx = *ops++;
    uint16_t dy = *ops++;
    uint32_t heading = ((uint32_t)(uint16_t)ops[0] << 16) | (uint16_t)ops[1];
    ops += 2;
    *a2 = ops.raw();
    if (spr->attr & ATTR_FLIP_H) {
        heading = (heading << 16) | (heading >> 16);
        dx = (uint16_t)-dx;
    }
    uint16_t x = (uint16_t)(dx + (uint16_t)self->x);
    uint16_t y = (uint16_t)(dy + (uint16_t)self->y);
    *d3 = heading & 0xFFFF0000u;
    *d4 = (uint32_t)x << 16;
    *d5 = (uint32_t)y << 16;
    PTR(PlayerRec) rec;
    rec.v = self->player;
    uint16_t index = (uint16_t)((uint16_t)(((uint16_t)rec->weapon_type << 3) + (uint16_t)rec->weapon_level) << 3);
    index = (uint16_t)(index + (uint16_t)*d7);
    uint16_t charge = rec->weapon_charge;
    *d1 = (*d1 & 0xFFFF0000u) | charge;
    rec->weapon_charge = 0;
    if (charge >= 0x2B00)
        index = (uint16_t)(index + 4);
    PTR(const u32) script;
    script.v = Tbl_010AF8 + (uint32_t)(int32_t)(int16_t)index;
    Vm4Obj *ctx = (Vm4Obj *)g_SpawnCtx;
    ctx->attr = (uint16_t)spr->attr;
    uint16_t owner = (uint16_t)(self->owner_id + 2);
    *d0 = (*d0 & 0xFFFF0000u) | owner;
    ctx->owner_id = owner;
    uint32_t r_a0 = rec.raw(), r_a2 = *a2, r_a3 = a3, r_a4 = *script, r_a5 = dk::addr(spr),
             r_a6 = dk::addr(ctx);
    VM4_Next(d0, d1, d2, d3, d4, d5, d6, d7, &r_a0, a1, &r_a2, &r_a3, &r_a4, &r_a5, &r_a6);
}

/* Weapon_FireFlying - fires the player's weapon in the stage 2 flying section: Weapon_Fire's body
 * with the flying half ($C0) of Tbl_010AF8.
 * In:  as Weapon_Fire
 * Out: as Weapon_Fire; d7 low word = $C0
 * Evidence: move.w #$C0,d7 / bra.s $010A86; native callback (a2 op $2CBC) at $4D05C and $4FB98
 */
void Weapon_FireFlying(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5,
                       uint32_t *d6, uint32_t *d7, uint32_t *a1, uint32_t *a2, uint32_t a3, uint32_t a4,
                       const Vm2Sprite *spr, const Vm2Obj *self)
{
    *d7 = (*d7 & 0xFFFF0000u) | 0xC0;
    fire(d0, d1, d2, d3, d4, d5, d6, d7, a1, a2, a3, a4, spr, self);
}

/* Weapon_Fire - native op of the player scripts: fires the player's weapon (the shot script for
 * its type, level and charge) at the player's position plus the operands' offset.
 * In:  self = the player object (a6: +$1E record, +$58 pad); spr = its sprite block (a5: +$38
 *      attr, facing); script = the operands (a2: word dx, word dy, long headings)
 * Out: a2 past the 8 operand bytes; d0-d5/d7 as the shot script's run left them
 * Evidence: moveq #0,d7 / moveq #0,d2 / moveq #0,d4 / moveq #0,d5 / move.w (a2)+,d4 / move.w (a2)+,d5 /
 *           move.l (a2)+,d3 / btst #3,$38(a5) / swap d3 / neg.w d4 / add.w $3E(a6),d4 / add.w
 *           $42(a6),d5 / clr.w d3 / swap d4 / swap d5 / the index from $28/$2A of $1E(a6) / cmpi.w
 *           #$2B00 / bcs / addq.w #4 / movem.l a2-a6 / movea.l Tbl_010AF8(pc,d0.w),a4 / lea
 *           g_SpawnCtx,a6 / jsr VM4_Next; native callback at 50 sites of the player scripts
 */
void Weapon_Fire(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6,
                 uint32_t *d7, uint32_t *a1, uint32_t *a2, uint32_t a3, uint32_t a4, const Vm2Sprite *spr,
                 const Vm2Obj *self)
{
    *d7 = 0;
    fire(d0, d1, d2, d3, d4, d5, d6, d7, a1, a2, a3, a4, spr, self);
}
