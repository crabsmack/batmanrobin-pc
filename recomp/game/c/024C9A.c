#include "game.h"

/* the thug's a2 script (ROM data right after this spawner's code; batmanrobin.asm) */
static constexpr uint32_t Scr2_024CA6 = 0x024CA6u;

/* Enemy_GreenThug_SpawnB - lift trigger callback: the green-suited thug variant (drop action $24) that only
 * cycles facing the targeted player ($024E34) and jumping ($024EC0) every $1E0
 * In:  x/y = where the trigger entry places it
 * Out: script = Scr2_024CA6, then Obj_SpawnEnemy: Z set and a6 = the thug when it spawned,
 *      Z clear when the pool is short (d0 is clobbered: no caller reads it)
 * Evidence: lea Scr2_024CA6,a2 / jmp Obj_SpawnEnemy; dc.l at $02123A, $021296, $021300 ...
 *           $02194A in Tbl_02105A; TAS f22959-f30352 (23 spawns, the most common)
 */
void Enemy_GreenThug_SpawnB(uint16_t *d1, uint16_t x, uint16_t y, uint32_t *d7, uint32_t *a0, uint32_t *script, uint32_t *a3, uint32_t *a4, uint32_t *a5, uint32_t *a6)
{
    *script = Scr2_024CA6;
    dk::jump(at_Obj_SpawnEnemy);   /* bra: Obj_SpawnEnemy returns to our caller */
}
