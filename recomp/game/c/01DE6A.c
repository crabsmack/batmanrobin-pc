#include "game.h"

/* the 11 thrown objects' a2 scripts, by kind (dc.l; ROM) */
static constexpr uint32_t Tbl_01DE8A = 0x01DE8Au;
static constexpr uint16_t THROWN_BLOCKS = 5;

/* Boss_JokerBalloon_SpawnThrown - spawns a 5-block a2 object (VM2_RunObject) running script
 * Tbl_01DE8A[kind] (11 kinds, each thrown left at its own cruise x with an arcing Op2_JumpAnim) at
 * d4/d5 (16.16)
 * In:  d4/d5 = x/y longs (16.16; Obj_Spawn sees their low words), d6 = the kind x 4
 * Out: d7 = 5; Obj_Spawn's d0/d1/a0/a3-a6 (d0 = 0 and a4/a6 = the new object's script block and
 *      object, nonzero d0 when the pool is short); after a spawn its resume script and its x/y longs
 * Evidence: lea VM2_RunObject,a0 / moveq #5,d7 / jsr Obj_Spawn / bne / move.l Tbl_01DE8A(pc,d6.w),
 *           $2A(a4) / move.l d4,$3E(a6) / move.l d5,$42(a6); bsr from Boss_JokerBalloon_ThrowByArg;
 *           no corpus run reaches it (crane-crash 2026-10-30); 01DCFE.syn checks it through its
 *           caller. Prototype: no flags out (as its caller's)
 */
void Boss_JokerBalloon_SpawnThrown(uint32_t *d0, uint16_t *d1, uint32_t d4, uint32_t d5, uint16_t d6, uint32_t *d7,
                                   uint32_t *a0, uint32_t *a3, uint32_t *a4, uint32_t *a5, uint32_t *a6)
{
    *a0 = at_VM2_RunObject;
    uint16_t blocks = THROWN_BLOCKS;
    Obj_Spawn(d0, d1, (uint16_t)d4, (uint16_t)d5, &blocks, a0, a3, a4, a5, a6);
    *d7 = blocks;                                          /* moveq #5,d7: the whole register */
    if (*d0 != 0)                                          /* bne: the pool was short */
        return;
    PTR(const u32) kinds;
    kinds.v = Tbl_01DE8A + (uint32_t)(int32_t)(int16_t)d6;
    PTR(Vm2Block) vm;
    vm.v = *a4;
    vm->resume.v = *kinds;
    PTR(Vm2Obj) obj;
    obj.v = *a6;
    obj->x = (int16_t)(d4 >> 16);
    obj->x_frac = (uint16_t)d4;
    obj->y = (int16_t)(d5 >> 16);
    obj->y_frac = (uint16_t)d5;
}
