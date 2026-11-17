#include "game.h"

/* the object's a2 script (ROM data right after this spawner's code; batmanrobin.asm) */
static constexpr uint32_t Scr2_02D2E2 = 0x02D2E2u;

/* Enemy_DomeCraft_SpawnB - wave spawner: the longer green-domed craft course (Scr2_02D2E2, body
 * $280EC): rises from below the screen, holds windows for $2580 then $12C0 ticks, leaves left;
 * fires bursts of 6 shot requests 6 times, then launches 7 spinning mines downward, and loops
 * In:  x/y = where the trigger or wave entry places it
 * Out: script = Scr2_02D2E2, then Obj_SpawnEnemy: Z set and a6 = the new object when it spawned, Z
 *      clear when the pool is short (d0 is clobbered: no caller reads it)
 * Evidence: lea Scr2_02D2E2,a2 / jmp Obj_SpawnEnemy; Scr5_021D3A +$480/+$488 (x $118, screen y
 *           $110); f65000 shows the craft (green dome, vertical bar, thrusters) next to the player;
 *           TAS f64940
 */
void Enemy_DomeCraft_SpawnB(uint16_t *d1, uint16_t x, uint16_t y, uint32_t *d7, uint32_t *script, uint32_t *a3, uint32_t *a4, uint32_t *a6)
{
    *script = Scr2_02D2E2;
    dk::jump(at_Obj_SpawnEnemy);   /* jmp: Obj_SpawnEnemy returns to our caller */
}
