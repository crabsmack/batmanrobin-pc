#include "game.h"

/* a 68000 address held in a register, as a pointer (PTR(T): the 68000 value kept) */
template <class T> static T *ptr(uint32_t a)
{
    PTR(T) p;
    p.v = a;
    return p;
}

/* the sprite attribute's high byte (the byte btst #3 / #4 test): bit 3 = H flip, bit 4 = V flip */
enum { FX_ATTR_HI_HFLIP = 1 << 3, FX_ATTR_HI_VFLIP = 1 << 4 };

/* Fx_SpawnKnockback - native callback: runs an a4 spawn script at a mirrored offset, the spawned
 * objects starting with 3/4 of the object's knockback impulse as their velocity (debris that flies
 * the way the hit pushed it).
 * In:  a6 = the object; a3 = its motion block (+2/+6 the x/y impulses, 16.16); a5 = its sprite
 *      block; a2 = the script: dc.w dx, dy, dc.l the a4 script
 * Out: d2 = d3 = 0 (radius, heading), d4/d5 = (x + dx, y + dy) << 16 for the new objects;
 *      g_SpawnCtx: radius/heading speeds 0, attr = the object's, x/y velocity = impulse - impulse/4;
 *      a2 past the dc.l; d0-d7 as the a4 script's run leaves them; a1-a6 kept
 * Evidence: moveq #0,d3/d2/d4/d5 / move.w $3E(a6),d4 / $42(a6),d5 / the mirrored dx, dy / swap /
 *           movea.l (a2)+,a0 / movem.l a1-a6,-(a7) / movea.l a0,a4 / lea g_SpawnCtx,a6 / clr.l
 *           $36(a6) / clr.l $3A(a6) / move.w $38(a5),$22(a6) / movem.l $2(a3),d0-d1 / d6 = d0 asr 2,
 *           d7 = d1 asr 2 / sub / movem.l d0-d1,$3E(a6) / bsr.w VM4_Next / movem.l (a7)+,a1-a6;
 *           one script use $33C22
 */
void Fx_SpawnKnockback(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5,
                       uint32_t *d6, uint32_t *d7, uint32_t a1, uint32_t *a2, uint32_t a3, uint32_t a4, uint32_t a5,
                       uint32_t a6)
{
    const Vm2Obj *self = ptr<const Vm2Obj>(a6);
    const Vm2Motion *mot = ptr<const Vm2Motion>(a3);
    const Vm2Sprite *spr = ptr<const Vm2Sprite>(a5);
    *d3 = 0;
    *d2 = 0;
    const u16 *ofs = ptr<const u16>(*a2);
    uint16_t dx = ofs[0];
    if ((uint8_t)(spr->attr >> 8) & FX_ATTR_HI_HFLIP)
        dx = (uint16_t)-dx;
    uint16_t dy = ofs[1];
    if ((uint8_t)(spr->attr >> 8) & FX_ATTR_HI_VFLIP)
        dy = (uint16_t)-dy;
    *d4 = (uint32_t)(uint16_t)((uint16_t)self->x + dx) << 16;
    *d5 = (uint32_t)(uint16_t)((uint16_t)self->y + dy) << 16;
    uint32_t script = *ptr<const u32>(*a2 + 4);
    *a2 += 8;

    dk::push_long(a6);   /* movem.l a1-a6,-(a7) */
    dk::push_long(a5);
    dk::push_long(a4);
    dk::push_long(a3);
    dk::push_long(*a2);
    dk::push_long(a1);
    Vm4Obj *ctx = (Vm4Obj *)g_SpawnCtx;
    ctx->vel_radius = 0;
    ctx->vel_heading = 0;
    ctx->attr = spr->attr;
    int32_t ix = mot->impulse_x, iy = mot->impulse_y;
    *d6 = (uint32_t)(ix >> 2);   /* asr.l #2 */
    *d7 = (uint32_t)(iy >> 2);
    *d0 = (uint32_t)ix - *d6;
    *d1 = (uint32_t)iy - *d7;
    ctx->vel_x = (int32_t)*d0;
    ctx->vel_y = (int32_t)*d1;

    uint32_t r0 = script, r1 = a1, r2 = *a2, r3 = a3, r4 = script, r5 = a5, r6 = dk::addr(ctx);
    VM4_Next(d0, d1, d2, d3, d4, d5, d6, d7, &r0, &r1, &r2, &r3, &r4, &r5, &r6);
    for (int i = 0; i < 6; i++)   /* movem.l (a7)+,a1-a6 */
        (void)dk::pop_long();
}
