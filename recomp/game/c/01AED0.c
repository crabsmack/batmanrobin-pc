#include "game.h"

/* the a2 script the spawned object runs (ROM data right after this spawner's code; batmanrobin.asm) */
static constexpr uint32_t Scr2_01AEDC = 0x01AEDCu;

/* Enemy_Brute_SpawnToughAlt - spawns the tougher brute (palette line 2, energy $120) whose
 * attack loop starts at $1B060 (anim $2FC) instead of $1B0CA
 * In:  d4/d5 = x/y (wave script Tbl_0174F0 +$D4/+$DC)
 * Out: script = Scr2_01AEDC, then Obj_SpawnEnemy's (d0 = 0 and a6 = the new object; Z clear
 *      when the pool is short)
 * Evidence: lea Scr2_01AEDC,a2 / jmp Obj_SpawnEnemy; Scr2_01AEDC differs from Scr2_01AD68 only
 *           in the vector-2 +8 goto ($1B060: StartCoroutine0Rel, SetAnim $2FC, GosubVector2Plus
 *           4); TAS f8124-f8222 (Gem Expo)
 */
void Enemy_Brute_SpawnToughAlt(uint16_t *d1, uint16_t d4, uint16_t d5, uint32_t *d7,
                               uint32_t *script, uint32_t *a3, uint32_t *a4, uint32_t *a6)
{
    *script = Scr2_01AEDC;
    dk::jump(at_Obj_SpawnEnemy);
}
