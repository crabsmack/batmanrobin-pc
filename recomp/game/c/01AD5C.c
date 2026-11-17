#include "game.h"

/* the a2 script the spawned object runs (ROM data right after this spawner's code; batmanrobin.asm) */
static constexpr uint32_t Scr2_01AD68 = 0x01AD68u;

/* Enemy_Brute_SpawnTough - spawns the tougher brute: palette line 2, energy $120, walks in,
 * attack loop at $1B0CA
 * In:  d4/d5 = x/y (wave script Tbl_0174F0)
 * Out: script = Scr2_01AD68, then Obj_SpawnEnemy's (d0 = 0 and a6 = the new object; Z clear
 *      when the pool is short)
 * Evidence: lea Scr2_01AD68,a2 / jmp Obj_SpawnEnemy; Scr2_01AD68 = ModSprAttr $98/$40 (palette
 *           2), SetEnergy $120, coroutine 0 $1B032; RAM f9420: an Scr2_01AF1A object with
 *           energy $120; TAS f9404
 */
void Enemy_Brute_SpawnTough(uint16_t *d1, uint16_t d4, uint16_t d5, uint32_t *d7, uint32_t *script,
                            uint32_t *a3, uint32_t *a4, uint32_t *a6)
{
    *script = Scr2_01AD68;
    dk::jump(at_Obj_SpawnEnemy);
}
