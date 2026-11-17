#include "game.h"

/* the pickup's a2 script (ROM data right after this spawner's code; batmanrobin.asm) */
static constexpr uint32_t Scr2_0139CA = 0x0139CAu;

/* Item_SpawnWeaponRandomOrDown - spawn action 18: drops a weapon pickup whose value is random among
 * 0-3 (0 = the level-down value of Item_CollectWeapon)
 * In:  x/y = where the pickup appears (SpawnAction_Run passes the dying object's position)
 * Out: script = Scr2_0139CA, then Obj_SpawnEnemy: d0 = 0 and a6 = the pickup, d0 nonzero when
 *      the pool is short (the other registers pass through unchanged)
 * Evidence: lea Scr2_0139CA,a2 / jmp Obj_SpawnEnemy; Tbl_012EF0 entry 18; Scr2_0139CA = coroutine
 *           $133AE, Item_WeaponItemInit with table $13C7E (4 values 0-3); 2 TAS frames (120197,
 *           121657)
 */
void Item_SpawnWeaponRandomOrDown(uint32_t *result, uint16_t *d1, uint16_t x, uint16_t y, uint32_t *d7, uint32_t a4, uint32_t a6, uint32_t *script)
{
    *script = Scr2_0139CA;
    dk::jump(at_Obj_SpawnEnemy);
}
