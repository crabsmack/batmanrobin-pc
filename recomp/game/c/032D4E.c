#include "game.h"

/* the object's a2 script (ROM data right after this spawner's code; batmanrobin.asm) */
static constexpr uint32_t Scr2_032D5A = 0x032D5Au;

/* Boss_DomeCannon_SpawnBallTurret - spawns a ball turret of the dome cannon's second form as an
 * unlinked child: random speed, the ball-turret body $2FDE4 (energy $200/$480); its coroutine $32D64
 * fires 8 aimed shots $1E0 apart, then swaps target
 * In:  x/y = the parent's position (Op2_SpawnChildOrWait)
 * Out: script = Scr2_032D5A, then Obj_SpawnEnemyUnlinked: Z set and a6 = the new turret when it
 *      spawned, Z clear when the pool is short
 * Evidence: lea Scr2_032D5A,a2 / jmp Obj_SpawnEnemyUnlinked; dc.l of Op2_SpawnChildOrWait (fields
 *           $30/$26) in Scr2_031D3C; RAM f72600: two ball turrets (pc $2FE10, energy 512) beside
 *           the core; TAS f72564. The prototype gains &a2 (measured a clobber: Op2_SpawnChildOrWait
 *           saves and restores a2 around its jsr (a0)) so the jump hands the script on
 */
void Boss_DomeCannon_SpawnBallTurret(uint16_t x, uint16_t y, uint32_t *d7, uint32_t *script, uint32_t a4, uint32_t *a6)
{
    *script = Scr2_032D5A;
    dk::jump(at_Obj_SpawnEnemyUnlinked);   /* jmp: it returns to Op2_SpawnChildOrWait */
}
