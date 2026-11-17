#include "game.h"

/* the a2 script the spawned object runs (ROM data right after this spawner's code; batmanrobin.asm) */
static constexpr uint32_t Scr2_01B760 = 0x01B760u;

/* Enemy_Clown_SpawnRanges - spawns a standard-palette clown (palette line 3, energy $80) with
 * Enemy_Clown_SpawnGreen's script: drops in and picks one of three approach ranges at random
 * In:  d4/d5 = x/y (the trigger entry's)
 * Out: script = Scr2_01B760, then Obj_SpawnEnemy's (d0 = 0 and a6 = the new object; Z clear when
 *      the pool is short)
 * Evidence: lea Scr2_01B760,a2 / jmp Obj_SpawnEnemy; Scr2_01B760 = Scr2_01B69A's words but
 *           Op2_ModSprAttr $9860 (line 3); trigger callback longs at $016F26 / $016F2E (Tbl_0166EA,
 *           the parade truck $1400); no corpus run reaches it (crane-crash 2026-10-30);
 *           01B754.syn checks it
 */
void Enemy_Clown_SpawnRanges(uint16_t *d1, uint16_t d4, uint16_t d5, uint32_t *d7, uint32_t *script,
                             uint32_t *a3, uint32_t *a4, uint32_t *a6)
{
    *script = Scr2_01B760;
    dk::jump(at_Obj_SpawnEnemy);
}
