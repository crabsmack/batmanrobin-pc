#include "game.h"

/* the pickup's a2 script (ROM data right after this spawner's code; batmanrobin.asm) */
static constexpr uint32_t Scr2_0135C6 = 0x0135C6u;

/* Item_SpawnExtraLifeDrifting - spawn action 29: the extra-life pickup (lives +1, sound $42) for
 * the flying sections: drifts sideways at cruise speed $AA (motion coroutine $133CA, body $13316)
 * In:  x/y = where the pickup appears (SpawnAction_Run passes the dying object's position)
 * Out: script = Scr2_0135C6, then Obj_SpawnEnemy: d0 = 0 and a6 = the pickup, d0 nonzero when
 *      the pool is short (the other registers pass through unchanged)
 * Evidence: lea Scr2_0135C6,a2 / jmp Obj_SpawnEnemy; Tbl_012EF0 entry 29; Scr2_0135C6: coroutine 1
 *           = $133CA (Op2_SetAccelXForDist/Op2_SetCruiseX $AA), a larger off-screen box than
 *           $132D0; TAS frame 74543
 */
void Item_SpawnExtraLifeDrifting(uint32_t *result, uint16_t *d1, uint16_t x, uint16_t y, uint32_t *d7, uint32_t a4, uint32_t a6, uint32_t *script)
{
    *script = Scr2_0135C6;
    dk::jump(at_Obj_SpawnEnemy);
}
