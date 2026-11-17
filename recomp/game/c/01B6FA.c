#include "game.h"

/* the a2 script the spawned object runs (ROM data right after this spawner's code; batmanrobin.asm) */
static constexpr uint32_t Scr2_01B706 = 0x01B706u;

/* Enemy_Clown_SpawnRunIn - spawns a low-energy clown ($20, palette line 3) that does not drop
 * in: it targets a player, runs toward it and leaps (Op2_JumpAnim $46,$180)
 * In:  d4/d5 = x/y (Tbl_0166EA +$2B8..+$310)
 * Out: script = Scr2_01B706, then Obj_SpawnEnemy's (d0 = 0 and a6 = the new object; Z clear
 *      when the pool is short)
 * Evidence: lea Scr2_01B706,a2 / jmp Obj_SpawnEnemy; Scr2_01B706: coroutine 0 = $1BB34
 *           (BalancePlayerTarget, GosubVector2Plus 4, FaceCruiseToPlayerXTurnBranch,
 *           CruiseForward, JumpAnim); a raw spawner for $1B760 (never run) follows at $1B754;
 *           TAS f638-f9043
 */
void Enemy_Clown_SpawnRunIn(uint16_t *d1, uint16_t d4, uint16_t d5, uint32_t *d7, uint32_t *script,
                            uint32_t *a3, uint32_t *a4, uint32_t *a6)
{
    *script = Scr2_01B706;
    dk::jump(at_Obj_SpawnEnemy);
}
