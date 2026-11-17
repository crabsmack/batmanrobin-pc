#include "game.h"

/* the object's a2 script (ROM, right after this spawner's code) */
static constexpr uint32_t Scr2_020628 = 0x020628u;

/* Stage1_SpawnUrn - trigger callback of the stage 1 Gem Expo section: spawns a breakable urn on a
 * pedestal (Scr2_020628: energy $20, one frame, bursts into an effect and gives its score to the hitter)
 * In:  d4/d5 = the trigger's x/y (the entry's top byte = its drop)
 * Out: script (a2) = Scr2_020628, then as Obj_SpawnEnemy (d0 = 0 and a6 = the object, Z set; d0
 *      nonzero when the pool is short)
 * Evidence: lea Scr2_020628,a2 / jmp Obj_SpawnEnemy; dc.l entries in the section-2 trigger table Tbl_0163BA ($0163D4 ...;
 *           scene $1FC50); TAS f6399-f8887; RAM f6420: a Scr2_020628 object at x 378 y 173 (camera
 *           104) = the urn at screen x 274 in shot f6420
 */
void Stage1_SpawnUrn(uint16_t *d1, uint16_t d4, uint16_t d5, uint32_t *a0, uint32_t *a4, uint32_t *a5, uint32_t *a6,
                     uint32_t *script)
{
    *script = Scr2_020628;
    dk::jump(at_Obj_SpawnEnemy);
}
