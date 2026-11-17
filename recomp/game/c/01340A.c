#include "game.h"

/* the pickup's a2 script (ROM data right after this spawner's code; batmanrobin.asm) */
static constexpr uint32_t Scr2_013416 = 0x013416u;

/* Item_SpawnBigHealthDrifting - spawn action 32: the large health pickup (+$80 health, sound $41)
 * for the flying sections: drifts sideways at cruise speed $AA (motion coroutine $133CA, body
 * $13316)
 * In:  x/y = where the pickup appears (SpawnAction_Run passes the dying object's position)
 * Out: script = Scr2_013416, then Obj_SpawnEnemy: d0 = 0 and a6 = the pickup, d0 nonzero when
 *      the pool is short (the other registers pass through unchanged)
 * Evidence: lea Scr2_013416,a2 / jmp Obj_SpawnEnemy; Tbl_012EF0 entry 32; Scr2_013416: coroutine 1
 *           = $133CA (Op2_SetAccelXForDist/Op2_SetCruiseX $AA), a larger off-screen box than
 *           $132D0; TAS frame 71905 (a flying section, screenshot f71920)
 */
void Item_SpawnBigHealthDrifting(uint32_t *result, uint16_t *d1, uint16_t x, uint16_t y, uint32_t *d7, uint32_t a4, uint32_t a6, uint32_t *script)
{
    *script = Scr2_013416;
    dk::jump(at_Obj_SpawnEnemy);
}
