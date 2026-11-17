#include "game.h"

/* Obj_SpawnAfter - allocates a d7-block object with handler a0 linked right after the running object
 * (Obj_NewMultiAfter) and initialises it at d4/d5 (Obj_InitSpawned).
 * In:  a0 = the handler; d7 = blocks; d4/d5 = x/y; a6 = the running object (the new one follows it)
 * Out: d0 = 0, a6 = the object, a3-a5 its blocks; the pool short: d0 = -1, Z clear
 * Evidence: movem.l a1-a2,-(a7) / bsr.w Obj_NewMultiAfter / movem.l (a7)+,a1-a2 / beq.w Obj_InitSpawned /
 *           rts, Obj_SpawnUnlinked's shape; bsr target of Obj_SpawnEnemyAfter ($008E28; the raw $008E3C,
 *           $008EC4, $008EDE too); no corpus run reaches it (raw-stubs 2026-10-30); the contract and the
 *           C are Obj_SpawnUnlinked's: the two saved longs are dead once restored; 008F3C.syn checks it
 */
void Obj_SpawnAfter(uint32_t *d0, uint16_t *d1, uint16_t d4, uint16_t d5, uint16_t *d7, uint32_t *a0, uint32_t *a3,
                    uint32_t *a4, uint32_t *a5, uint32_t *a6)
{
    (void)d4, (void)d5, (void)a3, (void)a4, (void)a5;
    dk::push_long(0);   /* movem.l a1-a2,-(a7) */
    dk::push_long(0);
    uint32_t a1 = 0, a2 = 0;
    Obj_NewMultiAfter(d0, d1, d7, a0, &a1, &a2, a6);
    (void)dk::pop_long();   /* movem.l (a7)+,a1-a2 */
    (void)dk::pop_long();
    if (*d0 == 0)
        dk::jump(at_Obj_InitSpawned);
}
