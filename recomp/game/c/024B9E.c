#include "game.h"

/* the thug's a2 script (ROM data right after this spawner's code; batmanrobin.asm) */
static constexpr uint32_t Scr2_024BAA = 0x024BAAu;

/* Enemy_BarrelThug_SpawnD - lift trigger callback: the barrel thug variant (drop action $24) that waits a
 * $1E0 timer, then enters the random-wait cycle
 * In:  x/y = where the trigger entry places it
 * Out: script = Scr2_024BAA, then Obj_SpawnEnemy: Z set and a6 = the thug when it spawned,
 *      Z clear when the pool is short (d0 is clobbered: no caller reads it)
 * Evidence: lea Scr2_024BAA,a2 / jmp Obj_SpawnEnemy; dc.l at $0213C8 and $021A0C in Tbl_02105A;
 *           body $024CC8; TAS f28291-f31437
 */
void Enemy_BarrelThug_SpawnD(uint16_t *d1, uint16_t x, uint16_t y, uint32_t *d7, uint32_t *a0, uint32_t *script, uint32_t *a3, uint32_t *a4, uint32_t *a5, uint32_t *a6)
{
    *script = Scr2_024BAA;
    dk::jump(at_Obj_SpawnEnemy);   /* bra: Obj_SpawnEnemy returns to our caller */
}
