#include "game.h"

/* the pickup's a2 script (ROM data right after this spawner's code; batmanrobin.asm) */
static constexpr uint32_t Scr2_013B00 = 0x013B00u;

/* Item_SpawnWeaponRandomDrifting - spawn action 17: a weapon pickup of random type 0-2 for the flying sections, drifting in sideways
 * In:  x/y = where the pickup appears (SpawnAction_Run passes the dying object's position)
 * Out: script = Scr2_013B00, then Obj_SpawnEnemy: d0 = 0 and a6 = the pickup, d0 nonzero when
 *      the pool is short (the other registers pass through unchanged)
 * Evidence: lea Scr2_013B00,a2 / jmp Obj_SpawnEnemy; Tbl_012EF0 entry 17; Scr2_013B00 = coroutine $133CA, table $13C74; 32 TAS frames
 *           (36482-107640)
 */
void Item_SpawnWeaponRandomDrifting(uint32_t *result, uint16_t *d1, uint16_t x, uint16_t y, uint32_t *d7, uint32_t a4, uint32_t a6, uint32_t *script)
{
    *script = Scr2_013B00;
    dk::jump(at_Obj_SpawnEnemy);
}
