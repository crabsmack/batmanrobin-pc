#include "game.h"

/* Obj_SpawnUnlinked - allocates a d7-block object with handler a0 that is not linked on the main
 * object list (Obj_NewMultiUnlinked) and initialises it at d4/d5 (Obj_InitSpawned).
 * In:  a0 = the handler; d7 = blocks; d4/d5 = x/y
 * Out: d0 = 0, a6 = the object, a3-a5 its blocks; the pool short: d0 = -1, Z clear
 * Evidence: movem.l a1-a2,-(a7) / bsr.w Obj_NewMultiUnlinked / movem.l (a7)+,a1-a2 /
 *           beq.w Obj_InitSpawned / rts; the Unlinked spawners and $370DC. As Obj_Spawn: d0 says
 *           which way the beq goes, and the two saved longs are dead once restored
 */
void Obj_SpawnUnlinked(uint32_t *d0, uint16_t *d1, uint16_t d4, uint16_t d5, uint16_t *d7, uint32_t *a0, uint32_t *a3,
                       uint32_t *a4, uint32_t *a5, uint32_t *a6)
{
    (void)d4, (void)d5, (void)a3, (void)a4, (void)a5;
    dk::push_long(0);   /* movem.l a1-a2,-(a7) */
    dk::push_long(0);
    Obj_NewMultiUnlinked(d0, d1, d7, a0, a6);
    (void)dk::pop_long();   /* movem.l (a7)+,a1-a2 */
    (void)dk::pop_long();
    if (*d0 == 0)
        dk::jump(at_Obj_InitSpawned);
}
