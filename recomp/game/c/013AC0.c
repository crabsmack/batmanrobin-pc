#include "game.h"

/* the pickup's a2 script (ROM data right after this spawner's code; batmanrobin.asm) */
static constexpr uint32_t Scr2_013ACC = 0x013ACCu;

/* Item_SpawnWeapon1Drifting - spawn action 11: the weapon type 1 pickup (value 2) for the flying sections, drifting in sideways
 * In:  x/y = where the pickup appears (SpawnAction_Run passes the dying object's position)
 * Out: script = Scr2_013ACC, then Obj_SpawnEnemy: d0 = 0 and a6 = the pickup, d0 nonzero when
 *      the pool is short (the other registers pass through unchanged)
 * Evidence: lea Scr2_013ACC,a2 / jmp Obj_SpawnEnemy; Tbl_012EF0 entry 11; Scr2_013ACC = coroutine $133CA, table $13C68 (fixed value 2); 2 fuzz
 *           runs only
 */
void Item_SpawnWeapon1Drifting(uint32_t *result, uint16_t *d1, uint16_t x, uint16_t y, uint32_t *d7, uint32_t a4, uint32_t a6, uint32_t *script)
{
    *script = Scr2_013ACC;
    dk::jump(at_Obj_SpawnEnemy);
}
