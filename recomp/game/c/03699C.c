#include "game.h"

/* the spawned object's a2 script (ROM data right after this spawner's code; batmanrobin.asm) */
static constexpr uint32_t Scr2_0369A8 = 0x0369A8u;

/* Enemy_DropCarrier_SpawnRabbits - spawner of a drop carrier that crosses at screen height $84 and
 * drops hopping bomb rabbits ($035CF2) below itself (Scr2_0369A8: energy $100, cruise x $300)
 * In:  x/y = spawn position (d4/d5)
 * Out: script = Scr2_0369A8, then Obj_SpawnEnemy's (d0 = 0 and the new object's blocks;
 *      d0 nonzero when the pool is short)
 * Evidence: lea Scr2_0369A8,a2 / jmp Obj_SpawnEnemy; stage 3 trigger table Tbl_034142 (+$5D0, +$666, +$744); TAS f112595-f112729
 */
void Enemy_DropCarrier_SpawnRabbits(uint16_t *d1, uint16_t x, uint16_t y, uint32_t *d7, uint32_t *script, uint32_t *a3, uint32_t *a4, uint32_t *a6)
{
    *script = Scr2_0369A8;
    dk::jump(at_Obj_SpawnEnemy);
}
