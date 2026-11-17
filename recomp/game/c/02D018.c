#include "game.h"

/* the object's a2 script (ROM data right after this spawner's code; batmanrobin.asm) */
static constexpr uint32_t Scr2_02D024 = 0x02D024u;

/* Enemy_DomeCraft_SpawnFromLeft - wave spawner: a green-domed craft (Scr2_02D024, body $280EC:
 * energy $800, contact attack box with knockback) that enters from the left edge (sprite attr
 * $F808) and holds windows right of the player ($2580 ticks each), then leaves right; fires 5-way
 * fans (headings $380-$480, a4 script $298F0) and then launches 7 spinning mines upward ($299B8)
 * In:  x/y = where the trigger or wave entry places it
 * Out: script = Scr2_02D024, then Obj_SpawnEnemy: Z set and a6 = the new object when it spawned, Z
 *      clear when the pool is short (d0 is clobbered: no caller reads it)
 * Evidence: lea Scr2_02D024,a2 / jmp Obj_SpawnEnemy; Scr5_021D3A +$2D4 (x -$A, screen y $16, drop
 *           action 8); timer 1 = $3C0; Fx_SpawnAtOfsAngle x5 per volley; f44400: the craft beside
 *           the player's glider; TAS f44261
 */
void Enemy_DomeCraft_SpawnFromLeft(uint16_t *d1, uint16_t x, uint16_t y, uint32_t *d7, uint32_t *script, uint32_t *a3, uint32_t *a4, uint32_t *a6)
{
    *script = Scr2_02D024;
    dk::jump(at_Obj_SpawnEnemy);   /* jmp: Obj_SpawnEnemy returns to our caller */
}
