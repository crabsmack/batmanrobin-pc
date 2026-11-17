#include "game.h"

/* the a2 script the spawned object runs (ROM data right after this spawner's code; batmanrobin.asm) */
static constexpr uint32_t Scr2_01B7CC = 0x01B7CCu;

/* Enemy_Clown_Spawn - spawns the standard clown (palette line 3, energy $80) that drops in from
 * above; its prologue heads the shared clown body
 * In:  d4/d5 = x/y (Tbl_0166EA +$B2/+$21E, Tbl_0174F0 +$6B2/+$6BA)
 * Out: script = Scr2_01B7CC, then Obj_SpawnEnemy's (d0 = 0 and a6 = the new object; Z clear
 *      when the pool is short)
 * Evidence: lea Scr2_01B7CC,a2 / jmp Obj_SpawnEnemy; Scr2_01B7CC = ModSprAttr $98/$60,
 *           SetEnergy $80, coroutine 0 $1BAD6 (fall in); every clown variant runs the body in
 *           this span; TAS f13431-f18590
 */
void Enemy_Clown_Spawn(uint16_t *d1, uint16_t d4, uint16_t d5, uint32_t *d7, uint32_t *a0,
                       uint32_t *script, uint32_t *a3, uint32_t *a4, uint32_t *a5, uint32_t *a6)
{
    *script = Scr2_01B7CC;
    dk::jump(at_Obj_SpawnEnemy);
}
