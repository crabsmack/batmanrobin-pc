#include "game.h"

/* the pickup's a2 script (ROM data right after this spawner's code; batmanrobin.asm) */
static constexpr uint32_t Scr2_013AE6 = 0x013AE6u;

/* Item_SpawnWeapon2Drifting - spawn action 14: the weapon type 2 pickup (value 3) for the flying sections, drifting in sideways
 * In:  x/y = where the pickup appears (SpawnAction_Run passes the dying object's position)
 * Out: script = Scr2_013AE6, then Obj_SpawnEnemy: d0 = 0 and a6 = the pickup, d0 nonzero when
 *      the pool is short (the other registers pass through unchanged)
 * Evidence: lea Scr2_013AE6,a2 / jmp Obj_SpawnEnemy; Tbl_012EF0 entry 14; Scr2_013AE6 = coroutine $133CA, table $13C6E (fixed value 3); TAS
 *           frame 125959
 */
void Item_SpawnWeapon2Drifting(uint32_t *result, uint16_t *d1, uint16_t x, uint16_t y, uint32_t *d7, uint32_t a4, uint32_t a6, uint32_t *script)
{
    *script = Scr2_013AE6;
    dk::jump(at_Obj_SpawnEnemy);
}
