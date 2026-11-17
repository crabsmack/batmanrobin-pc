#include "game.h"

/* the pickup's a2 script (ROM data right after this spawner's code; batmanrobin.asm) */
static constexpr uint32_t Scr2_013A98 = 0x013A98u;

/* Item_SpawnWeaponDownDrifting - spawn action 5: the level-down weapon pickup (value 0, table
 * $13C5C) for the flying sections: drifts sideways (motion coroutine $133CA, body $13BC2)
 * In:  x/y = where the pickup appears (SpawnAction_Run passes the dying object's position)
 * Out: script = Scr2_013A98, then Obj_SpawnEnemy: d0 = 0 and a6 = the pickup, d0 nonzero when the
 *      pool is short (the other registers pass through unchanged)
 * Evidence: lea Scr2_013A98,a2 / jmp Obj_SpawnEnemy; Tbl_012EF0 entry 5; raw until crane-crash
 *           (2026-10-30): no corpus run spawns it; 013A8C.syn checks it
 */
void Item_SpawnWeaponDownDrifting(uint32_t *result, uint16_t *d1, uint16_t x, uint16_t y, uint32_t *d7, uint32_t a4, uint32_t a6, uint32_t *script)
{
    *script = Scr2_013A98;
    dk::jump(at_Obj_SpawnEnemy);
}
