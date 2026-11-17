#include "game.h"

/* the a2 script the spawned object runs (ROM data right after this spawner's code; batmanrobin.asm) */
static constexpr uint32_t Scr2_01B69A = 0x01B69Au;

/* Enemy_Clown_SpawnGreen - spawns the grey-green clown (palette line 2, energy $80) that drops
 * in and picks one of three approach ranges at random
 * In:  d4/d5 = x/y (Tbl_0166EA +$7F0.., Tbl_0174F0 +$14/+$1C)
 * Out: script = Scr2_01B69A, then Obj_SpawnEnemy's (d0 = 0 and a6 = the new object; Z clear
 *      when the pool is short)
 * Evidence: lea Scr2_01B69A,a2 / jmp Obj_SpawnEnemy; Scr2_01B69A = ModSprAttr $98/$40,
 *           SetEnergy $80, Op2_SwitchRandom with 3 cases; RAM f3780: a5 attr $C000 (palette 2)
 *           energy $80 on the grey-green clown of shot f3780; TAS f3515-f9504
 */
void Enemy_Clown_SpawnGreen(uint16_t *d1, uint16_t d4, uint16_t d5, uint32_t *d7, uint32_t *script,
                            uint32_t *a3, uint32_t *a4, uint32_t *a6)
{
    *script = Scr2_01B69A;
    dk::jump(at_Obj_SpawnEnemy);
}
