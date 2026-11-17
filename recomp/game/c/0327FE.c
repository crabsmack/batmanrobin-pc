#include "game.h"

/* the object's a2 script (ROM data right after this spawner's code; batmanrobin.asm) */
static constexpr uint32_t Scr2_03280A = 0x03280Au;

/* Boss_DomeCannon_SpawnCore - spawns the dome-cannon core as an unlinked child: spawn action $2E,
 * then body $300EE (energy $1400/$2800 for 1P/2P, score $96: a large dome whose barrel turns toward
 * the targeted player; on death BossMeter_Defeated and a weapon-drop stop); coroutine $3015A,
 * started by the parent, shows the boss meter and starts a weapon drop
 * In:  x/y = the parent's position (Op2_SpawnChildOrWait)
 * Out: script = Scr2_03280A, then Obj_SpawnEnemyUnlinked: Z set and a6 = the new core when it
 *      spawned, Z clear when the pool is short
 * Evidence: lea Scr2_03280A,a2 / jmp Obj_SpawnEnemyUnlinked; dc.l of Op2_SpawnChildOrWait (field
 *           $2A) in the three Boss_DomeCannon_Spawn* scripts; RAM: energy 5120 at f70080 falling to
 *           0 by f71100; TAS 70036-75008 (3 spawns). The prototype gains &a2 (measured a clobber:
 *           Op2_SpawnChildOrWait saves and restores a2 around its jsr (a0)) so the jump hands the
 *           script on
 */
void Boss_DomeCannon_SpawnCore(uint16_t x, uint16_t y, uint32_t *d7, uint32_t *script, uint32_t a4, uint32_t *a6)
{
    *script = Scr2_03280A;
    dk::jump(at_Obj_SpawnEnemyUnlinked);   /* jmp: it returns to Op2_SpawnChildOrWait */
}
