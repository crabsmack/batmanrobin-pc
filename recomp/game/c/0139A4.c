#include "game.h"

/* the pickup's a2 script (ROM data right after this spawner's code; batmanrobin.asm) */
static constexpr uint32_t Scr2_0139B0 = 0x0139B0u;

/* Item_SpawnWeaponRandom - spawn action 15: drops a weapon pickup of a random weapon type 0-2 (item
 * value from table $13C74)
 * In:  x/y = where the pickup appears (SpawnAction_Run passes the dying object's position)
 * Out: script = Scr2_0139B0, then Obj_SpawnEnemy: d0 = 0 and a6 = the pickup, d0 nonzero when
 *      the pool is short (the other registers pass through unchanged)
 * Evidence: lea Scr2_0139B0,a2 / jmp Obj_SpawnEnemy; Tbl_012EF0 entry 15; Scr2_0139B0 = coroutine
 *           $133AE, Item_WeaponItemInit with table $13C74 (3 values 1,2,3); 8 TAS frames
 *           (603-111706)
 */
void Item_SpawnWeaponRandom(uint32_t *result, uint16_t *d1, uint16_t x, uint16_t y, uint32_t *d7, uint32_t a4, uint32_t a6, uint32_t *script)
{
    *script = Scr2_0139B0;
    dk::jump(at_Obj_SpawnEnemy);
}
