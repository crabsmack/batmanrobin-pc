#include "game.h"

/* Obj_SpawnScriptedOrQuit - spawns a 4-block VM2_RunObject object at d4/d5 (Obj_SpawnScripted),
 * returning to the caller's caller when the pool is short.
 * In:  d4/d5 = x/y
 * Out: d0 = 0, a6 = the object, a3-a5 its blocks (the caller sets the script and owner id); the
 *      pool short: one return address dropped
 * Evidence: bsr.w Obj_SpawnScripted / bne.s (addq.l #4,a7 / rts) / moveq #0,d0 / rts; callers
 *           $1F480, $1F634, $1F75C, $1F882. The flags: Obj_SpawnScripted's (its d0 says which:
 *           0 with Z set, -1 with Z clear), and moveq #0 leaves the same
 */
void Obj_SpawnScriptedOrQuit(uint32_t *d0, uint16_t *d1, uint16_t d4, uint16_t d5, uint32_t *d7, uint32_t *a0,
                             uint32_t *a3, uint32_t *a4, uint32_t *a5, uint32_t *a6)
{
    Obj_SpawnScripted(d0, d1, d4, d5, d7, a0, a3, a4, a5, a6);
    if (*d0 != 0) {   /* the pool is short */
        dk::drop_return();
        return;
    }
    *d0 = 0;
}
