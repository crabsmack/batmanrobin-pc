#include "game.h"

/* the spawned object's a2 script (ROM data right after this spawner's code; batmanrobin.asm) */
static constexpr uint32_t Scr2_02D414 = 0x02D414u;

/* Enemy_DomeCraft_SpawnC - wave spawner: Enemy_DomeCraft_SpawnA's green-domed craft (Scr2_02D414,
 * body $280EC) holding its windows (first y -$14..+$14, x +$78..+$C8) for $9600 ticks, not $1680
 * In:  d4/d5 = x/y (the wave entry's)
 * Out: script = Scr2_02D414, then as Obj_SpawnEnemy (d0 = 0 and a6 = the new object; nonzero d0
 *      when the pool is short)
 * Evidence: spawner long(s) at $0221E6; lea script / jmp Obj_SpawnEnemy as its siblings; raw until
 *           crane-crash (2026-10-30): no corpus run spawns it; 02D408.syn checks it
 */
void Enemy_DomeCraft_SpawnC(uint16_t *d1, uint16_t x, uint16_t y, uint32_t *d7, uint32_t *script, uint32_t *a3, uint32_t *a4, uint32_t *a6)
{
    *script = Scr2_02D414;
    dk::jump(at_Obj_SpawnEnemy);   /* jmp: Obj_SpawnEnemy returns to our caller */
}
