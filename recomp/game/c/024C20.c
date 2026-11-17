#include "game.h"

/* the thug's a2 script (ROM data right after this spawner's code; batmanrobin.asm) */
static constexpr uint32_t Scr2_024C2C = 0x024C2Cu;

/* Enemy_BarrelThug_SpawnF - the barrel thug variant (drop action $24) that makes its first move after a $7F8
 * timer
 * In:  x/y = where the trigger entry places it
 * Out: script = Scr2_024C2C, then Obj_SpawnEnemy: Z set and a6 = the thug when it spawned,
 *      Z clear when the pool is short (d0 is clobbered: no caller reads it)
 * Evidence: lea Scr2_024C2C,a2 / jmp Obj_SpawnEnemy; dc.l at $021A04 (Tbl_02105A),
 *           $021CFA/$021D0A (Tbl_021B26); body $024CC8; TAS f28309-f30261
 */
void Enemy_BarrelThug_SpawnF(uint16_t *d1, uint16_t x, uint16_t y, uint32_t *d7, uint32_t *script, uint32_t *a3, uint32_t *a4, uint32_t *a6)
{
    *script = Scr2_024C2C;
    dk::jump(at_Obj_SpawnEnemy);   /* bra: Obj_SpawnEnemy returns to our caller */
}
