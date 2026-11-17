#include "game.h"

static constexpr uint32_t Scr2_011B3E = 0x011B3Eu;   /* the meter: BossMeter_Init, HUD_LockJoin, then its draw loop */

/* BossMeter_SpawnObject - spawns the boss energy meter as a script object running Scr2_011B3E
 * (Obj_SpawnEnemy, the enemy owner class).
 * In:  d1/d4/d5/d7/a4/a6 for Obj_SpawnEnemy
 * Out: as Obj_SpawnEnemy: d0 = 0 and a6 = the new object, or d0 = -1 (its flags left in place)
 * Evidence: lea Scr2_011B3E,a2 / jmp Obj_SpawnEnemy; only caller BossMeter_Spawn. The jmp is a
 *           call here: a2, its input, is a clobber of this contract, so a jump could not hand it on
 */
void BossMeter_SpawnObject(uint32_t *d0, uint16_t *d1, uint16_t d4, uint16_t d5, uint32_t *d7, uint32_t a4,
                           uint32_t *a6)
{
    uint32_t a0 = 0, a3 = 0, a5 = 0, r4 = a4;   /* clobbers of this contract */
    Obj_SpawnEnemy(d0, d1, d4, d5, d7, &a0, Scr2_011B3E, &a3, &r4, &a5, a6);
}
