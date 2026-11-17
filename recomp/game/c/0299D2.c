#include "game.h"

/* the object's a2 script (ROM data right after this spawner's code; batmanrobin.asm) */
static constexpr uint32_t Scr2_0299DE = 0x0299DEu;

/* Enemy_SpinMine_SpawnDown - spawns a spinning mine (Scr2_0299DE, body $28468: energy $40, touch
 * damage) that first flies to a point 20 px below where it was launched, then makes random swoops
 * and homes on the player
 * In:  x/y = where it starts (d4/d5)
 * Out: script = Scr2_0299DE, then Obj_SpawnEnemy (it returns to our caller: d0 and the flags its)
 * Evidence: lea Scr2_0299DE,a2 / jmp Obj_SpawnEnemy; dc.l in the a2 subroutine $2999E
 *           (Spawn_AtOwnPos unless a player box is within $1E px), gosub'd by the scripts
 *           $2D1AA/$2D2E2 and the beam ship's attacks $2E2A8/$2E5A6, and by the third turret's
 *           script $2F088; frames $3A20-$3A52 rendered: a dark orb that grows and spins a blade;
 *           fuzz-41400 / fuzz-64800 only. The prototype gains &a2 (measured a clobber: no caller
 *           reads it) so the jump hands the script on
 */
void Enemy_SpinMine_SpawnDown(uint32_t *d0, uint16_t *d1, uint16_t x, uint16_t y, uint32_t *d7, uint32_t *script, uint32_t a4, uint32_t a6)
{
    *script = Scr2_0299DE;
    dk::jump(at_Obj_SpawnEnemy);   /* jmp: Obj_SpawnEnemy returns to our caller */
}
