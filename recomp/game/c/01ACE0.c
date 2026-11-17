#include "game.h"

/* the a2 script the spawned object runs (ROM data right after this spawner's code; batmanrobin.asm) */
static constexpr uint32_t Scr2_01ACEC = 0x01ACECu;

/* Enemy_Brute_SpawnFromSign - spawns the big sack-carrying brute that leaps out of a blasted
 * bank sign: palette line 3, energy $D0, its entry coroutine starts with a jump (height $23,
 * time $F0)
 * In:  d4/d5 = x/y (trigger entry)
 * Out: script = Scr2_01ACEC, then Obj_SpawnEnemy's (d0 = 0 and a6 = the new object; Z clear
 *      when the pool is short)
 * Evidence: lea Scr2_01ACEC,a2 / jmp Obj_SpawnEnemy; Scr2_01ACEC = ModSprAttr $98/$60,
 *           SetScoreValue $96, SetEnergy $D0, coroutine 0 = $1B012 (Op2_JumpAnim $23,$F0), then
 *           the shared brute body $1AF74 (vector 2); Tbl_015F5A entries chain
 *           Stage1_BlastSignChained -> this; TAS f2896-f4284, shot f2940 (brute in front of the
 *           blasted sign)
 */
void Enemy_Brute_SpawnFromSign(uint32_t *d0, uint16_t *d1, uint16_t d4, uint16_t d5, uint32_t *d7,
                               uint32_t *a0, uint32_t *script, uint32_t *a3, uint32_t *a4,
                               uint32_t *a5, uint32_t *a6)
{
    *script = Scr2_01ACEC;
    dk::jump(at_Obj_SpawnEnemy);
}
