#include "game.h"

/* Fx_RunScriptNoParent - runs the a4 spawn script at a4 on g_SpawnCtx with the scene's default
 * sprite attribute; the context's velocities stay as they are (no parent to inherit from).
 * In:  a4 = the a4 script; d2-d5 = the new objects' radius, heading, x, y (16.16)
 * Out: a6 = g_SpawnCtx, its attr = g_SceneSprAttr; then VM4_Next (the script returns from its end
 *      op to this function's caller)
 * Evidence: lea g_SpawnCtx,a6 / move.w g_SceneSprAttr,$22(a6) / bra.w VM4_Next; callers $13288,
 *           $1FECC, $2417E. Prototype: every register (it was &a0,&a4,&a6): the script run that the
 *           jump starts changes any register, and a C caller's call must hand them all back
 */
void Fx_RunScriptNoParent(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5,
                          uint32_t *d6, uint32_t *d7, uint32_t *a0, uint32_t *a1, uint32_t *a2, uint32_t *a3,
                          uint32_t *a4, uint32_t *a5, uint32_t *a6)
{
    (void)d0, (void)d1, (void)d2, (void)d3, (void)d4, (void)d5, (void)d6, (void)d7, (void)a0, (void)a1, (void)a2,
        (void)a3, (void)a4, (void)a5;
    Vm4Obj *ctx = (Vm4Obj *)g_SpawnCtx;
    *a6 = dk::addr(ctx);
    ctx->attr = g_SceneSprAttr;
    dk::jump(at_VM4_Next);
}
