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

/* Fx_SpawnAtOfsWithAngle - the shared body of the aimed spawn callbacks: the new objects start at
 * the object's position plus the script's offset, mirrored by the sprite's flips, radius 0,
 * heading d3; then Fx_RunScriptArg runs the a4 script that follows.
 * In:  d3 = the heading (16.16); a6 = the object; a5 = its sprite block; a2 = the script: dc.w dx,
 *      dy, dc.l the a4 script
 * Out: d0 = dy as negated or not (its low word); d2 = 0; d4/d5 = (x + dx, y + dy) << 16; a2 at the
 *      dc.l; then Fx_RunScriptArg
 * Evidence: moveq #0,d2 / moveq #0,d4 / moveq #0,d5 / move.w $3E(a6),d4 / move.w $42(a6),d5 /
 *           move.w (a2)+,d0 / btst #3,$38(a5) / neg.w d0 / add.w d0,d4 / ... btst #4 ... / swap d4 /
 *           swap d5 / bra.w Fx_RunScriptArg; entered from Fx_SpawnAimedN, Fx_SpawnAimed, Fx_SpawnAtOfs
 */
void Fx_SpawnAtOfsWithAngle(uint32_t *d0, uint32_t d1, uint32_t *d2, uint32_t d3, uint32_t *d4, uint32_t *d5,
                            uint32_t d6, uint32_t d7, uint32_t a1, uint32_t *a2, uint32_t a3, uint32_t a4, uint32_t a5,
                            uint32_t a6)
{
    (void)d1, (void)d3, (void)d6, (void)d7, (void)a1, (void)a3, (void)a4;
    const Vm2Obj *self = ptr<const Vm2Obj>(a6);
    const Vm2Sprite *spr = ptr<const Vm2Sprite>(a5);
    const u16 *ofs = ptr<const u16>(*a2);
    *d2 = 0;
    uint16_t dx = ofs[0];
    if ((uint8_t)(spr->attr >> 8) & FX_ATTR_HI_HFLIP)
        dx = (uint16_t)-dx;
    uint16_t dy = ofs[1];
    if ((uint8_t)(spr->attr >> 8) & FX_ATTR_HI_VFLIP)
        dy = (uint16_t)-dy;
    *d0 = (*d0 & 0xFFFF0000u) | dy;
    *d4 = (uint32_t)(uint16_t)((uint16_t)self->x + dx) << 16;
    *d5 = (uint32_t)(uint16_t)((uint16_t)self->y + dy) << 16;
    *a2 += 4;
    dk::jump(at_Fx_RunScriptArg);
}

/* Fx_SpawnAtOfs - native callback: runs an a4 spawn script at the object's position plus an offset
 * mirrored by its flips, heading 0 (Fx_SpawnAtOfsWithAngle with d3 = 0).
 * In:  a6 = the object; a5 = its sprite block; a2 = the script: dc.w dx, dy, dc.l the a4 script
 * Out: d3 = 0; then Fx_SpawnAtOfsWithAngle's
 * Evidence: moveq #0,d3, falling into Fx_SpawnAtOfsWithAngle; script uses after Op2_CallNative at
 *           $41EB4, $421F6, $45AFE (dx = 0, dy = -$18, then script $45B8C)
 */
void Fx_SpawnAtOfs(uint32_t *d0, uint32_t d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t d6,
                   uint32_t d7, uint32_t a1, uint32_t *a2, uint32_t a3, uint32_t a4, uint32_t a5, uint32_t a6)
{
    *d3 = 0;
    Fx_SpawnAtOfsWithAngle(d0, d1, d2, *d3, d4, d5, d6, d7, a1, a2, a3, a4, a5, a6);
}
