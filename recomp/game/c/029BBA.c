#include "game.h"

/* the object's a2 script (ROM data right after this spawner's code; batmanrobin.asm) */
static constexpr uint32_t Scr2_029BC6 = 0x029BC6u;

/* Enemy_Jet_SpawnTrackY - wave spawner: a rocket jet (Scr2_029BC6, body $285DE: energy $B0, fires
 * every $40 ticks) carrying a random pickup, that cruises left for one of four random times while
 * following the player's height
 * In:  x/y = where it starts (d4/d5)
 * Out: script = Scr2_029BC6, then Obj_SpawnEnemy: Z set and a6 = the new object when it spawned, Z
 *      clear when the pool is short (d0 is clobbered: no caller reads it)
 * Evidence: lea Scr2_029BC6,a2 / jmp Obj_SpawnEnemy; 18 entries in the wave script Scr5_022C7E (x
 *           $154, right of the screen); the script gosubs $29B76 (Op2_SwitchRandom over spawn
 *           actions $2D/$17/$1A/$E/$8/$B or none) and its course uses vector 2 = $27D1A
 *           (Op2_CruiseToPlayerY until timer 2); frames $3B38-$3B4C rendered: a silver rocket jet
 *           with a blue flame; TAS 38691-50838
 */
void Enemy_Jet_SpawnTrackY(uint16_t *d1, uint16_t x, uint16_t y, uint32_t *d7, uint32_t *script, uint32_t *a3, uint32_t *a4, uint32_t *a6)
{
    *script = Scr2_029BC6;
    dk::jump(at_Obj_SpawnEnemy);   /* jmp: Obj_SpawnEnemy returns to our caller */
}
