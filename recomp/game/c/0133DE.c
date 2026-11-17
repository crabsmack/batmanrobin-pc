#include "game.h"

/* the pickup's a2 script (ROM data right after this spawner's code; batmanrobin.asm) */
static constexpr uint32_t Scr2_0133EA = 0x0133EAu;

/* Item_SpawnBigHealth - spawn action 30: drops the large health pickup (+$80 health, sound $41)
 * where the object died (motion coroutine $133AE, then the common pickup body $132D0)
 * In:  x/y = where the pickup appears (SpawnAction_Run passes the dying object's position)
 * Out: script = Scr2_0133EA, then Obj_SpawnEnemy: d0 = 0 and a6 = the pickup, d0 nonzero when
 *      the pool is short (the other registers pass through unchanged)
 * Evidence: lea Scr2_0133EA,a2 / jmp Obj_SpawnEnemy; Tbl_012EF0 entry 30; Scr2_0133EA: coroutine 1
 *           = $133AE, touch coroutine at $13436 plays the sound and calls the collect native; 4
 *           fuzz runs (not in the TAS)
 */
void Item_SpawnBigHealth(uint32_t *result, uint16_t *d1, uint16_t x, uint16_t y, uint32_t *d7, uint32_t a4, uint32_t a6, uint32_t *script)
{
    *script = Scr2_0133EA;
    dk::jump(at_Obj_SpawnEnemy);
}
