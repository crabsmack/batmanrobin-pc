#include "game.h"

/* the pickup's a2 script (ROM data right after this spawner's code; batmanrobin.asm) */
static constexpr uint32_t Scr2_013882 = 0x013882u;

/* Item_SpawnRapidCharge - spawn action 21: drops the rapid-charge pickup (fast weapon charge for
 * $2D0 frames, sound $48) where the object died (motion coroutine $133AE, then the common pickup
 * body $132D0)
 * In:  x/y = where the pickup appears (SpawnAction_Run passes the dying object's position)
 * Out: script = Scr2_013882, then Obj_SpawnEnemy: d0 = 0 and a6 = the pickup, d0 nonzero when
 *      the pool is short (the other registers pass through unchanged)
 * Evidence: lea Scr2_013882,a2 / jmp Obj_SpawnEnemy; Tbl_012EF0 entry 21; Scr2_013882: coroutine 1
 *           = $133AE, touch coroutine at $138CE plays the sound and calls the collect native; 9 TAS
 *           frames (1541-127837)
 */
void Item_SpawnRapidCharge(uint32_t *result, uint16_t *d1, uint16_t x, uint16_t y, uint32_t *d7, uint32_t a4, uint32_t a6, uint32_t *script)
{
    *script = Scr2_013882;
    dk::jump(at_Obj_SpawnEnemy);
}
