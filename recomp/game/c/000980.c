#include "game.h"

/* Obj_Destroy - tears down the running script object: tells its spawn group a member is gone
 * (spawn action 1), takes its population weight off g_ObjPopulation (not below 0), releases its
 * sprite frames and both box lists, then frees it with its blocks, unlinking it first when it is
 * on an object list.
 * In:  a6 = the object, a4 = its VM block (box lists), a5 = its sprite block; the rest pass
 *      through to the callees
 * Out: d0 = the weight, d7 / d1-d6 / a1-a2 as the box and frame calls leave them; then
 *      Obj_UnlinkFreeWithBlocks (prev link set) or Obj_FreeWithBlocks, jumped to (their flags)
 * Evidence: cmpi.b #1,$19(a6) / bsr SpawnAction_GroupMemberGone; sub.w d0,(g_ObjPopulation) / bcc /
 *           clr.w; bsr Obj_ReleaseFrames; moveq #0,d7 / bsr Obj_SetBodyBoxes / Obj_SetAttackBoxes;
 *           tst.w $4(a6) / jmp; callers Op2_Kill, Obj_DestroyOther, Obj_DestroyLoaded; 2.2k TAS frames
 */
void Obj_Destroy(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6,
                 uint32_t *d7, uint32_t *a0, uint32_t *a1, uint32_t *a2, uint32_t a4, uint32_t a5, uint32_t a6)
{
    PTR(Vm2Obj) self;
    self.v = a6;
    PTR(Vm2Sprite) spr;
    spr.v = a5;

    if (self->spawn_action == 1) {
        *d1 = (*d1 & 0xFFFF0000u) | self->spawn_arg;
        SpawnAction_GroupMemberGone((uint16_t)*d1);
    }

    *d0 = self->population;
    uint16_t pop = g_ObjPopulation;
    g_ObjPopulation = pop >= (uint16_t)*d0 ? (uint16_t)(pop - *d0) : 0;

    uint16_t w0 = (uint16_t)*d0, w2 = (uint16_t)*d2, w7 = (uint16_t)*d7;
    u16 *slot = 0;
    Obj_ReleaseFrames(&w0, &w2, &w7, &slot, spr);
    *d0 = (*d0 & 0xFFFF0000u) | w0;
    *d2 = (*d2 & 0xFFFF0000u) | w2;
    *a1 = dk::addr(slot);

    /* moveq #0,d7: no boxes, which frees each list's pool blocks */
    uint16_t none = 0;
    Obj_SetBodyBoxes(d0, d1, d2, d3, d4, d5, d6, &none, a1, a2, a4, a6);
    *d7 = none;
    none = 0;
    dk::Flags boxes_ccr{};   /* its condition codes stay in the CPU too */
    Obj_SetAttackBoxes(d0, d1, d2, d3, d4, d5, d6, &none, a1, a2, a4, a6, &boxes_ccr);
    *d7 = none;

    dk::jump(self->prev != 0 ? at_Obj_UnlinkFreeWithBlocks : at_Obj_FreeWithBlocks);
}
