#include "game.h"

/* Obj_SpawnScriptedUnlinked - spawns a 4-block VM2_RunObject object at d4/d5 without linking it on
 * the main object list (Obj_SpawnUnlinked with handler $7D2 and d7 = 4).
 * In:  d4/d5 = x/y
 * Out: a0 = VM2_RunObject, d7 = 4; then Obj_SpawnUnlinked's (d0 = 0, a6 = the object, a3-a5 its
 *      blocks; Z clear when the pool is short)
 * Evidence: lea ($7D2).w,a0 / moveq #4,d7, falling into Obj_SpawnUnlinked (the next file); caller
 *           $29250. Prototype: d0, d7 and a0 outputs (measured clobbers): the jump hands the
 *           handler and the block count to Obj_SpawnUnlinked, and d0 comes back from it
 */
void Obj_SpawnScriptedUnlinked(uint32_t *d0, uint16_t *d1, uint16_t d4, uint16_t d5, uint32_t *d7, uint32_t *a0,
                               uint32_t *a3, uint32_t *a4, uint32_t *a5, uint32_t *a6)
{
    (void)d0, (void)d1, (void)d4, (void)d5, (void)a3, (void)a4, (void)a5, (void)a6;
    *a0 = at_VM2_RunObject;
    *d7 = 4;
    dk::jump(at_Obj_SpawnUnlinked);
}
