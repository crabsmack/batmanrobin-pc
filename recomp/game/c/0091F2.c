#include "game.h"

/* a 68000 address held in a register, as a pointer (PTR(T): the 68000 value kept) */
template <class T> static T *ptr(uint32_t a)
{
    PTR(T) p;
    p.v = a;
    return p;
}

/* Fx_RunScript - runs a4 spawn script a0 on the spawn context g_SpawnCtx with the parent's velocity
 * and sprite attribute, so the objects it spawns start from d2-d5 and fly on as the parent did.
 * In:  a0 = the a4 script; d2/d3/d4/d5 = the new objects' radius, heading, x, y (16.16); a3 = the
 *      parent's motion block (+$18/+$1C velocity); a5 = its sprite block (+$38 attr)
 * Out: g_SpawnCtx: radius/heading speeds 0, x/y velocity = the parent's, attr = the parent's;
 *      d0-d7 as the script's run leaves them; a1-a6 kept (movem)
 * Evidence: movem.l a1-a6,-(a7) / movea.l a0,a4 / lea g_SpawnCtx,a6 / clr.l $36(a6) / clr.l
 *           $3A(a6) / move.l $18(a3),$3E(a6) / move.l $1C(a3),$42(a6) / move.w $38(a5),$22(a6) /
 *           bsr.w VM4_Next / movem.l (a7)+,a1-a6 / rts; callers $1088A, $10D12, $1A34E, $3CCD4
 */
void Fx_RunScript(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6,
                  uint32_t *d7, uint32_t a0, uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4, uint32_t a5, uint32_t a6)
{
    dk::push_long(a6);   /* movem.l a1-a6,-(a7) */
    dk::push_long(a5);
    dk::push_long(a4);
    dk::push_long(a3);
    dk::push_long(a2);
    dk::push_long(a1);
    const Vm2Motion *parent = ptr<const Vm2Motion>(a3);
    const Vm2Sprite *spr = ptr<const Vm2Sprite>(a5);
    Vm4Obj *ctx = (Vm4Obj *)g_SpawnCtx;
    ctx->vel_radius = 0;
    ctx->vel_heading = 0;
    ctx->vel_x = parent->vel_x;
    ctx->vel_y = parent->vel_y;
    ctx->attr = spr->attr;

    uint32_t r0 = a0, r1 = a1, r2 = a2, r3 = a3, r4 = a0, r5 = a5, r6 = dk::addr(ctx);
    VM4_Next(d0, d1, d2, d3, d4, d5, d6, d7, &r0, &r1, &r2, &r3, &r4, &r5, &r6);
    for (int i = 0; i < 6; i++)   /* movem.l (a7)+,a1-a6 */
        (void)dk::pop_long();
}

/* Fx_SpawnAtScreenCentre - runs the a4 spawn script at a4 with the new objects at the screen centre
 * (camera + ($A0, $70)), radius and heading 0, on the scene's sprite attribute
 * (Fx_RunScriptNoParent, which it falls into).
 * In:  a4 = the a4 script
 * Out: d2 = d3 = 0, d4/d5 = g_CameraX + $A00000 / g_CameraY + $700000; then Fx_RunScriptNoParent
 * Evidence: moveq #0,d2 / moveq #0,d3 / move.l g_CameraX,d4 / move.l g_CameraY,d5 / addi.l #$A00000,d4
 *           / addi.l #$700000,d5, falling into Fx_RunScriptNoParent (the next file); caller $20D76
 */
void Fx_SpawnAtScreenCentre(uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *a0, uint32_t *a4,
                            uint32_t *a6)
{
    (void)a0, (void)a4, (void)a6;
    *d2 = 0;
    *d3 = 0;
    *d4 = (uint32_t)g_CameraX + (160u << 16);   /* $A00000: 160 px, 16.16 */
    *d5 = (uint32_t)g_CameraY + (112u << 16);   /* $700000: 112 px */
    dk::jump(at_Fx_RunScriptNoParent);
}
