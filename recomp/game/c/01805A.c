#include "game.h"

/* the a2 script the spawned object runs (ROM data right after this spawner's code; batmanrobin.asm) */
static constexpr uint32_t Scr2_018066 = 0x018066u;

/* Boss_Harley_Spawn - trigger callback at the end of the stage 1 street: spawns the Harley
 * mid-boss machine (boss class) running Scr2_018066
 * In:  d4/d5 = trigger position
 * Out: script = Scr2_018066, then Obj_SpawnBoss's (d0 = 0 and a6 = the new object; Z clear when
 *      the pool is short)
 * Evidence: lea Scr2_018066,a2 / jmp Obj_SpawnBoss; its long is the last entry of the street
 *           trigger table ($163B6 in Tbl_015F5A); TAS f4518 (BOSS TIME, then the balloon-topped
 *           cart rolls in: f4750); identified as Harley from the red/black jester that rises on
 *           the pole (f5850) and NOTES Game flow 'Harley mid-boss' (inferred)
 */
void Boss_Harley_Spawn(uint16_t *d1, uint16_t d4, uint16_t d5, uint32_t *a0, uint32_t *script,
                       uint32_t *a4, uint32_t *a5, uint32_t *a6)
{
    *script = Scr2_018066;
    dk::jump(at_Obj_SpawnBoss);
}
