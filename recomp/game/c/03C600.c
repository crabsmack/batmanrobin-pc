#include "game.h"

/* the spawned object's a2 script (ROM data right after this spawner) */
static constexpr uint32_t Scr2_03C60C = 0x03C60Cu;

/* Enemy_ArenaDebris_SpawnBounce - spawner of a piece of debris that tumbles into the cat-head arena
 * at a random x (Op2_JitterPos $12C) and bounces twice on the floor (coroutine 0 $03B66A, terrain
 * box bottom $BE); shared body $03B5EC: starts at y -$14, falls (cruise y $6AA), one of three
 * random looks, smoke/spark effects every $60 ticks, attack box with Enemy_ArenaDebris_HitPlayer
 * In:  d4/d5 = position (wave spawn entry)
 * Out: jumps to Obj_SpawnEnemy with a2 = Scr2_03C60C
 * Evidence: dc.l $03C600 in the arena wave Scr5_03C4A8 (+$124, +$13C, +$154); TAS f88695-f89152 (16
 *           spawns); falling objects at f88800 (RAM: objects at Scr2_03B3FE+$22C) and on the floor
 *           at f089400
 */
void Enemy_ArenaDebris_SpawnBounce(uint16_t *d1, uint16_t x, uint16_t y, uint32_t *d7, uint32_t *script, uint32_t *a3, uint32_t *a4, uint32_t *a6)
{
    *script = Scr2_03C60C;
    dk::jump(at_Obj_SpawnEnemy);
}
