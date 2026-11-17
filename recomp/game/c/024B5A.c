#include "game.h"

/* the thug's a2 script (ROM data right after this spawner's code; batmanrobin.asm) */
static constexpr uint32_t Scr2_024B66 = 0x024B66u;

/* Enemy_BarrelThug_SpawnC - lift trigger callback: the barrel thug variant (drop action $25) that makes its
 * first move after a random $F0-$1E0 wait
 * In:  x/y = where the trigger entry places it
 * Out: script = Scr2_024B66, then Obj_SpawnEnemy: Z set and a6 = the thug when it spawned,
 *      Z clear when the pool is short (d0 is clobbered: no caller reads it)
 * Evidence: lea Scr2_024B66,a2 / jmp Obj_SpawnEnemy; dc.l at $02139E and $0219D4-$0219EC in
 *           Tbl_02105A; body $024CC8; TAS f28037-f30860
 */
void Enemy_BarrelThug_SpawnC(uint16_t *d1, uint16_t x, uint16_t y, uint32_t *d7, uint32_t *a0, uint32_t *script, uint32_t *a3, uint32_t *a4, uint32_t *a5, uint32_t *a6)
{
    *script = Scr2_024B66;
    dk::jump(at_Obj_SpawnEnemy);   /* bra: Obj_SpawnEnemy returns to our caller */
}
