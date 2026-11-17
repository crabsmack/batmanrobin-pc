#include "game.h"

/* the object's a2 script (ROM data right after this spawner's code; batmanrobin.asm) */
static constexpr uint32_t Scr2_031670 = 0x031670u;

/* Enemy_Missile_SpawnAgile - spawns a homing missile with a tight turn (Scr2_031670: speed $100,
 * cruise $3AA reached over $50, turn rate $A00, coroutine 0 $28836, body Scr2_0287B0) and sets its
 * heading, straight and homing times from the launcher's g_VM2Arg0-2
 * In:  x/y = the launch point (low words read; d4/d5 whole: their upper words come back as they
 *      came) (Spawn_AtOwnOfs, offset (13, 0)); a6 = the launcher (the new missile's parent);
 *      g_VM2Arg0-2 from the launcher's GosubArgs
 * Out: Obj_SpawnEnemyOrQuit with Scr2_031670 (d1/d7, motion/vm = the new missile's blocks), then
 *      Enemy_Missile_InitFromArgs: heading, vars 0/1 from the args, d0 = 0; a short pool skips the
 *      init: Obj_SpawnEnemyOrQuit returns straight to Spawn_AtOwnOfs with d0 nonzero
 * Evidence: lea Scr2_031670,a2 / jsr Obj_SpawnEnemyOrQuit / jmp Enemy_Missile_InitFromArgs; dc.l in
 *           the launch subroutine $3164E, gosub'd by Enemy_MissileDome_SpawnSingles ($315CC); not
 *           in the TAS, fuzz-68400-s1/s2 (f69175-71368). The prototype's a3/a4 become in/out
 *           (measured clobbers: no caller reads them) so the jump hands the new missile's blocks
 *           on; x/y are typed whole (the same registers): Obj_SpawnEnemyOrQuit is called through
 *           EnemyMissileSpawnFn so its short-pool return keeps their upper words (verify: generated
 *           FFFF00F2, C 000000F2 in d5 with Obj.h's uint16_t x/y)
 */
void Enemy_Missile_SpawnAgile(uint32_t *d0, uint16_t *d1, uint32_t x, uint32_t y, uint32_t *d7, uint32_t *motion, uint32_t *vm, uint32_t a6)
{
    /* Obj_Spawn sets a0 (the run list) and a5 itself: the values going in are not read; a6
       (the new missile) is this prototype's clobber */
    uint32_t list = 0, sprite = 0, self = a6;
    /* d4/d5 whole: the short-pool path returns from Obj_SpawnEnemyOrQuit past this function */
    EnemyMissileSpawnFn(at_Obj_SpawnEnemyOrQuit, d0, d1, x, y, d7, &list, Scr2_031670, motion, vm, &sprite, &self);
    dk::jump(at_Enemy_Missile_InitFromArgs);   /* jmp: its a3/a4 = the new missile's blocks */
}
