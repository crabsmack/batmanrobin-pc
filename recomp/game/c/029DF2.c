#include "game.h"

/* the spawned object's a2 script (ROM data right after this spawner's code; batmanrobin.asm) */
static constexpr uint32_t Scr2_029DFE = 0x029DFEu;

/* Enemy_Jet_SpawnMissileLauncherB - wave spawner: a rocket jet like
 * Enemy_Jet_SpawnMissileLauncher's (Scr2_029DFE, the same body) whose hold time is one of
 * $2D0/$3C0/$5A0/$690 at random, without its screen-x range test
 * In:  d4/d5 = x/y (the wave entry's)
 * Out: script = Scr2_029DFE, then as Obj_SpawnEnemy (d0 = 0 and a6 = the new object; nonzero d0
 *      when the pool is short)
 * Evidence: spawner long(s) at 12 wave entries $023976-$023A80; lea script / jmp Obj_SpawnEnemy as
 *           its siblings; raw until crane-crash (2026-10-30): no corpus run spawns it; 029DF2.syn
 *           checks it
 */
void Enemy_Jet_SpawnMissileLauncherB(uint16_t *d1, uint16_t x, uint16_t y, uint32_t *d7, uint32_t *script, uint32_t *a3, uint32_t *a4, uint32_t *a6)
{
    *script = Scr2_029DFE;
    dk::jump(at_Obj_SpawnEnemy);   /* jmp: Obj_SpawnEnemy returns to our caller */
}
