#include "game.h"

/* the thug's a2 script (ROM data right after this spawner's code; batmanrobin.asm) */
static constexpr uint32_t Scr2_024C6C = 0x024C6Cu;

/* Enemy_GreenThug_SpawnA - lift trigger callback: the green-suited thug without a barrel (body $024CD8, a
 * hit handler whose death explodes without dropping one), drop action $25, random $F0-$1E0
 * wait
 * In:  x/y = where the trigger entry places it
 * Out: script = Scr2_024C6C, then Obj_SpawnEnemy: Z set and a6 = the thug when it spawned,
 *      Z clear when the pool is short (d0 is clobbered: no caller reads it)
 * Evidence: lea Scr2_024C6C,a2 / jmp Obj_SpawnEnemy; dc.l at $0211D0-$021288 in Tbl_02105A;
 *           TAS f22330-f26678 (8 spawns); RAM f24840: two objects with coroutine 2 = $024D4C
 *           = the two thugs on the girder in shot f24840
 */
void Enemy_GreenThug_SpawnA(uint16_t *d1, uint16_t x, uint16_t y, uint32_t *d7, uint32_t *a0, uint32_t *script, uint32_t *a3, uint32_t *a4, uint32_t *a5, uint32_t *a6)
{
    *script = Scr2_024C6C;
    dk::jump(at_Obj_SpawnEnemy);   /* bra: Obj_SpawnEnemy returns to our caller */
}
