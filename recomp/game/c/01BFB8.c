#include "game.h"

/* the a2 script the spawned object runs (ROM data right after this spawner's code; batmanrobin.asm) */
static constexpr uint32_t Scr2_01BFC4 = 0x01BFC4u;

/* Enemy_Gunman_SpawnDropIn - spawns the trench-coat gunman in palette line 3 (energy $75) that
 * drops in from above before taking position
 * In:  d4/d5 = x/y (wave script Tbl_0174F0 +$69A..+$6AA)
 * Out: script = Scr2_01BFC4, then Obj_SpawnEnemy's (d0 = 0 and a6 = the new object; Z clear
 *      when the pool is short)
 * Evidence: lea Scr2_01BFC4,a2 / jmp Obj_SpawnEnemy; Scr2_01BFC4: SetEnergy $75, coroutine 0 =
 *           $1C17C (op $1CDC fall $46,$140, anim $AE, floor check); gunmen wait above the
 *           screen (y < 0) in RAM f2220-f2940; shot f2460 (gunman firing on the awning, tagged
 *           Scr2_01BFC4); TAS f13277-f15671
 */
void Enemy_Gunman_SpawnDropIn(uint16_t *d1, uint16_t d4, uint16_t d5, uint32_t *d7,
                              uint32_t *script, uint32_t *a3, uint32_t *a4, uint32_t *a6)
{
    *script = Scr2_01BFC4;
    dk::jump(at_Obj_SpawnEnemy);
}
