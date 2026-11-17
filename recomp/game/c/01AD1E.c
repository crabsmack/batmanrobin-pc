#include "game.h"

/* the a2 script the spawned object runs (ROM data right after this spawner's code; batmanrobin.asm) */
static constexpr uint32_t Scr2_01AD2A = 0x01AD2Au;

/* Enemy_Brute_Spawn - spawns the big sack-carrying brute that walks in on the ground: palette
 * line 3, energy $D0, attack loop entered at $1B0CA
 * In:  d4/d5 = x/y (trigger or wave entry)
 * Out: script = Scr2_01AD2A, then Obj_SpawnEnemy's (d0 = 0 and a6 = the new object; Z clear
 *      when the pool is short)
 * Evidence: lea Scr2_01AD2A,a2 / jmp Obj_SpawnEnemy; Scr2_01AD2A: same prologue as
 *           Enemy_Brute_SpawnFromSign but coroutine 0 = $1B032 (GosubVector2Plus 0, anim $382,
 *           floor check); referenced from Tbl_0166EA (+$4E8, +$662); TAS f2118-f2422; brute
 *           body Scr2_01AF1A drawn as the purple strongman with a sack (shots f2940, f3000)
 */
void Enemy_Brute_Spawn(uint16_t *d1, uint16_t d4, uint16_t d5, uint32_t *d7, uint32_t *script,
                       uint32_t *a3, uint32_t *a4, uint32_t *a6)
{
    *script = Scr2_01AD2A;
    dk::jump(at_Obj_SpawnEnemy);
}
