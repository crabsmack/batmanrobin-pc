#include "game.h"

/* the a2 script the spawned object runs (ROM data right after this spawner's code; batmanrobin.asm) */
static constexpr uint32_t Scr2_01BF6A = 0x01BF6Au;

/* Enemy_Gunman_SpawnWalkIn - spawns the trench-coat gunman in palette line 2 (energy $40, score
 * $FA) that walks in and keeps $50-$78 px from its target player
 * In:  d4/d5 = x/y (Tbl_0166EA +$430..+$56A)
 * Out: script = Scr2_01BF6A, then Obj_SpawnEnemy's (d0 = 0 and a6 = the new object; Z clear
 *      when the pool is short)
 * Evidence: lea Scr2_01BF6A,a2 / jmp Obj_SpawnEnemy; Scr2_01BF6A: ModSprAttr $98/$40, SetEnergy
 *           $40, SetRandJitter $20, coroutine 0 = $1C0A0 (BalancePlayerTarget, anim $10A,
 *           Op2_HoldRangeOfPlayerX $50,$78); TAS f1528-f9385
 */
void Enemy_Gunman_SpawnWalkIn(uint16_t *d1, uint16_t d4, uint16_t d5, uint32_t *d7,
                              uint32_t *script, uint32_t *a3, uint32_t *a4, uint32_t *a6)
{
    *script = Scr2_01BF6A;
    dk::jump(at_Obj_SpawnEnemy);
}
