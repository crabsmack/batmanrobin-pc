#include "game.h"

/* the object's a2 script (ROM, right after this spawner's code) */
static constexpr uint32_t Scr2_020504 = 0x020504u;

/* Stage1_SpawnTrashCan - trigger callback of the stage 1 street: spawns a breakable trash can
 * (Scr2_020504: energy $20, one frame, explodes with sound $15 and gives its score to the hitter)
 * In:  d4/d5 = the trigger's x/y (Trigger_Fire; the entry's top byte becomes the spawn action: its drop)
 * Out: script (a2) = Scr2_020504, then as Obj_SpawnEnemy (d0 = 0 and a6 = the object, Z set; d0
 *      nonzero when the pool is short)
 * Evidence: lea Scr2_020504,a2 / jmp Obj_SpawnEnemy; dc.l entries in the street trigger table Tbl_015F5A ($015F66, $015F82,
 *           ...); TAS f532-f4237; RAM f600: three Scr2_020504 objects at x 302/323/344 y 181, the
 *           three trash cans in shot f600
 */
void Stage1_SpawnTrashCan(uint16_t *d1, uint16_t d4, uint16_t d5, uint32_t *a0, uint32_t *a4, uint32_t *a5, uint32_t *a6,
                          uint32_t *script)
{
    *script = Scr2_020504;
    dk::jump(at_Obj_SpawnEnemy);
}
