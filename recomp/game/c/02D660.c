#include "game.h"

/* the object's a2 script (ROM data right after this spawner's code; batmanrobin.asm) */
static constexpr uint32_t Scr2_02D66C = 0x02D66Cu;

/* Enemy_Biplane_SpawnShuttle - wave spawner: a red biplane (Scr2_02D66C) that flies 180 px in, then
 * shuttles +-12 px left/right while its speed rises from $355 to $6AA, then dashes 600 px left off
 * the screen
 * In:  x/y = where the trigger or wave entry places it
 * Out: script = Scr2_02D66C, then Obj_SpawnEnemy: Z set and a6 = the new object when it spawned, Z
 *      clear when the pool is short (d0 is clobbered: no caller reads it)
 * Evidence: lea Scr2_02D66C,a2 / jmp Obj_SpawnEnemy; 27 entries in Scr5_023C78 (+$FA.. columns of 9
 *           at x $154, y $2026-$2166 step $28, and +$3AC..), all kill-group members (action 1);
 *           fuzz runs only
 */
void Enemy_Biplane_SpawnShuttle(uint16_t *d1, uint16_t x, uint16_t y, uint32_t *d7, uint32_t *script, uint32_t *a3, uint32_t *a4, uint32_t *a6)
{
    *script = Scr2_02D66C;
    dk::jump(at_Obj_SpawnEnemy);   /* jmp: Obj_SpawnEnemy returns to our caller */
}
