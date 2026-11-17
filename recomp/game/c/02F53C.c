#include "game.h"

/* the object's a2 script (ROM data right after this spawner's code; batmanrobin.asm) */
static constexpr uint32_t Scr2_02F54C = 0x02F54Cu;

/* Enemy_Missile_SpawnFromPodB - spawns a homing missile like Enemy_Missile_SpawnFromPodA with a
 * wider turn (Scr2_02F54C: cruise $300, turn rate $800)
 * In:  x/y = the launch point (Spawn_AtOwnOfs: the launcher's position plus the offset); a6 = the
 *      launcher (Obj_SpawnEnemyOrQuit's parent); g_VM2Arg0-2 = heading, straight frames, homing
 *      frames
 * Out: Obj_SpawnEnemyOrQuit with Scr2_02F54C (d1/d7, motion/vm = the new missile's blocks), then
 *      Enemy_Missile_InitFromArgs: heading, vars 0/1 from the args, d0 = 0; a short pool skips the
 *      init: Obj_SpawnEnemyOrQuit returns straight to Spawn_AtOwnOfs with d0 nonzero
 * Evidence: lea Scr2_02F54C,a2 / jsr Obj_SpawnEnemyOrQuit / jmp Enemy_Missile_InitFromArgs; dc.l in
 *           4 Spawn_AtOwnOfs calls at $2F4AE-$2F50A in the same attack subroutines; jmp
 *           Enemy_Missile_InitFromArgs; fuzz-45000-s1/s2 only (f46700-48588). The prototype's a3/a4
 *           become in/out (measured clobbers: no caller reads them) so the jump hands the new
 *           missile's blocks on
 */
void Enemy_Missile_SpawnFromPodB(uint32_t *d0, uint16_t *d1, uint16_t x, uint16_t y, uint32_t *d7, uint32_t *motion, uint32_t *vm, uint32_t a6)
{
    /* Obj_Spawn sets a0 (the run list) and a5 itself: the values going in are not read; a6
       (the new missile) is this prototype's clobber */
    uint32_t list = 0, sprite = 0, self = a6;
    Obj_SpawnEnemyOrQuit(d0, d1, x, y, d7, &list, Scr2_02F54C, motion, vm, &sprite, &self);
    dk::jump(at_Enemy_Missile_InitFromArgs);   /* jmp: its a3/a4 = the new missile's blocks */
}
