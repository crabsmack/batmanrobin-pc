#include "game.h"

/* the pickup's a2 script (ROM data right after this spawner's code; batmanrobin.asm) */
static constexpr uint32_t Scr2_0134EA = 0x0134EAu;

/* Item_SpawnSmallHealth - spawn action 33: drops a small health pickup where the object died
 * (motion coroutine $133AE, then the LARGE health pickup's body $1341E (+$80, sound $41): its Goto
 * lands on Item_SpawnBigHealth's body, not the small one at $1351E)
 * In:  x/y = where the pickup appears (SpawnAction_Run passes the dying object's position)
 * Out: script = Scr2_0134EA, then Obj_SpawnEnemy: d0 = 0 and a6 = the pickup, d0 nonzero when the
 *      pool is short (the other registers pass through unchanged)
 * Evidence: lea Scr2_0134EA,a2 / jmp Obj_SpawnEnemy; Tbl_012EF0 entry 33; raw until crane-crash
 *           (2026-10-30): no corpus run spawns it; 0134DE.syn checks it
 */
void Item_SpawnSmallHealth(uint32_t *result, uint16_t *d1, uint16_t x, uint16_t y, uint32_t *d7, uint32_t a4, uint32_t a6, uint32_t *script)
{
    *script = Scr2_0134EA;
    dk::jump(at_Obj_SpawnEnemy);
}
