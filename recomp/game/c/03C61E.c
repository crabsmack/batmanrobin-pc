#include "game.h"

/* the spawned object's a2 script (ROM data right after this spawner) */
static constexpr uint32_t Scr2_03C62A = 0x03C62Au;

/* Enemy_ArenaDebris_SpawnThud - spawner of arena debris (body $03B5EC) that hits the floor once
 * with a thud (coroutine 0 $03B684: random left/right cruise, random screen shake, sound $F, sprite
 * shake), bounces off and drops through (terrain box A disabled)
 * In:  d4/d5 = position (wave spawn entry)
 * Out: jumps to Obj_SpawnEnemy with a2 = Scr2_03C62A
 * Evidence: dc.l $03C61E in Scr5_03C4A8 (+$1C, +$3C, +$5C, +$7C) and Scr2_03C34C+$158; TAS
 *           f88125-f89327 (17 spawns)
 */
void Enemy_ArenaDebris_SpawnThud(uint16_t *d1, uint16_t x, uint16_t y, uint32_t *d7, uint32_t *script, uint32_t *a3, uint32_t *a4, uint32_t *a6)
{
    *script = Scr2_03C62A;
    dk::jump(at_Obj_SpawnEnemy);
}
