#include "game.h"

/* the pickup's a2 script (ROM data right after this spawner's code; batmanrobin.asm) */
static constexpr uint32_t Scr2_013996 = 0x013996u;

/* Item_SpawnWeapon2 - spawn action 12: drops the weapon pickup of weapon type 2 (item value 3,
 * table $13C6E)
 * In:  x/y = where the pickup appears (SpawnAction_Run passes the dying object's position)
 * Out: script = Scr2_013996, then Obj_SpawnEnemy: d0 = 0 and a6 = the pickup, d0 nonzero when
 *      the pool is short (the other registers pass through unchanged)
 * Evidence: lea Scr2_013996,a2 / jmp Obj_SpawnEnemy; Tbl_012EF0 entry 12; Scr2_013996 = coroutine
 *           $133AE, Item_WeaponItemInit with table $13C6E (0, 3: fixed value 3), goto $13B6C (touch
 *           -> $13C40); 2 TAS frames (125018, 127253)
 */
void Item_SpawnWeapon2(uint32_t *result, uint16_t *d1, uint16_t x, uint16_t y, uint32_t *d7, uint32_t a4, uint32_t a6, uint32_t *script)
{
    *script = Scr2_013996;
    dk::jump(at_Obj_SpawnEnemy);
}
