#include "game.h"

/* Obj_Spawn - allocates a d7-block object with handler a0 (Obj_NewMulti: linked before
 * g_ObjListTail) and initialises it at d4/d5 (Obj_InitSpawned).
 * In:  a0 = the handler; d7 = blocks; d4/d5 = x/y
 * Out: d0 = 0, a6 = the object, a3-a5 its extra blocks (Obj_InitSpawned); the pool short: d0 = -1,
 *      Z clear (Obj_NewMulti's)
 * Evidence: movem.l a1-a2,-(a7) / bsr.w Obj_NewMulti / movem.l (a7)+,a1-a2 / beq.s Obj_InitSpawned /
 *           rts; every Obj_SpawnEnemy / Obj_SpawnBoss entry, $1E70A. Obj_NewMulti ends moveq #0 or
 *           moveq #-1, so its d0 says which way the beq goes. The two longs the movem saves keep
 *           the stack as deep as the 68000's; their values are dead once restored (a1/a2 are not in
 *           the prototype: the bridge keeps the registers themselves), and Obj_NewMulti never reads
 *           its a1/a2 before writing them (the failure path hands them back unchanged)
 */
void Obj_Spawn(uint32_t *d0, uint16_t *d1, uint16_t d4, uint16_t d5, uint16_t *d7, uint32_t *a0, uint32_t *a3,
               uint32_t *a4, uint32_t *a5, uint32_t *a6)
{
    (void)d4, (void)d5, (void)a3, (void)a4, (void)a5;
    dk::push_long(0);   /* movem.l a1-a2,-(a7) */
    dk::push_long(0);
    uint32_t a1 = 0, a2 = 0;
    Obj_NewMulti(d0, d1, d7, a0, &a1, &a2, a6);
    (void)dk::pop_long();   /* movem.l (a7)+,a1-a2 */
    (void)dk::pop_long();
    if (*d0 == 0)
        dk::jump(at_Obj_InitSpawned);
}
