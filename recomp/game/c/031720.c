#include "game.h"

/* the object's a2 script (ROM data right after this spawner's code; batmanrobin.asm) */
static constexpr uint32_t Scr2_031732 = 0x031732u;

/* Enemy_Missile_SpawnFast - spawns a fast homing missile (Scr2_031732: speed $100, cruise $3D5
 * reached over $78, turn rate $800, body Scr2_0287B0) with heading, straight and homing time from
 * the launcher's g_VM2Arg0-2
 * In:  x/y = the launch point (low words read; d4/d5 whole: their upper words come back as they
 *      came) (Spawn_AtOwnOfs, offset (10, 0)); a6 = the launcher (the new missile's parent);
 *      g_VM2Arg0-2
 * Out: Obj_SpawnEnemyOrQuit with Scr2_031732 (d1/d7, motion/vm = the new missile's blocks), then
 *      Enemy_Missile_InitFromArgs: heading, vars 0/1 from the args, d0 = 0; a short pool skips the
 *      init: Obj_SpawnEnemyOrQuit returns straight to Spawn_AtOwnOfs with d0 nonzero
 * Evidence: lea Scr2_031732,a2 / jsr Obj_SpawnEnemyOrQuit / jmp Enemy_Missile_InitFromArgs; dc.l in
 *           the launch subroutine $31710, gosub'd at $32D3A in the domes' script Scr2_032ACE; TAS
 *           73391-73464 (5). The prototype's a3/a4 become in/out (measured clobbers: no caller
 *           reads them) so the jump hands the new missile's blocks on; x/y are typed whole (the
 *           same registers): Obj_SpawnEnemyOrQuit is called through EnemyMissileSpawnFn so its
 *           short-pool return keeps their upper words (verify: generated FFFF00F2, C 000000F2 in d5
 *           with Obj.h's uint16_t x/y)
 */
void Enemy_Missile_SpawnFast(uint32_t *d0, uint16_t *d1, uint32_t x, uint32_t y, uint32_t *d7, uint32_t *motion, uint32_t *vm, uint32_t a6)
{
    /* Obj_Spawn sets a0 (the run list) and a5 itself: the values going in are not read; a6
       (the new missile) is this prototype's clobber */
    uint32_t list = 0, sprite = 0, self = a6;
    /* d4/d5 whole: the short-pool path returns from Obj_SpawnEnemyOrQuit past this function */
    EnemyMissileSpawnFn(at_Obj_SpawnEnemyOrQuit, d0, d1, x, y, d7, &list, Scr2_031732, motion, vm, &sprite, &self);
    dk::jump(at_Enemy_Missile_InitFromArgs);   /* jmp: its a3/a4 = the new missile's blocks */
}
