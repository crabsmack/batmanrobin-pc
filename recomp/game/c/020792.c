#include "game.h"

/* the object's a2 script (ROM, right after this spawner's code) */
static constexpr uint32_t Scr2_02079E = 0x02079Eu;

/* Stage1_SpawnGiftBox - trigger callback of the stage 1 parade-truck section: spawns a breakable gift
 * box (Scr2_02079E: energy $20, one front-drawn frame) that drops the item in the entry's top byte
 * In:  d4/d5 = the trigger's x/y
 * Out: script (a2) = Scr2_02079E, then as Obj_SpawnEnemy (d0 = 0 and a6 = the object, Z set; d0
 *      nonzero when the pool is short)
 * Evidence: lea Scr2_02079E,a2 / jmp Obj_SpawnEnemy; entries $0F/$15/$27/$1E/$28 << 24 | $020792 in Tbl_0166EA ($0166F6-$01674A;
 *           scene $1FECC); TAS f13247-f18552, level-skip f2961; RAM f13800: a Scr2_02079E object at
 *           x 1271 y 165 = the red present on the truck (shot f13790)
 */
void Stage1_SpawnGiftBox(uint16_t *d1, uint16_t d4, uint16_t d5, uint32_t *a0, uint32_t *a4, uint32_t *a5, uint32_t *a6,
                         uint32_t *script)
{
    *script = Scr2_02079E;
    dk::jump(at_Obj_SpawnEnemy);
}
