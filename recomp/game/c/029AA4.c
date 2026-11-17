#include "game.h"

/* the object's a2 script (ROM data right after this spawner's code; batmanrobin.asm) */
static constexpr uint32_t Scr2_029AB0 = 0x029AB0u;

/* Enemy_SpinMine_SpawnUp - spawns the spinning mine with Scr2_029AB0, identical to
 * Enemy_SpinMine_SpawnDown except that its first point is 20 px above the launch point
 * In:  x/y = where it starts (d4/d5)
 * Out: script = Scr2_029AB0, then Obj_SpawnEnemy (it returns to our caller: d0 and the flags its)
 * Evidence: lea Scr2_029AB0,a2 / jmp Obj_SpawnEnemy; dc.l in the a2 subroutine $299B8 (gosub'd by
 *           $2D024, $2E2A8, $2E5A6); the two scripts differ only in the first
 *           Op2_SetVector2HereGotoArgs2 offset (0, -$14) vs (0, +$14); fuzz-43200 / fuzz-77400
 *           only. The prototype gains &a2 (measured a clobber: no caller reads it) so the jump
 *           hands the script on
 */
void Enemy_SpinMine_SpawnUp(uint32_t *d0, uint16_t *d1, uint16_t x, uint16_t y, uint32_t *d7, uint32_t *script, uint32_t a4, uint32_t a6)
{
    *script = Scr2_029AB0;
    dk::jump(at_Obj_SpawnEnemy);   /* jmp: Obj_SpawnEnemy returns to our caller */
}
