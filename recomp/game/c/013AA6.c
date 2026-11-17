#include "game.h"

/* the pickup's a2 script (ROM data right after this spawner's code; batmanrobin.asm) */
static constexpr uint32_t Scr2_013AB2 = 0x013AB2u;

/* Item_SpawnWeapon0Drifting - spawn action 8: the weapon type 0 pickup (value 1) for the flying sections, drifting in sideways
 * In:  x/y = where the pickup appears (SpawnAction_Run passes the dying object's position)
 * Out: script = Scr2_013AB2, then Obj_SpawnEnemy: d0 = 0 and a6 = the pickup, d0 nonzero when
 *      the pool is short (the other registers pass through unchanged)
 * Evidence: lea Scr2_013AB2,a2 / jmp Obj_SpawnEnemy; Tbl_012EF0 entry 8; Scr2_013AB2 = coroutine $133CA, table $13C62 (fixed value 1), goto
 *           $13BC2; 4 TAS frames (37162-50001)
 */
void Item_SpawnWeapon0Drifting(uint32_t *result, uint16_t *d1, uint16_t x, uint16_t y, uint32_t *d7, uint32_t a4, uint32_t a6, uint32_t *script)
{
    *script = Scr2_013AB2;
    dk::jump(at_Obj_SpawnEnemy);
}
