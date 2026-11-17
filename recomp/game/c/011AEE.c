#include "game.h"

/* BossMeter_Spawn - spawns the boss energy meter object (BossMeter_SpawnObject) and keeps its
 * handle in g_BossMeterObj; a failed spawn (the pool short) returns its result without storing.
 * In:  d1/d4/d5/d7/a4/a6 for the spawn (Obj_SpawnEnemy)
 * Out: d0 = 0 and g_BossMeterObj = the meter, or d0 = -1 (the flags tst.l d0 either way: the
 *      spawn's moveq or this one); a2-a6 preserved, a0 = the new object
 * Evidence: movem.l a2-a6,-(a7) / bsr.w BossMeter_SpawnObject / movea.l a6,a0 / movem.l
 *           (a7)+,a2-a6 / bne / move.w a0,(g_BossMeterObj).w / moveq #0,d0; reached from
 *           BossMeter_Show when no meter exists; 13 times in the TAS (4540-128791)
 */
void BossMeter_Spawn(uint32_t *d0, uint16_t *d1, uint16_t d4, uint16_t d5, uint32_t *d7, uint32_t a4, uint32_t a6)
{
    /* movem.l a2-a6,-(a7): a2/a3/a5 are not inputs (nothing reads them back) */
    dk::push_long(a6);
    dk::push_long(0);
    dk::push_long(a4);
    dk::push_long(0);
    dk::push_long(0);
    uint32_t spawned = a6;
    BossMeter_SpawnObject(d0, d1, d4, d5, d7, a4, &spawned);
    for (int i = 0; i < 5; i++)
        dk::pop_long();
    if (*d0 != 0)   /* bne: Obj_SpawnEnemy's d0 = -1 (its flags are tst.l d0) */
        return;
    g_BossMeterObj = (uint16_t)spawned;
    *d0 = 0;
}
