#include "game.h"

/* the thug's a2 script (ROM data right after this spawner's code; batmanrobin.asm) */
static constexpr uint32_t Scr2_024BEC = 0x024BECu;

/* Enemy_BarrelThug_SpawnE - the barrel thug variant (drop action $25) whose first move comes after a $528
 * timer and whose cycle timer is a fixed $780 (the others draw $3C0-$780 at random)
 * In:  x/y = where the trigger entry places it
 * Out: script = Scr2_024BEC, then Obj_SpawnEnemy: Z set and a6 = the thug when it spawned,
 *      Z clear when the pool is short (d0 is clobbered: no caller reads it)
 * Evidence: lea Scr2_024BEC,a2 / jmp Obj_SpawnEnemy; dc.l at $0219FC (Tbl_02105A) and $021D02
 *           (wave script Tbl_021B26); body $024CC8; TAS f28300-f30260
 */
void Enemy_BarrelThug_SpawnE(uint16_t *d1, uint16_t x, uint16_t y, uint32_t *d7, uint32_t *script, uint32_t *a3, uint32_t *a4, uint32_t *a6)
{
    *script = Scr2_024BEC;
    dk::jump(at_Obj_SpawnEnemy);   /* bra: Obj_SpawnEnemy returns to our caller */
}
