#include "game.h"

/* the pickup's a2 script (ROM data right after this spawner's code; batmanrobin.asm) */
static constexpr uint32_t Scr2_01397C = 0x01397Cu;

/* Item_SpawnWeapon1 - spawn action 9: drops the weapon type 1 pickup (item value 2, table $13C68)
 * where the object died (motion coroutine $133AE, then the weapon pickup body $13B6C)
 * In:  x/y = where the pickup appears (SpawnAction_Run passes the dying object's position)
 * Out: script = Scr2_01397C, then Obj_SpawnEnemy: d0 = 0 and a6 = the pickup, d0 nonzero when the
 *      pool is short (the other registers pass through unchanged)
 * Evidence: lea Scr2_01397C,a2 / jmp Obj_SpawnEnemy; Tbl_012EF0 entry 9; raw until crane-crash
 *           (2026-10-30): no corpus run spawns it; 013970.syn checks it
 */
void Item_SpawnWeapon1(uint32_t *result, uint16_t *d1, uint16_t x, uint16_t y, uint32_t *d7, uint32_t a4, uint32_t a6, uint32_t *script)
{
    *script = Scr2_01397C;
    dk::jump(at_Obj_SpawnEnemy);
}
