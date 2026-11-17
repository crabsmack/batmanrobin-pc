#include "game.h"

/* the thug's a2 script (ROM data right after this spawner's code; batmanrobin.asm) */
static constexpr uint32_t Scr2_024B24 = 0x024B24u;

/* Enemy_BarrelThug_SpawnB - lift trigger callback: the barrel thug variant whose script drops action $24 and
 * makes its first move after a $5A0 timer
 * In:  x/y = where the trigger entry places it
 * Out: script = Scr2_024B24, then Obj_SpawnEnemy: Z set and a6 = the thug when it spawned,
 *      Z clear when the pool is short (d0 is clobbered: no caller reads it)
 * Evidence: lea Scr2_024B24,a2 / jmp Obj_SpawnEnemy; dc.l at $0219F4 in Tbl_02105A; body $024CC8
 *           (as Enemy_BarrelThug_SpawnA); TAS f27317-f31022 (7 spawns)
 */
void Enemy_BarrelThug_SpawnB(uint16_t *d1, uint16_t x, uint16_t y, uint32_t *d7, uint32_t *a0, uint32_t *script, uint32_t *a3, uint32_t *a4, uint32_t *a5, uint32_t *a6)
{
    *script = Scr2_024B24;
    dk::jump(at_Obj_SpawnEnemy);   /* bra: Obj_SpawnEnemy returns to our caller */
}
