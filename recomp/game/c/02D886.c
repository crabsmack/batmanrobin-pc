#include "game.h"

/* the object's a2 script (ROM data right after this spawner's code; batmanrobin.asm) */
static constexpr uint32_t Scr2_02D892 = 0x02D892u;

/* Enemy_Biplane_SpawnZigzagDown - wave spawner: a red biplane (Scr2_02D892) that flies 160 px in,
 * then zigzags +-20 px with every leg 4 px lower while speeding up ($500-$655), then turns back up
 * and leaves left; paired with Enemy_Biplane_SpawnZigzagUp
 * In:  x/y = where the trigger or wave entry places it
 * Out: script = Scr2_02D892, then Obj_SpawnEnemy: Z set and a6 = the new object when it spawned, Z
 *      clear when the pool is short (d0 is clobbered: no caller reads it)
 * Evidence: lea Scr2_02D892,a2 / jmp Obj_SpawnEnemy; Scr5_023C78 +$1A0/+$20E/+$276 (x $154, y
 *           $20C4, right before ZigzagUp at the same point); TAS 47924-66203
 */
void Enemy_Biplane_SpawnZigzagDown(uint16_t *d1, uint16_t x, uint16_t y, uint32_t *d7, uint32_t *script, uint32_t *a3, uint32_t *a4, uint32_t *a6)
{
    *script = Scr2_02D892;
    dk::jump(at_Obj_SpawnEnemy);   /* jmp: Obj_SpawnEnemy returns to our caller */
}
