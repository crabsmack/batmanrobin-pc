#include "game.h"

/* the object's a2 script (ROM data right after this spawner's code; batmanrobin.asm) */
static constexpr uint32_t Scr2_029D18 = 0x029D18u;

/* Enemy_Jet_SpawnMissileLauncher - wave spawner: a rocket jet (Scr2_029D18, same body and random
 * pickup) that flies in, follows the player's height, then fires a fan of 4 homing missiles and
 * U-turns away
 * In:  x/y = where it starts (d4/d5)
 * Out: script = Scr2_029D18, then Obj_SpawnEnemy: Z set and a6 = the new object when it spawned, Z
 *      clear when the pool is short (d0 is clobbered: no caller reads it)
 * Evidence: lea Scr2_029D18,a2 / jmp Obj_SpawnEnemy; 36 entries in Scr5_022C7E (+$2E2..) and
 *           Tbl_022FA4; four GosubArgs ($29D84-$29DA2: headings $240/$1C0/$40/$3C0) into $29DAE,
 *           which calls Spawn_AtOwnOfs with Enemy_Missile_SpawnFromJet unless a player is within
 *           $1E px; exit $2865A (heading + $200, no body box); TAS 43060-62428
 */
void Enemy_Jet_SpawnMissileLauncher(uint16_t *d1, uint16_t x, uint16_t y, uint32_t *d7, uint32_t *script, uint32_t *a3, uint32_t *a4, uint32_t *a6)
{
    *script = Scr2_029D18;
    dk::jump(at_Obj_SpawnEnemy);   /* jmp: Obj_SpawnEnemy returns to our caller */
}
